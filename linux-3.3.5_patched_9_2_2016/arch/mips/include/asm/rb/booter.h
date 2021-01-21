#ifndef _ASM_BOOTER_H
#define _ASM_BOOTER_H

#define ID_HW_OPTIONS		0x00000015

#define HW_OPT_UART_ABSENT	(1 << 0)
#define HW_OPT_HAS_VOLTAGE	(1 << 1)
#define HW_OPT_HAS_USB		(1 << 2)
#define HW_OPT_HAS_ATTINY	(1 << 3)
#define HW_OPT_NO_NAND		(1 << 14)
#define HW_OPT_HAS_LCD		(1 << 15)
#define HW_OPT_HAS_POE_OUT	(1 << 16)
#define HW_OPT_HAS_uSD		(1 << 17)
#define HW_OPT_HAS_SFP		(1 << 20)
#define HW_OPT_HAS_WIFI		(1 << 21)
#define HW_OPT_HAS_TS_FOR_ADC	(1 << 22)

int read_booter_cfg(unsigned id, void *buf, int amount);

#endif
