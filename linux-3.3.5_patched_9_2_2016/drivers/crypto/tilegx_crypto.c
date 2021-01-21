/*
 * Copyright 2013 Tilera Corporation. All Rights Reserved.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 *   NON INFRINGEMENT.  See the GNU General Public License for
 *   more details.
 */

#include <asm/homecache.h>

#include <linux/crypto.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/scatterlist.h>
#include <linux/spinlock.h>
#include <linux/rtnetlink.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

#include <crypto/aes.h>
#include <crypto/algapi.h>
#include <crypto/aead.h>
#include <crypto/authenc.h>
#include <crypto/des.h>
#include <crypto/md5.h>
#include <crypto/sha.h>
#include <crypto/scatterwalk.h>

#include <gxio/gxcr.h>
#include <gxio/aead.h>
#include <gxio/token_aead.h>
#include <gxio/iorpc_mica.h>

static int tile_num_hw_contexts = 0;
static int irq = -1;
module_param(tile_num_hw_contexts, int, 0444);
MODULE_PARM_DESC(tile_num_hw_contexts,
		 "Number of MiCA hardware contexts to allocate");
MODULE_AUTHOR("Tilera Corporation");
MODULE_LICENSE("GPL");

#define JUMBO_PACKET_BUF_SIZE 65535

// synchronous mode
#define TGCRYPTO_SYNC 0

struct tile_crypto_alg {
	struct crypto_alg alg;
	gxcr_cipher_t cipher;
	gxcr_digest_t digest;
};

struct tile_hw_ctx {
	gxio_mica_context_t mica_ctx;
	unsigned char packet_buffer[JUMBO_PACKET_BUF_SIZE];
};

struct ctx_inf {
    char used;
    char encrypt;
    unsigned metadata_mem_size;
    unsigned char *metadata_mem;
    unsigned char *iv;
    struct aead_request *req;
    struct tile_hw_ctx *ctx;
    struct tasklet_struct bh_task;
};

#define TGCRYPTO_DEBUG_TIMER 0
#if TGCRYPTO_DEBUG_TIMER
static struct timer_list timer;
static void tile_timer(unsigned long data);
#endif
static void tile_interrupt_handler_bh(unsigned long data);

static struct ctx_inf *mica_ctx_pool;
static spinlock_t ctx_pool_lock;

static struct tile_crypto_alg tile_algs[] = {
	{
		.alg = {
			.cra_name	= "authenc(hmac(sha1),cbc(des))",
			.cra_blocksize	= DES_BLOCK_SIZE,
			.cra_u		= { .aead = {
					.ivsize		= DES_BLOCK_SIZE,
					.maxauthsize	= SHA1_DIGEST_SIZE,
				}
			}
		},
		.cipher = GXCR_CIPHER_DES_CBC,
		.digest = GXCR_DIGEST_SHA1,
	}, {
		.alg = {
			.cra_name	= "authenc(hmac(md5),cbc(aes))",
			.cra_blocksize	= AES_BLOCK_SIZE,
			.cra_u		= { .aead = {
					.ivsize		= AES_BLOCK_SIZE,
					.maxauthsize	= MD5_DIGEST_SIZE,
				}
			}
		},
		.cipher = GXCR_CIPHER_AES_CBC_128,
		.digest = GXCR_DIGEST_MD5,
	}, {
		.alg	= {
			.cra_name	= "authenc(hmac(sha1),cbc(aes))",
			.cra_blocksize	= AES_BLOCK_SIZE,
			.cra_u		= { .aead = {
					.ivsize		= AES_BLOCK_SIZE,
					.maxauthsize	= SHA1_DIGEST_SIZE,
				}
			}
		},
		.cipher = GXCR_CIPHER_AES_CBC_128,
		.digest = GXCR_DIGEST_SHA1,
	} ,
{
		.alg	= {
			.cra_name	= "authenc(hmac(sha256),cbc(aes))",
			.cra_blocksize	= AES_BLOCK_SIZE,
			.cra_u		= { .aead = {
					.ivsize		= AES_BLOCK_SIZE,
					.maxauthsize	= SHA256_DIGEST_SIZE,
				}
			}
		},
		.cipher = GXCR_CIPHER_AES_CBC_128,
		.digest = GXCR_DIGEST_SHA_256,
}
};

