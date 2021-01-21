#include <linux/kernel.h>
#include <linux/console.h>
#include <linux/init.h>

#include <linux/io.h>

#define NOT_SET(reg, bit_nr) (!(read32(reg) & bit_nr))

static void write32(unsigned addr, unsigned val) {
    * (volatile unsigned *) addr = val;
}

static unsigned read32(unsigned addr) {
    return * (volatile unsigned *) addr;
}

static unsigned UART_BASE = 0;

#define UART_SR                                (UART_BASE + 0x08)
#define UART_CR                                (UART_BASE + 0x10)
#define UART_ISR                       (UART_BASE + 0x14)
#define UART_TCNT                      (UART_BASE + 0x40)
#define UART_TF0                       (UART_BASE + 0x70)

#define UART_GCMD_RES_TX_RDY_INT       (3 << 8)

static void uart_cmd(unsigned cmd) {
    write32(UART_CR, cmd);
}

static void printch(unsigned data) {
    if (!UART_BASE) {
	UART_BASE = (unsigned) ioremap_nocache(0x16640000, 0x1000);
    }

    if (NOT_SET(UART_SR, (1 << 3))) {
	while (NOT_SET(UART_ISR, (1 << 7))) { }
    }

    write32(UART_TCNT, 1);

    uart_cmd(UART_GCMD_RES_TX_RDY_INT);

    while (NOT_SET(UART_SR, (1 << 2))) { }

    write32(UART_TF0, data);
}

static void early_write(const char *s, unsigned n)
{
    while (n-- > 0) {
	if (*s == '\n')
	    printch('\r');
	printch(*s);
	s++;
    }
}

static void early_console_write(struct console *con, const char *s, unsigned n)
{
    early_write(s, n);
}

static struct console early_console = {
    .name =         "earlycon",
    .write =        early_console_write,
    .flags =        CON_PRINTBUFFER | CON_BOOT,
    .index =        -1,
};

asmlinkage void early_printk(const char *fmt, ...)
{
    char buf[512];
    int n;
    va_list ap;

    va_start(ap, fmt);
    n = vscnprintf(buf, sizeof(buf), fmt, ap);
    early_write(buf, n);
    va_end(ap);
}

int init_early_printk(void)
{
    register_console(&early_console);
    return 0;
}
