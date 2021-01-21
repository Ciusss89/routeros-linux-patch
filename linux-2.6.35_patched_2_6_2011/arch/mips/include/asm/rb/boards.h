#ifndef _ASM_RB_BOARDS_H
#define _ASM_RB_BOARDS_H

#define MACH_GROUP_MT_RB500    1	/* Mikrotik RB500 */
#define MACH_GROUP_MT_RB100    2	/* Mikrotik RB100 */
#define MACH_GROUP_MT_CR       3	/* Mikrotik CR */
#define MACH_GROUP_MT_RB400    4
#define MACH_GROUP_MT_VM       6
#define MACH_GROUP_MT_RB700    7


#define  MACH_MT_RB500		0
#define  MACH_MT_RB500R5	1
#define  MACH_MT_RB100		2
#define  MACH_MT_RB150		3
#define  MACH_MT_RB133		4
#define  MACH_MT_RB133C		5
#define  MACH_MT_MR		6
#define  MACH_MT_RB192		7
#define  MACH_MT_CR1		8
#define  MACH_MT_RB411		10
#define  MACH_MT_RB433		11
#define  MACH_MT_RB433U		12	/* RB433 + USB */
#define  MACH_MT_RB450		13
#define  MACH_MT_RB493		15
#define  MACH_MT_RB450G		16
#define  MACH_MT_RB411U		17
#define  MACH_MT_RB493G		18
#define  MACH_MT_RB750G		19
#define  MACH_MT_RB750		23
#define  MACH_MT_RB711		24
#define  MACH_MT_RB_SXT5D	26
#define  MACH_MT_RB_GROOVE	27
#define  MACH_MT_RB711R3	29

extern unsigned long mips_machgroup;

#endif