static struct tile_hw_ctx *alloc_mica_context(int shim)
{
	pte_t hash_pte = pte_set_home((pte_t) { 0 }, PAGE_HOME_HASH);
	struct tile_hw_ctx *pcb = kmalloc(sizeof(struct tile_hw_ctx),
					  GFP_KERNEL);
	int res;
	if (!pcb)
		return 0;

	res = gxio_mica_init(&pcb->mica_ctx, GXIO_MICA_ACCEL_CRYPTO, shim);
	if (res) {
		kfree(pcb);
		return 0;
	}

	res = __gxio_mica_register_client_memory(pcb->mica_ctx.fd, 0,
						 hash_pte, 0);
	if (res) {
		gxio_mica_destroy(&pcb->mica_ctx);
		kfree(pcb);
		return 0;
	}


	return pcb;
}

static void release_mica_context_pool(int shim)
{
	int i;
	unsigned long flags;

        if (!mica_ctx_pool) return;

	spin_lock_irqsave(&ctx_pool_lock, flags);

        for (i = 0; i < tile_num_hw_contexts; i++) {
            if (mica_ctx_pool[i].ctx) {
		gxio_mica_destroy(&mica_ctx_pool[i].ctx->mica_ctx);
		kfree(mica_ctx_pool[i].ctx);
                if (mica_ctx_pool[i].metadata_mem) kfree(mica_ctx_pool[i].metadata_mem);
                tasklet_kill(&mica_ctx_pool[i].bh_task);
            }
        }
        kfree(mica_ctx_pool);
        mica_ctx_pool = NULL;

        spin_unlock_irqrestore(&ctx_pool_lock, flags);
}

/* FIXME: still need to decide how to make use of both shims.  Maybe
 * configuration time param to decide which one shim, or both shims, and
 * if both shims then put them into pool interleaved, figuring they'd get
 * about equal use?  One pool per shim.
 */

static irqreturn_t tile_interrupt_handler(int irq, void *data);
static int init_mica_context_pool(int irq)
{
	int i;
	int result = 0;
        const char shims = (num_possible_cpus() >= 36) ? 1 : 0;

        if (!tile_num_hw_contexts) {
            tile_num_hw_contexts = num_possible_cpus() + 4;
            // XXX - too many global contexts degrade performance
            //if (shims) tile_num_hw_contexts *= 2;
        }
        printk(KERN_ERR "crypto driver contexts: %d\n", tile_num_hw_contexts);

	mica_ctx_pool = kmalloc(sizeof(*mica_ctx_pool) * tile_num_hw_contexts, GFP_KERNEL);
	if (!mica_ctx_pool) {
		return -ENOMEM;
	}
        i = request_irq(irq, &tile_interrupt_handler, IRQF_SHARED, "crypto", mica_ctx_pool);
	if (i) {
            printk(KERN_WARNING "crypto driver request_irq failed \n");
            kfree(mica_ctx_pool);
            mica_ctx_pool = NULL;
            return i;
        }

	for (i = 0; i < tile_num_hw_contexts; i++) {
                int shim = shims ? (i % 2) : 0;
                struct ctx_inf *ctx = &mica_ctx_pool[i];
                memset(ctx, 0, sizeof(*mica_ctx_pool));
		ctx->ctx = alloc_mica_context(shim);
		if (ctx->ctx == 0) {
			printk(KERN_WARNING "crypto driver failed allocating "
			       "context %d on shim %d, requested %d\n",
			       i, shim, tile_num_hw_contexts);
			tile_num_hw_contexts = i;

			if (i == 0) {
				result = -EBUSY;
				kfree(mica_ctx_pool);
				mica_ctx_pool = NULL;
				break;
			}
		}
                tasklet_init(&ctx->bh_task, tile_interrupt_handler_bh, (unsigned long) ctx);
#if TGCRYPTO_SYNC == 0
                result = __gxio_mica_cfg_completion_interrupt(ctx->ctx->mica_ctx.fd,
                                                            cpu_x(i), cpu_y(i),
                                                            KERNEL_PL, irq);
                if (result < 0) {
                    printk(KERN_ERR "__gxio_mica_cfg_completion_interrupt failed: %x\n", result);
                    kfree(mica_ctx_pool);
                    mica_ctx_pool = NULL;
                    break;
                }
#endif
	}

	return result;
}

