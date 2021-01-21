/*
 * Copyright (C) 2007 Google, Inc.
 * Copyright (c) 2008-2010, Code Aurora Forum. All rights reserved.
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
 */

#ifndef __ASM_ARCH_MSM_IRQS_H
#define __ASM_ARCH_MSM_IRQS_H

#define MSM_IRQ_BIT(irq)     (1 << ((irq) & 31))

#if defined(CONFIG_ARCH_MSM7X30)
#include "irqs-7x30.h"
#elif defined(CONFIG_ARCH_QSD8X50)
#include "irqs-8x50.h"
#include "sirc.h"
#elif defined(CONFIG_ARCH_MSM8X60)
#include "irqs-8x60.h"
#elif defined(CONFIG_ARCH_IPQ806X)

#define NR_MSM_IRQS 288
#define NR_GPIO_IRQS 152
#define NR_BOARD_IRQS 0
#define NR_PCIE_MSI_IRQS 256

#define GIC_PPI_START 16
#define INT_DEBUG_TIMER_EXP	(GIC_PPI_START + 1)
#define INT_GP_TIMER_EXP	(GIC_PPI_START + 2)
#define GIC_SPI_START 32

#define RPM_APCC_CPU0_GP_HIGH_IRQ	(GIC_SPI_START + 19)
#define RPM_APCC_CPU0_GP_MEDIUM_IRQ	(GIC_SPI_START + 20)
#define RPM_APCC_CPU0_GP_LOW_IRQ	(GIC_SPI_START + 21)
#define RPM_APCC_CPU0_WAKE_UP_IRQ	(GIC_SPI_START + 22)

#define RPM_APCC_CPU1_GP_HIGH_IRQ	(GIC_SPI_START + 23)
#define RPM_APCC_CPU1_GP_MEDIUM_IRQ	(GIC_SPI_START + 24)
#define RPM_APCC_CPU1_GP_LOW_IRQ	(GIC_SPI_START + 25)
#define RPM_APCC_CPU1_WAKE_UP_IRQ	(GIC_SPI_START + 26)

#define PM8821_SEC_IRQ_N	(GIC_SPI_START + 15)
#define TLMM_SCSS_SUMMARY_IRQ	(GIC_SPI_START + 16)
#define TLMM_MSM_SUMMARY_IRQ	(GIC_SPI_START + 16)
#define PCIE20_INT_MSI		(GIC_SPI_START + 35)
#define PCIE20_INTA		(GIC_SPI_START + 36)
#define PCIE20_1_INT_MSI	(GIC_SPI_START + 57)
#define PCIE20_1_INTA		(GIC_SPI_START + 58)
#define PCIE20_2_INT_MSI	(GIC_SPI_START + 71)
#define PCIE20_2_INTA		(GIC_SPI_START + 72)
#define LPASS_DML_IRQ		(GIC_SPI_START + 88)
#define LPASS_SPDIFTX_IRQ	(GIC_SPI_START + 89)
#define LPASS_SCSS_GP_HIGH_IRQ	(GIC_SPI_START + 90)
#define USB30_EE1_IRQ_1		(GIC_SPI_START + 110)
#define SPS_MTI_30		(GIC_SPI_START + 137)
#define SPS_MTI_31		(GIC_SPI_START + 138)
#define GSBI5_QUP_IRQ		(GIC_SPI_START + 155)
#define GSBI2_QUP_IRQ		(GIC_SPI_START + 196)
#define ADM_0_SCSS_0_IRQ	(GIC_SPI_START + 170)
#define USB1_HS_IRQ		(GIC_SPI_START + 100)
#define USB3_HS_IRQ		(GIC_SPI_START + 188)
#define USB30_EE1_IRQ		(GIC_SPI_START + 205)
#define USB4_HS_IRQ		(GIC_SPI_START + 215)


#define AVS_SVICINT		(GIC_PPI_START + 6)
#define AVS_SVICINTSWDONE	(GIC_PPI_START + 7)

#define MSM_PCIE_MSI_INT(n)	(921 + (n))

#elif defined(CONFIG_ARCH_MSM8960)
/* TODO: Make these not generic. */
#include "irqs-8960.h"
#elif defined(CONFIG_ARCH_MSM_ARM11)
#include "irqs-7x00.h"
#else
#error "Unknown architecture specification"
#endif

#define NR_IRQS (NR_MSM_IRQS + NR_GPIO_IRQS + NR_BOARD_IRQS)
#define MSM_GPIO_TO_INT(n) (NR_MSM_IRQS + (n))
#define MSM_INT_TO_REG(base, irq) (base + irq / 32)

#endif
