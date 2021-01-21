#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <asm/delay.h>
#include <asm/traps.h>
#include <asm/rb/rb400.h>

/*
 * PCI cfg an I/O routines are done by programming a 
 * command/byte enable register, and then read/writing
 * the data from a data regsiter. We need to ensure
 * these transactions are atomic or we will end up
 * with corrupt data on the bus or in a driver.
 */

unsigned get_pci_crp(void) {
    static unsigned base = 0;
    if (!base) base = (unsigned) ioremap(0x180c0000, PAGE_SIZE);
    return base;
}

unsigned get_pci_dev_cfgbase(void) {
    static unsigned base = 0;
    if (!base) base = (unsigned) ioremap(0x14000000, PAGE_SIZE);
    return base;
}

#define RB700_PCI_CRP			get_pci_crp()
#define RB700_PCI_DEV_CFGBASE		get_pci_dev_cfgbase()
#define RB700_PCI_ERROR			RB700_PCI_CRP + 0x1c
#define RB700_PCI_ERROR_ADDRESS		RB700_PCI_CRP + 0x20
#define RB700_PCI_AHB_ERROR		RB700_PCI_CRP + 0x24
#define RB700_PCI_AHB_ERROR_ADDRESS	RB700_PCI_CRP + 0x28

static DEFINE_SPINLOCK(ar7100_pci_lock);

static unsigned mask[4] = { 0xff, 0xffff, 0, 0xffffffff };

static unsigned read_word(unsigned addr, int where) {
    return rb400_readl(addr + (where & ~3));
}

static int read_config(unsigned addr, int where, int size, uint32_t *value) {
    unsigned long flags;
    spin_lock_irqsave(&ar7100_pci_lock, flags);
    *value = (read_word(addr, where) >> (8 * (where & 3))) & mask[size - 1];
    spin_unlock_irqrestore(&ar7100_pci_lock, flags);
    return PCIBIOS_SUCCESSFUL;
}

static int rb700_local_read_config(int where, int size, uint32_t *value) {
    return read_config(RB700_PCI_CRP, where, size, value);
}

static int write_config(unsigned addr, int where, int size, uint32_t value) {
    unsigned long flags, word, shift;
    spin_lock_irqsave(&ar7100_pci_lock, flags);    
    shift = 8 * (where & 3);
    word = (read_word(addr, where) & ~(mask[size - 1] << shift));
    rb400_writel(word | (value << shift), addr + (where & ~3));
    spin_unlock_irqrestore(&ar7100_pci_lock, flags);
    return PCIBIOS_SUCCESSFUL;
}

int rb700_local_write_config(int where, int size, uint32_t value) {
    return write_config(RB700_PCI_CRP, where, size, value);
}

static int rb700_pci_read_config(struct pci_bus *bus, unsigned int devfn,
			  int where, int size, uint32_t *value) {
    return devfn 
	? PCIBIOS_DEVICE_NOT_FOUND
	: read_config(RB700_PCI_DEV_CFGBASE, where, size, value);
}

static int rb700_pci_write_config(struct pci_bus *bus,  unsigned int devfn,
				  int where, int size, uint32_t value) {
    return devfn 
	? PCIBIOS_DEVICE_NOT_FOUND
	: write_config(RB700_PCI_DEV_CFGBASE, where, size, value);
}

struct pci_ops rb700_pci_ops = {
	.read =  rb700_pci_read_config,
	.write = rb700_pci_write_config,
};

int rb700_be_handler(struct pt_regs *regs, int is_fixup) {
    int error = 0, status, trouble = 0;
    error = rb400_readl(RB700_PCI_ERROR) & 3;

    if (error) {
        printk("PCI error %d at PCI addr 0x%x\n", 
	       error, rb400_readl(RB700_PCI_ERROR_ADDRESS));
        rb400_writel(error, RB700_PCI_ERROR);
        rb700_local_read_config(PCI_STATUS, 2, &status);
        printk("PCI status: %#x\n", status);
        trouble = 1;
    }

    error = 0;
    error = rb400_readl(RB700_PCI_AHB_ERROR) & 1;
    
    if (error) {
        printk("AHB error at AHB address 0x%x\n", 
	       rb400_readl(RB700_PCI_AHB_ERROR_ADDRESS));
        rb400_writel(error, RB700_PCI_AHB_ERROR);
        rb700_local_read_config(PCI_STATUS, 2, &status);
        printk("PCI status: %#x\n", status);
        trouble = 1;
    }

    printk("rb700 data bus error: cause %#x\n", read_c0_cause());
    return (is_fixup ? MIPS_BE_FIXUP : MIPS_BE_FATAL);
}