static struct ctx_inf *get_context(int sync)
{
	int i;
	struct ctx_inf *ctx = &mica_ctx_pool[smp_processor_id()];
	unsigned long flags;

	spin_lock_irqsave(&ctx_pool_lock, flags);

        if (!ctx->used) {
            ctx->used = 1;
            spin_unlock_irqrestore(&ctx_pool_lock, flags);
            return ctx;
        }

        // XXX - reserve contexts for setkey
	for (i = 0; i < (tile_num_hw_contexts - (sync ? 0 : 4)); i++) {
            if (mica_ctx_pool[i].used) continue;
            ctx = &mica_ctx_pool[i];
            ctx->used = 1;
            spin_unlock_irqrestore(&ctx_pool_lock, flags);
            return ctx;
        }
	spin_unlock_irqrestore(&ctx_pool_lock, flags);

	return NULL;
}

static void release_context(struct ctx_inf *ctx)
{
        ctx->req = NULL;
        ctx->iv = NULL;
        smp_wmb();
        ctx->used = 0;
}

static int aead_setkey(struct crypto_aead *tfm, const u8 *key,
			unsigned int keylen)
{
	gxcr_aead_context_t *ctx = crypto_aead_ctx(tfm);
	unsigned int authkeylen;
	unsigned int enckeylen;
	struct crypto_authenc_key_param *param;
	struct rtattr *rta = (void *)key;
	struct ctx_inf *ci;
	int res;

	if (!RTA_OK(rta, keylen))
		goto badkey;

	if (rta->rta_type != CRYPTO_AUTHENC_KEYA_PARAM)
		goto badkey;

	if (RTA_PAYLOAD(rta) < sizeof(*param))
		goto badkey;

	param = RTA_DATA(rta);
	enckeylen = be32_to_cpu(param->enckeylen);

	key += RTA_ALIGN(rta->rta_len);
	keylen -= RTA_ALIGN(rta->rta_len);

	if (keylen < enckeylen)
		goto badkey;

	authkeylen = keylen - enckeylen;

	if (authkeylen <= 0) {
		printk(KERN_WARNING "TILE-Gx non-hmac digests not yet "
		       "supported\n");
		goto badkey;
	}

        if (!ctx->metadata_mem) {
            struct crypto_tfm *tfmx = crypto_aead_tfm(tfm);
            struct tile_crypto_alg *cra = container_of(tfmx->__crt_alg,
                                                       struct tile_crypto_alg,
                                                       alg);
            gxcr_aead_params_t params = {
                    .token_template = &aead_token_info,
                    .cipher = cra->cipher,
                    .digest = cra->digest,
                    .hmac_mode = 1,
            };
            int metadata_mem_size;
            unsigned char *metadata_mem;
            if (cra->cipher == GXCR_CIPHER_AES_CBC_128) {
                if (enckeylen == 24) params.cipher = GXCR_CIPHER_AES_CBC_192;
                else if (enckeylen == 32) params.cipher = GXCR_CIPHER_AES_CBC_256;
            }
            metadata_mem_size = gxcr_aead_calc_context_bytes(&params);
            metadata_mem = kmalloc(metadata_mem_size, GFP_KERNEL);
            if (!metadata_mem) return -ENOMEM;

            res = gxcr_aead_init_context(ctx, metadata_mem,
                                            metadata_mem_size,
                                            &params, NULL, NULL);
            if (res) {
                    kfree(metadata_mem);
                    return res;
            }

            ctx->metadata_mem_size = metadata_mem_size;
            aead_setup(ctx);
        }

	memcpy(gxcr_aead_context_key(ctx), key + authkeylen, enckeylen);

	ci = get_context(1);
	if (!ci) return -EAGAIN;

	res = gxcr_aead_precalc(&ci->ctx->mica_ctx, ctx,
				ci->ctx->packet_buffer,
				sizeof(ci->ctx->packet_buffer),
				(unsigned char *)key, authkeylen);

	release_context(ci);

	if (res)
		goto badkey;

	return 0;

badkey:
	crypto_aead_set_flags(tfm, CRYPTO_TFM_RES_BAD_KEY_LEN);
	return -EINVAL;
}

