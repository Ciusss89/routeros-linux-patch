/*
 * Copyright (C) 2007 Google, Inc.
 * Copyright (c) 2008-2011, Code Aurora Forum. All rights reserved.
 * Author: Brian Swetland <swetland@google.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 * The MSM peripherals are spread all over across 768MB of physical
 * space, which makes just having a simple IO_ADDRESS macro to slide
 * them into the right virtual location rough.  Instead, we will
 * provide a master phys->virt mapping for peripherals here.
 *
 */

#ifndef __ASM_ARCH_MSM_IOMAP_H
#define __ASM_ARCH_MSM_IOMAP_H

#include <asm/sizes.h>

/* Physical base address and size of peripherals.
 * Ordered by the virtual base addresses they will be mapped at.
 *
 * MSM_VIC_BASE must be an value that can be loaded via a "mov"
 * instruction, otherwise entry-macro.S will not compile.
 *
 * If you add or remove entries here, you'll want to edit the
 * msm_io_desc array in arch/arm/mach-msm/io.c to reflect your
 * changes.
 *
 */

#ifdef __ASSEMBLY__
#define IOMEM(x)	x
#else
#define IOMEM(x)	((void __force __iomem *)(x))
#endif

#if defined(CONFIG_ARCH_MSM7X30)
#include "msm_iomap-7x30.h"
#elif defined(CONFIG_ARCH_QSD8X50)
#include "msm_iomap-8x50.h"
#elif defined(CONFIG_ARCH_MSM8X60)
#include "msm_iomap-8x60.h"
#elif defined(CONFIG_ARCH_MSM7X00A)
#include "msm_iomap-7x00.h"
#elif defined(CONFIG_ARCH_MSM8960)
#include "msm_iomap-8960.h"
#elif defined(CONFIG_ARCH_IPQ806X)
#include "msm_iomap-ipq806x.h"
#endif

#define MSM_DEBUG_UART_SIZE	SZ_4K
#if defined(CONFIG_DEBUG_MSM_UART1)
#define MSM_DEBUG_UART_BASE	0xE1000000
#define MSM_DEBUG_UART_PHYS	MSM_UART1_PHYS
#elif defined(CONFIG_DEBUG_MSM_UART2)
#define MSM_DEBUG_UART_BASE	0xE1000000
#define MSM_DEBUG_UART_PHYS	MSM_UART2_PHYS
#elif defined(CONFIG_DEBUG_MSM_UART3)
#define MSM_DEBUG_UART_BASE	0xE1000000
#define MSM_DEBUG_UART_PHYS	MSM_UART3_PHYS
#endif

/* Virtual addresses shared across all MSM targets. */
#define MSM_CSR_BASE		IOMEM(0xFA101000)
#define MSM_QGIC_DIST_BASE	IOMEM(0xFA002000)
#define MSM_QGIC_CPU_BASE	IOMEM(0xFA003000)
#define MSM_TMR_BASE		IOMEM(0xFA000000)
#define MSM_TMR0_BASE		IOMEM(0xFA001000)
#define MSM_GPIO1_BASE		IOMEM(0xFA102000)
#define MSM_GPIO2_BASE		IOMEM(0xFA103000)
#define MSM_CLK_CTL_BASE	IOMEM(0xFA010000)
#define MSM_NSS_TCM_BASE        IOMEM(0xFB700000)
#define MSM_NSS_FPB_BASE        IOMEM(0xFB720000)
#define MSM_TLMM_BASE		IOMEM(0xFA017000)
#define MSM_UBI32_0_CSM_BASE    IOMEM(0xFB721000)
#define MSM_UBI32_1_CSM_BASE    IOMEM(0xFB722000)
#define MSM_QFPROM_BASE		IOMEM(0xFA700000)
#define MSM_SAW0_BASE		IOMEM(0xFA008000)
#define MSM_SAW1_BASE           IOMEM(0xFA009000)
#define MSM_SAW2_BASE           IOMEM(0xFA703000)
#define MSM_SAW3_BASE           IOMEM(0xFA704000)
#define MSM_SAW_L2_BASE		IOMEM(0xFA007000)
#define MSM_RPM_BASE		IOMEM(0xFA801000)
#define MSM_APCS_GCC_BASE	IOMEM(0xFA006000)
#define MSM_SHARED_RAM_BASE	IOMEM(0xFA300000)
#define MSM_LPASS_CLK_CTL_BASE	IOMEM(0xFA015000)

#define MSM_SHARED_RAM_SIZE	SZ_2M
#endif
