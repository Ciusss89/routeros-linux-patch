#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/bootinfo.h>
#include <asm/rb/boards.h>
#include <asm/rb/rb400.h>
#include <asm/irq_cpu.h>

#define REG_INT(x)		((unsigned) int_base + (x))
#define REG_INT_STATUS		REG_INT(0x10)
#define REG_INT_MASK		REG_INT(0x14)
#define REG_PCI_INT_STATUS	REG_INT(0x18)
#define REG_PCI_INT_MASK	REG_INT(0x1c)

#define REG_DDR_PCI_FLUSH	((unsigned) flush_base + 0xa8)

static void __iomem *int_base;
static void __iomem *flush_base;

static void rb400_ack_irq(unsigned int irq)
{
	rb400_writel(rb400_readl(REG_INT_STATUS) & ~(1 << (irq - IRQ_BASE)),
		     REG_INT_STATUS);
	rb400_readl(REG_INT_STATUS);
}

static void rb400_unmask_irq(unsigned int irq)
{
	rb400_writel(rb400_readl(REG_INT_MASK) | (1 << (irq - IRQ_BASE)),
		     REG_INT_MASK);
	rb400_readl(REG_INT_MASK);
}

static void rb400_mask_irq(unsigned int irq)
{
	rb400_writel(rb400_readl(REG_INT_MASK) & ~(1 << (irq - IRQ_BASE)),
		     REG_INT_MASK);
	rb400_readl(REG_INT_MASK);
}

static struct irq_chip rb700_irq_controller = {
	.typename	= "RB700",
	.ack		= rb400_ack_irq,
	.mask		= rb400_mask_irq,
	.unmask		= rb400_unmask_irq,
	.eoi		= rb400_unmask_irq,
};

static struct irq_chip rb400_irq_controller = {
	.typename	= "RB400",
	.ack		= rb400_mask_irq,
	.mask		= rb400_mask_irq,
	.mask_ack	= rb400_mask_irq,
	.unmask		= rb400_unmask_irq,
	.eoi		= rb400_unmask_irq,
};

static irqreturn_t rb400_cascade_irq(int irq, void *dev_id)
{
	int pending;
   
	pending = rb400_readl(REG_INT_STATUS) 
	    & rb400_readl(REG_INT_MASK) & 0xff;

	if (pending) {
		do_IRQ(fls(pending) - 1 + IRQ_BASE);
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}

static struct irqaction cascade  = {
	.handler = rb400_cascade_irq,
	.name = "cascade",
};

static void rb400_pci_unmask_irq(unsigned int irq)
{
	rb400_writel(
		rb400_readl(REG_PCI_INT_MASK) | (1 << (irq - PCI_IRQ_BASE)),
		REG_PCI_INT_MASK);
	rb400_readl(REG_PCI_INT_MASK);
}

static void rb400_pci_mask_irq(unsigned int irq)
{
	rb400_writel(
		rb400_readl(REG_PCI_INT_MASK) & ~(1 << (irq - PCI_IRQ_BASE)),
		REG_PCI_INT_MASK);
	rb400_readl(REG_PCI_INT_MASK);
}

static struct irq_chip rb400_pci_irq_controller = {
	.typename	= "RB400 PCI",
	.ack		= rb400_pci_mask_irq,
	.mask		= rb400_pci_mask_irq,
	.mask_ack	= rb400_pci_mask_irq,
	.unmask		= rb400_pci_unmask_irq,
	.eoi		= rb400_pci_unmask_irq,
};

static inline void rb400_flush_pci_to_memory(void)
{
	rb400_writel(1, REG_DDR_PCI_FLUSH);
	while ((rb400_readl(REG_DDR_PCI_FLUSH) & 0x1));
	rb400_writel(1, REG_DDR_PCI_FLUSH);
	while ((rb400_readl(REG_DDR_PCI_FLUSH) & 0x1));
}

static irqreturn_t rb400_pci_cascade_irq(int irq, void *dev_id)
{
	int pending;
   
	pending  = rb400_readl(REG_PCI_INT_STATUS) &
	    rb400_readl(REG_PCI_INT_MASK) & 0x7;

	rb400_flush_pci_to_memory();

	if (pending) {
		do_IRQ(fls(pending) - 1 + PCI_IRQ_BASE);
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}

static struct irqaction pci_cascade  = {
	.handler = rb400_pci_cascade_irq,
	.name = "pci-cascade",
};

void rb700_enable_pci_irq(void);

void __init rb400_init_irq(void)
{
	int i;
	struct irq_chip *irq_controller = &rb400_irq_controller;

	if (mips_machgroup == MACH_GROUP_MT_RB700) {
	    irq_controller = &rb700_irq_controller;
	}
	mips_cpu_irq_init();

	for (i = IRQ_BASE; i < IRQ_BASE + 8; i++)
		set_irq_chip_and_handler(i, irq_controller, handle_level_irq);
	setup_irq(6, &cascade);

	if (mips_machgroup == MACH_GROUP_MT_RB400 &&
	    mips_machtype != MACH_MT_RB450G &&
	    mips_machtype != MACH_MT_RB450) {
		for (i = PCI_IRQ_BASE; i < PCI_IRQ_BASE + 4; i++)
			set_irq_chip_and_handler(i, &rb400_pci_irq_controller,
						 handle_level_irq);
		setup_irq(2, &pci_cascade);
	}
}

static int __init base_init(void)
{
	switch (mips_machgroup) {
	case MACH_GROUP_MT_RB400:
	case MACH_GROUP_MT_RB700:
		int_base = ioremap(0x18060000, PAGE_SIZE);
		flush_base = ioremap(0x18000000, PAGE_SIZE);
		break;
	}
	return 0;
}
arch_initcall(base_init);
