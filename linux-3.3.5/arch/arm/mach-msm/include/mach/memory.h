#ifndef __ASM_ARCH_MEMORY_H
#define __ASM_ARCH_MEMORY_H
#include <linux/types.h>

/* physical offset of RAM */
#define PLAT_PHYS_OFFSET UL(CONFIG_PHYS_OFFSET)

#define MAX_PHYSMEM_BITS 32
#define SECTION_SIZE_BITS 28

/* Maximum number of Memory Regions
*  The largest system can have 4 memory banks, each divided into 8 regions
*/
#define MAX_NR_REGIONS 32

/* The number of regions each memory bank is divided into */
#define NR_REGIONS_PER_BANK 8

/* these correspond to values known by the modem */
#define MEMORY_DEEP_POWERDOWN	0
#define MEMORY_SELF_REFRESH	1
#define MEMORY_ACTIVE		2

#define NPA_MEMORY_NODE_NAME	"/mem/apps/ddr_dpd"

#define CONSISTENT_DMA_SIZE	(SZ_1M * 14)

#endif
