#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/console.h>
#include <asm/bootinfo.h>
#include <asm/rb/boards.h>
#include <linux/bootmem.h>
#include <linux/ioport.h>
#include <linux/ctype.h>
#include <linux/irq.h>
#include <linux/initrd.h>

#define FREQ_TAG   "HZ="
#define BOARD_TAG  "board="

#define SR_NMI	0x00180000

extern void rb500_setup(void);
extern void rb100_setup(void);
extern void rb400_setup(void);
extern void cr_setup(void);
extern void rbvm_setup(void);

extern char arcs_cmdline[COMMAND_LINE_SIZE];

extern unsigned long totalram_pages;
extern unsigned long mips_hpt_frequency;

unsigned char mips_mac_address[6];

unsigned long mips_machgroup __read_mostly = 0;
EXPORT_SYMBOL(mips_machgroup);

const char *get_system_type(void)
{
	switch (mips_machgroup) {
	case MACH_GROUP_MT_RB100:
		switch (mips_machtype) {
		case MACH_MT_RB100:
			return "Mikrotik RB100";
		case MACH_MT_RB150:
			return "Mikrotik RB150";
		case MACH_MT_RB133:
			return "Mikrotik RB133";
		case MACH_MT_RB133C:
			return "Mikrotik RB133C";
		case MACH_MT_RB192:
			return "Mikrotik RB192";
		case MACH_MT_MR:
			return "Mikrotik MR";
		}
		break;
	case MACH_GROUP_MT_RB500:
		switch (mips_machtype) {
		case MACH_MT_RB500:
			return "Mikrotik RB500";
		case MACH_MT_RB500R5:
			return "Mikrotik RB500r5";
		}
		break;
	case MACH_GROUP_MT_RB400:
		switch (mips_machtype) {
		case MACH_MT_RB411:
			return "Mikrotik RB411";
		case MACH_MT_RB411U:
			return "Mikrotik RB411U";
		case MACH_MT_RB433:
			return "Mikrotik RB433";
		case MACH_MT_RB433U:
			return "Mikrotik RB433U";
		case MACH_MT_RB450:
			return "Mikrotik RB450";
		case MACH_MT_RB450G:
			return "Mikrotik RB450G";
		case MACH_MT_RB493:
			return "Mikrotik RB493";
		case MACH_MT_RB750G:
			return "Mikrotik RB750G";
		}
		break;
	case MACH_GROUP_MT_RB700:
		switch (mips_machtype) {
		case MACH_MT_RB711R3:
		case MACH_MT_RB711:
			return "Mikrotik RB711";
		case MACH_MT_RB_SXT5D:
			return "Mikrotik SXT-5D";
		case MACH_MT_RB_GROOVE:
			return "Mikrotik Groove-5H";
		case MACH_MT_RB750:
			return "Mikrotik RB750";
		}
		break;
	case MACH_GROUP_MT_CR:
		switch (mips_machtype) {
		case MACH_MT_CR1:
			return "Mikrotik CR";
		}
	case MACH_GROUP_MT_VM:
		return "Mikrotik VM";
	}
	return "unknown routerboard";
}

