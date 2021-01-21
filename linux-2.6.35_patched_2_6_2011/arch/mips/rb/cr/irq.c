#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/pm.h>

#include <asm/irq.h>
#include <asm/mipsregs.h>
#include <asm/irq_cpu.h>
#include <asm/rb/cr.h>

#define CR_MISR	0x20
#define CR_MIMR 0x24

static irqreturn_t cr_cascade_irq(int irq, void *dev_id)
{
	int pending;

	pending = CR_CNTRL_REG(CR_MISR) & CR_CNTRL_REG(CR_MIMR) & 0xfd;
	if (pending) {
		do_IRQ(fls(pending) - 1 + CR_MISC_IRQ_BASE);
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}

static void cr_unmask_irq(unsigned int irq)
{
	CR_CNTRL_REG(CR_MIMR) |= 1 << (irq - CR_MISC_IRQ_BASE);
	CR_CNTRL_REG(CR_MIMR); /* flush write */
}

static void cr_mask_irq(unsigned int irq)
{
	CR_CNTRL_REG(CR_MIMR) &= ~(1 << (irq - CR_MISC_IRQ_BASE));
	CR_CNTRL_REG(CR_MIMR); /* flush write */
}

static struct irq_chip cr_irq_type = {
	.typename	= "CR",
	.ack		= cr_mask_irq,
	.mask		= cr_mask_irq,
	.mask_ack	= cr_mask_irq,
	.unmask		= cr_unmask_irq,
	.eoi		= cr_unmask_irq,
};

static struct irqaction misc_irq = {
	.handler	= cr_cascade_irq,
	.name		= "cascade",
};

void __init cr_init_irq(void)
{
	int i;

	mips_cpu_irq_init();
	setup_irq(CR_MISC_IRQ, &misc_irq);
	
	for (i = CR_MISC_IRQ_BASE; i < CR_MISC_IRQ_BASE + 8; i++) {
		set_irq_chip_and_handler(i, &cr_irq_type,
					 handle_level_irq);
	}
}
