#include <linux/kernel.h>
#include <linux/module.h>	/* for EXPORT_SYMBOL() */
#include <linux/string.h>	/* for memcpy() */
#include <linux/hardirq.h>	/* for in_atomic() */
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <asm/bootinfo.h>
#include <asm/rb/boards.h>
#include <asm/rb/rb400.h>

#define REG_GPIO_SET	KSEG1ADDR(0x1804000C)
#define REG_GPIO_CLR	KSEG1ADDR(0x18040010)

static struct spi_device *spi = 0;

struct spi_device *rb400_spi_get(void) {
	return spi;
}
EXPORT_SYMBOL(rb400_spi_get);

static void rb400_spi_async_complete(void *msg_buf) {
	kfree(msg_buf);
}

static void rb400_spi_write_async(const uint8_t *buf, unsigned len) {
	struct msg_async {
		struct spi_message m;
		struct spi_transfer t;
		uint8_t data[];
	};

	struct msg_async *ma;
		
	ma = kzalloc(sizeof(*ma) + len, GFP_KERNEL);
	if (ma == NULL) {
		printk(KERN_ERR "rb400_spi_write_async failed: OOM\n");
		return;
	}
	spi_message_init(&ma->m);
	spi_message_add_tail(&ma->t, &ma->m);

	ma->t.tx_buf = ma->data;
	ma->t.len = len;
	memcpy(ma->data, buf, len);

	ma->m.context = ma;
	ma->m.complete = rb400_spi_async_complete;

	spi_async(spi, &ma->m);
}

void rb400_change_cfg(unsigned off, unsigned on) {
	static unsigned cfg = (CFG_BIT_nLED1 | CFG_BIT_nLED2 |
			       CFG_BIT_nLED3 | CFG_BIT_nLED4 |
			       CFG_BIT_nLED5);
	unsigned ocfg;
	unsigned ncfg;

	static DEFINE_SPINLOCK(lock);
	unsigned long flags;

	spin_lock_irqsave(&lock, flags);
	ocfg = cfg;
	cfg = (cfg & ~off) | on;
	ncfg = cfg;
	spin_unlock_irqrestore(&lock, flags);
	/*
	 * FIXME: it is possible to send CFG changes in wrong order
	 *	  in case if task switch happens here:
	 *		1) LED change gets interrupted here
	 *		2) NAND_ALE change is finished, it gets interrupted
	 *		3) LED change finishes, ALE change is lost at moment
	 *		4) NAND write is done with missing ALE!!!
	 */

	if ((ncfg ^ ocfg) & 0xff) {
		unsigned char buf[2] = { SPI_CMD_WRITE_CFG, ncfg };

		if (in_atomic())
			rb400_spi_write_async(buf, 2);
		else
			spi_write(spi, buf, 2);
	}
	if ((ncfg ^ ocfg) & CFG_BIT_nLED5) {
		uint8_t cmd = ((ncfg & CFG_BIT_nLED5) ?
			       SPI_CMD_LED5_OFF :
			       SPI_CMD_LED5_ON);
		rb400_spi_write_async(&cmd, 1);
	}
}
EXPORT_SYMBOL(rb400_change_cfg);

int rb400_spiflash_read_verify(unsigned addr,
			       uint8_t *rdata, const uint8_t *vdata,
			       unsigned cnt) {
	const uint8_t cmd[5] = { SPI_CMD_READ_FAST,
				 (addr >> 16) & 0xff,
				 (addr >> 8) & 0xff,
				 addr & 0xff,
				 0
	};
	struct spi_transfer t[2] = {
		{
			.tx_buf = &cmd,
			.len = 5,
		},
		{
			.tx_buf = vdata,
			.rx_buf = rdata,
			.len = cnt,
			.verify = (vdata != NULL),
		},
	};
	struct spi_message m;

	spi_message_init(&m);
	m.fast_read = 1;
	spi_message_add_tail(&t[0], &m);
	spi_message_add_tail(&t[1], &m);
	return spi_sync(spi, &m);
}
EXPORT_SYMBOL(rb400_spiflash_read_verify);

int rb400_spi_sync(struct spi_message *message) {
	return spi_sync(spi, message);
}
EXPORT_SYMBOL(rb400_spi_sync);

static int rb400_spi_misc_probe(struct spi_device *_spi)
{
	printk("RB400 spi misc\n");
	spi = _spi;
	return 0;
}

static struct spi_driver rb400_spi_misc = {
	.driver	= {
		.name = "rb400-spi-misc",
		.owner = THIS_MODULE,
	},
	.probe	= rb400_spi_misc_probe,
};

int __init rb400_spiboard_init(void) {
	return spi_register_driver(&rb400_spi_misc);
}

module_init(rb400_spiboard_init);