void __init prom_init(void)
{
	int argc = fw_arg0;
	char **argv = (char **) fw_arg1;
	unsigned char board_type[16];

	unsigned i, offset = 0;

	set_io_port_base(KSEG1);

	memset(board_type, 0, sizeof(board_type));

	/* HZ must be parsed here because otherwise is too late */
	for (i = 0; (i < argc && argv[i] != NULL); i++) {
		if (strncmp(argv[i], FREQ_TAG, sizeof(FREQ_TAG) - 1) == 0) {
			mips_hpt_frequency = 
			    simple_strtoul(argv[i] + sizeof(FREQ_TAG) - 1, 0, 10);
			continue;
		}
		if (strncmp(argv[i], BOARD_TAG, sizeof(BOARD_TAG) - 1) == 0) {
			strncpy(board_type, argv[i] + sizeof(BOARD_TAG) - 1,
				sizeof(board_type));
		}
		offset += snprintf(arcs_cmdline + offset, sizeof(arcs_cmdline) - offset,
				   "%s ", argv[i]);
	}

	if (strncmp(board_type, "500r5", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB500;
		mips_machtype = MACH_MT_RB500R5;
	} else if (strncmp(board_type, "411", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB400;
		mips_machtype = MACH_MT_RB411;
	} else if (strncmp(board_type, "750G", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB400;
		mips_machtype = MACH_MT_RB750G;
	} else if (strncmp(board_type, "750i", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB700;
		mips_machtype = MACH_MT_RB750;
	} else if (strncmp(board_type, "751", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB700;
		mips_machtype = MACH_MT_RB750;
	} else if (strncmp(board_type, "711r3", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB700;
		mips_machtype = MACH_MT_RB711R3;
	} else if (strncmp(board_type, "711", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB700;
		mips_machtype = MACH_MT_RB711;
	} else if (strncmp(board_type, "groove", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB700;
		mips_machtype = MACH_MT_RB_SXT5D;
	} else if (strncmp(board_type, "groove-5", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB700;
		mips_machtype = MACH_MT_RB_GROOVE;
	} else if (strncmp(board_type, "411U", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB400;
		mips_machtype = MACH_MT_RB411U;
	} else if (strncmp(board_type, "433", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB400;
		mips_machtype = MACH_MT_RB433;
	} else if (strncmp(board_type, "433U", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB400;
		mips_machtype = MACH_MT_RB433U;
	} else if (strncmp(board_type, "450", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB400;
		mips_machtype = MACH_MT_RB450;
	} else if (strncmp(board_type, "450G", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB400;
		mips_machtype = MACH_MT_RB450G;
	} else if (strncmp(board_type, "493G", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB400;
		mips_machtype = MACH_MT_RB493G;
	} else if (strncmp(board_type, "493", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB400;
		mips_machtype = MACH_MT_RB493;
	} else if (strncmp(board_type, "100", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB100;
		mips_machtype = MACH_MT_RB100;
	} else if (strncmp(board_type, "150", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB100;
		mips_machtype = MACH_MT_RB150;
	} else if (strncmp(board_type, "133", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB100;
		mips_machtype = MACH_MT_RB133;
	} else if (strncmp(board_type, "133C", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB100;
		mips_machtype = MACH_MT_RB133C;
	} else if (strncmp(board_type, "192", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB100;
		mips_machtype = MACH_MT_RB192;
	} else if (strncmp(board_type, "mr", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_RB100;
		mips_machtype = MACH_MT_MR;
	} else if (strncmp(board_type, "cr", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_CR;
		mips_machtype = MACH_MT_CR1;
	} else if (strncmp(board_type, "vm", sizeof(board_type)) == 0) {
		mips_machgroup = MACH_GROUP_MT_VM;
		mips_machtype = 0;
	} else {
		mips_machgroup = MACH_GROUP_MT_RB500;
		mips_machtype = MACH_MT_RB500;
	}
}

void __init prom_free_prom_memory(void)
{
	unsigned long addr, end;
	extern char _text;

	/*
	 * Free everything below the kernel itself but leave
	 * the first page reserved for the exception handlers.
	 */

	end = __pa(&_text);
	addr = PAGE_SIZE;

	while (addr < end) {
		ClearPageReserved(virt_to_page(__va(addr)));
		init_page_count(virt_to_page(__va(addr)));
		free_page((unsigned long)__va(addr));
		addr += PAGE_SIZE;
		++totalram_pages;
	}
}

void __init plat_mem_setup(void)
{
#ifdef CONFIG_BLK_DEV_INITRD
	extern int _end;
	u32 *initrd_header;

	initrd_header = __va(PAGE_ALIGN(__pa_symbol(&_end) + 8)) - 8;
	if (initrd_header[0] == 0x494E5244) {
		initrd_start = (unsigned long) (initrd_header + 2);
                initrd_end = initrd_start + initrd_header[1];
	}
#endif    

	switch (mips_machgroup) {
	case MACH_GROUP_MT_RB500:
		rb500_setup();
		break;
	case MACH_GROUP_MT_RB100:
		rb100_setup();
		break;
	case MACH_GROUP_MT_RB400:
	case MACH_GROUP_MT_RB700:
		rb400_setup();
		break;
	case MACH_GROUP_MT_CR:
		cr_setup();
		break;
	case MACH_GROUP_MT_VM:
		rbvm_setup();
		break;
	}
}

void __init plat_time_init(void)
{
}

static int __init setup_kmac(char *s)
{
	int i, j;
	unsigned char result, value;

	for (i = 0; i < 6; i++) {
		if (s[0] == '\0' || s[1] == '\0') return 0;
		if (i != 5 && s[2] != ':') return 0;

		result = 0;
		for (j = 0; j < 2; j++) {
			if (!isxdigit(*s)) return 0;

			value = isdigit(*s) ? *s - '0' :
				toupper(*s) - 'A' + 10;
			if (value >= 16) return 0;

			result = result * 16 + value;
                        s++;
                }

                s++;
                mips_mac_address[i] = result;
        }

        return *s == '\0';
}

__setup("kmac=", setup_kmac);

EXPORT_SYMBOL(mips_mac_address);

unsigned long long sched_clock(void)
{
	return read_c0_count() * 1000000000 / mips_hpt_frequency;
}