static int aead_perform(struct aead_request *req, int encrypt,
                        int geniv, unsigned char *iv)
{
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	gxcr_aead_context_t *ctx = crypto_aead_ctx(tfm);
	unsigned int ivsize = crypto_aead_ivsize(tfm);
	unsigned int authsize = crypto_aead_authsize(tfm);
	unsigned char *src_data;
	int result = 0;
	struct ctx_inf *ctxi = get_context(0);
	struct tile_hw_ctx *pcb = NULL;

	if (!ctxi) return -EAGAIN;

        if (ctxi->metadata_mem_size < ctx->metadata_mem_size) {
            ctxi->metadata_mem_size = ctx->metadata_mem_size;
            if (ctxi->metadata_mem) kfree(ctxi->metadata_mem);
            ctxi->metadata_mem = kmalloc(ctx->metadata_mem_size, GFP_KERNEL);
            if (!ctxi->metadata_mem) {
                ctxi->metadata_mem_size = 0;
                ctxi->used = 0;
                return -ENOMEM;
            }
        }

        ctxi->req = req;
        ctxi->encrypt = encrypt;
        ctxi->iv = (geniv && iv) ? iv : NULL;

        pcb = ctxi->ctx;
	src_data = pcb->packet_buffer;
	scatterwalk_map_and_copy(src_data, req->assoc, 0, req->assoclen, 0);
	if (!geniv) {
		memcpy(src_data + req->assoclen, req->iv, ivsize);
		memcpy(gxcr_aead_context_iv(ctx), req->iv, ivsize);
		scatterwalk_map_and_copy(src_data + req->assoclen + ivsize,
					 req->src, 0, req->cryptlen, 0);
	} else
		scatterwalk_map_and_copy(src_data + req->assoclen, req->src,
					 0, req->cryptlen, 0);

	memcpy(ctxi->metadata_mem, ctx->metadata_mem, ctx->metadata_mem_size);

#if TGCRYPTO_SYNC == 0
	result = aead_process_packet_start(&pcb->mica_ctx, ctx, src_data,
			    req->cryptlen + req->assoclen + ivsize,
			    req->assoclen + ivsize, src_data,
			    0 /* unused */, authsize, encrypt, geniv, ivsize,
                            ctxi->metadata_mem);
        return result ? result : -EINPROGRESS;
#else
	result = aead_process_packet(&pcb->mica_ctx, ctx, src_data,
			    req->cryptlen + req->assoclen + ivsize,
			    req->assoclen + ivsize, src_data,
			    0 /* unused */, authsize, encrypt, geniv, ivsize,
                            ctxi->metadata_mem);

        if (!gxcr_aead_result(ctx)->e0_e14) {
            unsigned int copy_len = ctxi->encrypt ?
                req->cryptlen + authsize : req->cryptlen;
            if (ctxi->iv) memcpy(ctxi->iv, src_data + req->assoclen, ivsize);
            scatterwalk_map_and_copy(src_data + req->assoclen + ivsize, req->dst, 0, copy_len, 1);
        } else result = -EBADMSG;

        release_context(ctxi);
        return result;
#endif
}

static int aead_encrypt(struct aead_request *req)
{
	return aead_perform(req, 1, 0, NULL);
}

static int aead_decrypt(struct aead_request *req)
{
	return aead_perform(req, 0, 0, NULL);
}

static int aead_givencrypt(struct aead_givcrypt_request *req)
{
	return aead_perform(&req->areq, 1, 1, req->giv);
}

static void aead_done(struct ctx_inf *ctxi) {
    int result = 0;
    struct aead_request *req = ctxi->req;
    struct crypto_aead *tfm = crypto_aead_reqtfm(req);
    gxcr_aead_context_t *ctx = crypto_aead_ctx(tfm);
    unsigned int ivsize = crypto_aead_ivsize(tfm);
    unsigned int authsize = crypto_aead_authsize(tfm);
    unsigned int copy_len = ctxi->encrypt ?
        req->cryptlen + authsize : req->cryptlen;
    unsigned char *src_data = ctxi->ctx->packet_buffer;
    gxcr_result_token_t *res = gxcr_aead_result(ctx);

    if (!res->e0_e14) {
        if (ctxi->iv) memcpy(ctxi->iv, src_data + req->assoclen, ivsize);
        scatterwalk_map_and_copy(src_data + req->assoclen + ivsize, req->dst, 0, copy_len, 1);
    } else result = -EBADMSG;

    release_context(ctxi);
    aead_request_complete(req, result);
}

