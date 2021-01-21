/**
 * @file init.c
 *
 * @remark Copyright 2002 OProfile authors
 * @remark Read the file COPYING
 *
 * @author John Levon <levon@movementarian.org>
 */

#include <linux/oprofile.h>
#include <linux/init.h>
#include <linux/errno.h>

/*
 * We support CPUs that have performance counters like the Pentium Pro
 * with the NMI mode driver.
 */

extern int op_nmi_init(struct oprofile_operations *ops);
extern int op_nmi_timer_init(struct oprofile_operations *ops);
extern void op_nmi_exit(void);
extern void x86_backtrace(struct pt_regs * const regs, unsigned int depth);
extern int __init hrtimer_oprofile_init(struct oprofile_operations *);


int __init oprofile_arch_init(struct oprofile_operations *ops)
{
	int ret;

	ret = -ENODEV;

	ops->backtrace = x86_backtrace;

	if (hrtimer_oprofile_init(ops) == 0)
		return 0;

#ifdef CONFIG_X86_LOCAL_APIC
	ret = op_nmi_init(ops);
#endif
#ifdef CONFIG_X86_IO_APIC
	if (ret < 0)
		ret = op_nmi_timer_init(ops);
#endif
	return ret;
}


void oprofile_arch_exit(void)
{
#ifdef CONFIG_X86_LOCAL_APIC
	op_nmi_exit();
#endif
}
