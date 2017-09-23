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

/*
 *
 * Implementation of mica gxio calls.
 */

#include <gxio/iorpc_globals.h>
#include <gxio/iorpc_mica.h>
#include <gxio/kiorpc.h>
#include <gxio/mica.h>

int gxio_mica_init(gxio_mica_context_t * context,
		   gxio_mica_accelerator_type_t type, int mica_index)
{
	char file[32];

	if (type == GXIO_MICA_ACCEL_CRYPTO)
		snprintf(file, sizeof(file), "crypto/%d/iorpc", mica_index);
	else if (type == GXIO_MICA_ACCEL_COMP)
		snprintf(file, sizeof(file), "comp/%d/iorpc", mica_index);
	else
		return GXIO_MICA_ERR_BAD_ACCEL_TYPE;

	context->fd = hv_dev_open((HV_VirtAddr) file, 0);

	if (context->fd < 0) {
		return -ENODEV;
	}
	// Map in the Context User MMIO space, for just this one context.
	context->mmio_context_user_base = (void __force *)
		iorpc_ioremap(context->fd, 0, HV_MICA_CONTEXT_USER_MMIO_SIZE);
	if (context->mmio_context_user_base == NULL) {
		hv_dev_close(context->fd);
		return -ENODEV;
	}

	return 0;
}

EXPORT_SYMBOL_GPL(gxio_mica_init);

int gxio_mica_destroy(gxio_mica_context_t * context)
{

	iounmap((void __force __iomem *)(context->mmio_context_user_base));
	return hv_dev_close(context->fd);

}

EXPORT_SYMBOL_GPL(gxio_mica_destroy);

int gxio_mica_register_page(gxio_mica_context_t * context,
			    void *page, size_t page_size,
			    unsigned int page_flags)
{
	unsigned long vpn = (unsigned long)page >> 12;

	return __gxio_mica_register_page_aux(context->fd, page, page_size,
					     page_flags, vpn);

}

EXPORT_SYMBOL_GPL(gxio_mica_register_page);

void gxio_mica_memcpy_start(gxio_mica_context_t * context, void *dst,
			    void *src, int length)
{
	MICA_OPCODE_t opcode_oplen = { {0} };

	__gxio_mmio_write(context->mmio_context_user_base + MICA_SRC_DATA,
			  (unsigned long)src);
	__gxio_mmio_write(context->mmio_context_user_base + MICA_DEST_DATA,
			  (unsigned long)dst);
	__gxio_mmio_write(context->mmio_context_user_base +
			  MICA_EXTRA_DATA_PTR, 0);

	opcode_oplen.size = length;
	opcode_oplen.engine_type = MICA_OPCODE__ENGINE_TYPE_VAL_MEM_CPY;
	opcode_oplen.src_mode = MICA_OPCODE__SRC_MODE_VAL_SINGLE_BUFF_DESC;
	opcode_oplen.dest_mode = MICA_OPCODE__DEST_MODE_VAL_SINGLE_BUFF_DESC;

	__insn_mf();
	__gxio_mmio_write(context->mmio_context_user_base + MICA_OPCODE,
			  opcode_oplen.word);
}

void gxio_mica_start_op(gxio_mica_context_t * context,
			void *src, void *dst, void *extra_data,
			gxio_mica_opcode_t opcode)
{
	__gxio_mmio_write(context->mmio_context_user_base + MICA_SRC_DATA,
			  (unsigned long)src);
	__gxio_mmio_write(context->mmio_context_user_base + MICA_DEST_DATA,
			  (unsigned long)dst);
	__gxio_mmio_write(context->mmio_context_user_base +
			  MICA_EXTRA_DATA_PTR, (unsigned long)extra_data);

	__insn_mf();

	__gxio_mmio_write(context->mmio_context_user_base + MICA_OPCODE,
			  opcode.word);
}

EXPORT_SYMBOL_GPL(gxio_mica_start_op);

int gxio_mica_is_busy(gxio_mica_context_t * context)
{
	MICA_IN_USE_t inuse;
	inuse.word =
		__gxio_mmio_read(context->mmio_context_user_base +
				 MICA_IN_USE);
	return inuse.in_use;
}

EXPORT_SYMBOL_GPL(gxio_mica_is_busy);