static irqreturn_t tile_interrupt_handler(int irq, void *data) {
    struct ctx_inf *ctx = &((struct ctx_inf *) data)[smp_processor_id()];

    if (!ctx->used || !ctx->req) return IRQ_HANDLED; // precalc uses MICA in sync mode
    tasklet_schedule(&ctx->bh_task);

    return IRQ_HANDLED;
}

static void tile_interrupt_handler_bh(unsigned long data) {
    struct ctx_inf *ctx = (struct ctx_inf *) data;
    BUG_ON (!ctx->used);
    aead_done(ctx);
}

#if TGCRYPTO_DEBUG_TIMER
static void tile_timer(unsigned long data) {
    int i;
    for (i = 0; i < tile_num_hw_contexts; i++) {
        struct ctx_inf *ctx = &mica_ctx_pool[i];
        printk(KERN_ERR "busy %x - %x\n", i, ctx->used);
    }
    mod_timer(&timer, jiffies + 300);
}
#endif

static void exit_tfm(struct crypto_tfm *tfm)
{
	gxcr_aead_context_t *ctx = crypto_tfm_ctx(tfm);
        if (ctx->metadata_mem) {
            kfree(ctx->metadata_mem);
            ctx->metadata_mem = NULL;
            ctx->metadata_mem_size = 0;
        }
}

static int tile_remove(struct platform_device *ofdev)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(tile_algs); i++)
		crypto_unregister_alg(&tile_algs[i].alg);
        release_mica_context_pool(0);
	return 0;
}

static int tile_probe(struct platform_device *ofdev)
{
	int i;
	int err;

	err = init_mica_context_pool(irq);
	if (err) return err;

	for (i = 0; i < ARRAY_SIZE(tile_algs); i++) {
		struct crypto_alg *cra = &tile_algs[i].alg;
		cra->cra_type = &crypto_aead_type;
		cra->cra_flags = CRYPTO_ALG_TYPE_AEAD |
			CRYPTO_ALG_ASYNC;
		cra->cra_aead.setkey = aead_setkey;
		cra->cra_aead.encrypt = aead_encrypt;
		cra->cra_aead.decrypt = aead_decrypt;
		cra->cra_aead.givencrypt = aead_givencrypt;
		cra->cra_ctxsize = sizeof(gxcr_aead_context_t);
		cra->cra_module = THIS_MODULE;
		cra->cra_alignmask = 0;
		cra->cra_priority = 300;
		cra->cra_exit = exit_tfm;
		err = crypto_register_alg(cra);
		if (err) {
			printk(KERN_ERR "Failed to register TILE-Gx '%s'\n",
			       cra->cra_name);
			goto err_out;
		}
	}

#if TGCRYPTO_DEBUG_TIMER
	mod_timer(&timer, jiffies + 100);
#endif
	return 0;

err_out:
	tile_remove(ofdev);

	return err;
}

static struct platform_driver tile_driver = {
	.driver = {
		.name = "tile",
		.owner = THIS_MODULE,
	},
	.probe = tile_probe,
	.remove = tile_remove,
};

static int __init tile_module_init(void)
{
        irq = create_irq();
        if (irq < 0) {
            printk(KERN_WARNING "crypto driver create_irq failed \n");
            return -1;
        }
        tile_irq_activate(irq, TILE_IRQ_PERCPU);

#if TGCRYPTO_DEBUG_TIMER
        timer.function = tile_timer;
        timer.data = (int) 0;
        init_timer(&timer);
#endif
	spin_lock_init(&ctx_pool_lock);

	return platform_driver_register(&tile_driver);
}

static void __exit tile_module_exit(void)
{

#if TGCRYPTO_DEBUG_TIMER
        del_timer_sync(&timer);
#endif
	if (mica_ctx_pool) free_irq(irq, mica_ctx_pool);
	destroy_irq(irq);
	platform_driver_unregister(&tile_driver);
}

module_init(tile_module_init);
module_exit(tile_module_exit);
