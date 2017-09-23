#include <linux/export.h>
#include <linux/io.h>
#include <asm/smp.h>
#include <asm/hardware/gic.h>
#include <mach/clk-provider.h>
#include <mach/system.h>

int rb_mach;
EXPORT_SYMBOL(rb_mach);

/*
int clk_enable(struct clk *clk) {
	return 0;
}
EXPORT_SYMBOL(clk_enable);

void clk_disable(struct clk *clk) {
}
EXPORT_SYMBOL(clk_disable);

unsigned long clk_get_rate(struct clk *clk) {
	return clk->rate;
}
EXPORT_SYMBOL(clk_get_rate);
*/


static DEFINE_SPINLOCK(boot_lock);

void __cpuinit platform_secondary_init(unsigned cpu) {
	gic_secondary_init(0);
	spin_lock(&boot_lock);
	spin_unlock(&boot_lock);
}

void ipq806x_boot_secondary(unsigned cpu);
int __cpuinit boot_secondary(unsigned cpu, struct task_struct *idle) {
	spin_lock(&boot_lock);

	switch (rb_mach) {
	case RB_MACH_IPQ806X:
		ipq806x_boot_secondary(cpu);
		break;
	};

		gic_raise_softirq(cpumask_of(cpu), 0);
	spin_unlock(&boot_lock);
	return 0;
}

void __init smp_init_cpus(void) {
	unsigned i, ncores = 1;

	switch (rb_mach) {
	case RB_MACH_IPQ806X:
		ncores = 2;
		break;
	};

	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);

		set_smp_cross_call(gic_raise_softirq);
}

void __init qcom_smp_prepare_cpus(void);
void __init platform_smp_prepare_cpus(unsigned int max_cpus) {
	int i;

	for (i = 0; i < max_cpus; i++)
		set_cpu_present(i, true);
}

void ipq806x_teardown_msi_irq(unsigned int irq);
int ipq806x_setup_msi_irq(struct pci_dev *pdev, struct msi_desc *desc);
void arch_teardown_msi_irq(unsigned int irq) {
	switch (rb_mach) {
	case RB_MACH_IPQ806X:
		return ipq806x_teardown_msi_irq(irq);
	};
}
int arch_setup_msi_irq(struct pci_dev *pdev, struct msi_desc *desc) {
	switch (rb_mach) {
	case RB_MACH_IPQ806X:
		return ipq806x_setup_msi_irq(pdev, desc);
	};
	return -1;
}
