#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/hardirq.h>
#include <linux/platform_device.h>
#include <asm/rb/rb400.h>

#include <linux/spi/spi.h>

#define RB400_DEBUG 0
#define REG_PLL_CONFIG	0x18050000

#if RB400_DEBUG
#define CLK_HALF_DELAY() ndelay(20000)
#else
#define CLK_HALF_DELAY() while(0) {}
#endif

#define SPI_BASE	0x1F000000
#define   SPI_FUNC	  0x000000
#define     SBIT_FUNC_GPIO	0x00000001
#define   SPI_CTRL	  0x000004
#define	    SPI_CTRL_FASTEST	0x40
#define     SPI_FLASH_HZ	33333334
#define     SPI_CPLD_HZ		33333334
#define   SPI_IO_CTRL	  0x000008
#define     SBIT_DO2_SHIFT	18
#define     SBIT_DO2		(1u << SBIT_DO2_SHIFT)
#define     SBIT_CS_ALL		0x00070000
#define     SBIT_CS_2		0x00040000
#define     SBIT_CS_1		0x00020000
#define     SBIT_CS_0		0x00010000
#define     SBIT_CLK		0x00000100
#define     SBIT_DO_SHIFT	0
#define     SBIT_DO		(1u << SBIT_DO_SHIFT)
#define   SPI_RDATA	  0x00000C

#define SPI_REG(x)	(*(volatile unsigned *)((unsigned) spi_base + (x)))

extern unsigned mips_hpt_frequency;
static void __iomem *spi_base;

struct rb400_spi {
	unsigned		spi_ctrl_flash;
	unsigned		spi_ctrl_fread;

	unsigned long		cur_jiffies;
	long			req_till_schedule;

	spinlock_t		lock;
	struct list_head	queue;
	int			busy:1;
	int			cs_wait;
};
static unsigned spi_clk_low = SBIT_CS_ALL;
static unsigned spi_hclk_delay = 0;
static const unsigned spi_cs_map[] = {
	SBIT_CS_ALL ^ SBIT_CS_0,
	SBIT_CS_ALL ^ SBIT_CS_0 ^ SBIT_CS_2,
	SBIT_CS_ALL ^ SBIT_CS_1,
	SBIT_CS_ALL ^ SBIT_CS_2,
	SBIT_CS_ALL
};

static void inline do_spi_init(struct spi_device *spi) {
	unsigned hz = spi->max_speed_hz;
	if (hz == 0 || hz >= 10000000) {
		spi_hclk_delay = 0;
	}
	else {
		spi_hclk_delay = 500000000 / hz - 50;
	}
#if RB400_DEBUG
	if (spi_hclk_delay < 20000) spi_hclk_delay = 20000;
#endif
	if (unlikely(spi->mode & SPI_CS_HIGH)) {
		spi_clk_low = SBIT_CS_ALL;
	}
	else {
		spi_clk_low = spi_cs_map[spi->chip_select];
		SPI_REG(SPI_IO_CTRL) = spi_clk_low;
#if RB400_DEBUG
		printk("do_spi_init CS %x\n",
		       (spi_clk_low ^ SBIT_CS_ALL) >> 16);
#endif
	}
}
static void inline do_spi_finish(void) {
	if (unlikely(spi_hclk_delay)) ndelay(spi_hclk_delay);
	SPI_REG(SPI_IO_CTRL) = (SBIT_CS_ALL | (SPI_REG(SPI_IO_CTRL) & 1));
#if RB400_DEBUG
	printk("do_spi_finish (CS %x)\n", SBIT_CS_ALL >> 16);
#endif
}

static void inline do_spi_clk(int bit) {
	unsigned bval = spi_clk_low | (bit & 1);
	CLK_HALF_DELAY();
	SPI_REG(SPI_IO_CTRL) = bval;
	CLK_HALF_DELAY();
	SPI_REG(SPI_IO_CTRL) = bval | SBIT_CLK;
}

static void do_spi_byte(uint8_t byte) {
	do_spi_clk(byte >> 7);
	do_spi_clk(byte >> 6);
	do_spi_clk(byte >> 5);
	do_spi_clk(byte >> 4);
	do_spi_clk(byte >> 3);
	do_spi_clk(byte >> 2);
	do_spi_clk(byte >> 1);
	do_spi_clk(byte);
#if RB400_DEBUG
	printk("spi_byte sent 0x%x got 0x%x\n",
	       (unsigned)byte,
	       SPI_REG(SPI_RDATA));
#endif
}

static void inline do_spi_clk_delay(int bit) {
	unsigned bval = spi_clk_low | (bit & 1);
	ndelay(spi_hclk_delay);
	SPI_REG(SPI_IO_CTRL) = bval;
	ndelay(spi_hclk_delay);
	SPI_REG(SPI_IO_CTRL) = bval | SBIT_CLK;
}

static void do_spi_byte_delay(uint8_t byte) {
	do_spi_clk_delay(byte >> 7);
	do_spi_clk_delay(byte >> 6);
	do_spi_clk_delay(byte >> 5);
	do_spi_clk_delay(byte >> 4);
	do_spi_clk_delay(byte >> 3);
	do_spi_clk_delay(byte >> 2);
	do_spi_clk_delay(byte >> 1);
	do_spi_clk_delay(byte);
#if RB400_DEBUG
	printk("spi_byte_delay sent 0x%x got 0x%x\n",
	       (unsigned)byte,
	       SPI_REG(SPI_RDATA));
#endif
}

static void inline do_spi_clk_fast(int bit1, int bit2) {
	unsigned bval = (spi_clk_low |
			 ((bit1 << SBIT_DO_SHIFT) & SBIT_DO) |
			 ((bit2 << SBIT_DO2_SHIFT) & SBIT_DO2));
	CLK_HALF_DELAY();
	SPI_REG(SPI_IO_CTRL) = bval;
	CLK_HALF_DELAY();
	SPI_REG(SPI_IO_CTRL) = bval | SBIT_CLK;
}

static void do_spi_byte_fast(uint8_t byte) {
	do_spi_clk_fast(byte >> 7, byte >> 6);
	do_spi_clk_fast(byte >> 5, byte >> 4);
	do_spi_clk_fast(byte >> 3, byte >> 2);
	do_spi_clk_fast(byte >> 1, byte >> 0);
#if RB400_DEBUG
	printk("spi_byte_fast sent 0x%x got 0x%x\n",
	       (unsigned)byte,
	       SPI_REG(SPI_RDATA));
#endif
}

static int rb400_spi_txrx(struct spi_transfer *t)
{
	const unsigned char *rxv_ptr = NULL;
	const unsigned char *tx_ptr = t->tx_buf;
	unsigned char *rx_ptr = t->rx_buf;
	unsigned i;

#if RB400_DEBUG
	printk("spi_txrx len %u tx %u rx %u\n",
	       t->len,
	       (t->tx_buf ? 1 : 0),
	       (t->rx_buf ? 1 : 0));
#endif
	if (t->verify) {
		rxv_ptr = tx_ptr;
		tx_ptr = NULL;
	}

	for (i = 0; i < t->len; ++i) {
		unsigned char sdata = tx_ptr ? tx_ptr[i] : 0;
		if (t->fast_write)
			do_spi_byte_fast(sdata);
		else if (unlikely(spi_hclk_delay))
			do_spi_byte_delay(sdata);
		else
			do_spi_byte(sdata);
		if (rx_ptr) {
			rx_ptr[i] = SPI_REG(SPI_RDATA) & 0xff;
		}
		else if (rxv_ptr) {
			if (rxv_ptr[i] != (unsigned char)SPI_REG(SPI_RDATA)) {
				return i;
			}
		}
	}
	return i;
}

static int rb400_spi_read_fast(struct rb400_spi *rbspi,
			       struct spi_message *m) {
	struct spi_transfer *t;
	const unsigned char *tx_ptr;
	unsigned addr;

	/* check for exactly two transfers */
	if (list_empty(&m->transfers) ||
	    list_is_last(m->transfers.next, &m->transfers) ||
	    !list_is_last(m->transfers.next->next, &m->transfers)) {
		return -1;
	}

	/* first transfer contains command and address  */
	t = list_entry(m->transfers.next,
		       struct spi_transfer, transfer_list);
	if (t->len != 5 || t->tx_buf == NULL) {
		return -1;
	}
	tx_ptr = t->tx_buf;
	if (tx_ptr[0] != SPI_CMD_READ_FAST) {
		return -1;
	}
	addr = tx_ptr[1];
	addr = tx_ptr[2] | (addr << 8);
	addr = tx_ptr[3] | (addr << 8);
	addr += (unsigned) spi_base;

	m->actual_length += t->len;

	/* second transfer contains data itself */
	t = list_entry(m->transfers.next->next,
		       struct spi_transfer, transfer_list);

	if (t->tx_buf && !t->verify) {
		return -1;
	}

	SPI_REG(SPI_FUNC) = SBIT_FUNC_GPIO;
	SPI_REG(SPI_CTRL) = rbspi->spi_ctrl_fread;
	SPI_REG(SPI_FUNC) = 0;

	if (t->rx_buf) {
		memcpy(t->rx_buf, (const void *)addr, t->len);
	}
	else if (t->tx_buf) {
		unsigned char buf[t->len];
		memcpy(buf, (const void *)addr, t->len);
		if (memcmp(t->tx_buf, buf, t->len) != 0) {
			m->status = -EMSGSIZE;
		}
	}
	m->actual_length += t->len;

	if (rbspi->spi_ctrl_flash != rbspi->spi_ctrl_fread) {
		SPI_REG(SPI_FUNC) = SBIT_FUNC_GPIO;
		SPI_REG(SPI_CTRL) = rbspi->spi_ctrl_flash;
		SPI_REG(SPI_FUNC) = 0;
	}
	
	return 0;
}

static int rb400_spi_msg(struct rb400_spi *rbspi,
			 struct spi_message *m) {
	struct spi_transfer *t = NULL;

	m->status = 0;
	if (list_empty(&m->transfers))
		return -1;
	if (m->fast_read) {
		if (rb400_spi_read_fast(rbspi, m) == 0)
			return -1;
	}

	SPI_REG(SPI_FUNC) = SBIT_FUNC_GPIO;
	SPI_REG(SPI_CTRL) = SPI_CTRL_FASTEST;
	do_spi_init(m->spi);

	list_for_each_entry (t, &m->transfers, transfer_list) {
		int len = rb400_spi_txrx(t);
		if (len != t->len) {
			m->status = -EMSGSIZE;
			break;
		}
		m->actual_length += len;

		if (t->cs_change) {
			if (list_is_last(&t->transfer_list, &m->transfers)) {
				/* wait for continuation */
				return m->spi->chip_select;
			}
			do_spi_finish();
			ndelay(100);
		}
	}

	do_spi_finish();
	SPI_REG(SPI_CTRL) = rbspi->spi_ctrl_flash;
	SPI_REG(SPI_FUNC) = 0;
	return -1;
}

static void rb400_spi_process_queue_locked(struct rb400_spi *rbspi,
					   unsigned long *flags) {
	int cs = rbspi->cs_wait;
	rbspi->busy = 1;
	while (!list_empty(&rbspi->queue)) {
		struct spi_message *m;
		list_for_each_entry(m, &rbspi->queue, queue) {
			if (cs < 0 || cs == m->spi->chip_select) break;
		}
		if (&m->queue == &rbspi->queue) break;

		list_del_init(&m->queue);
		spin_unlock_irqrestore(&rbspi->lock, *flags);

		cs = rb400_spi_msg(rbspi, m);
		rbspi->req_till_schedule -= m->actual_length;
		m->complete(m->context);

		spin_lock_irqsave(&rbspi->lock, *flags);
	}
	rbspi->cs_wait = cs;
	rbspi->busy = 0;
	if (cs >= 0) {
		// TODO: add timer to unlock cs after 1s inactivity
	}
}

static int rb400_spi_transfer(struct spi_device *spi,
			      struct spi_message *m) {
	struct rb400_spi *rbspi = spi_master_get_devdata(spi->master);
	unsigned long flags;

	m->actual_length = 0;
	m->status = -EINPROGRESS;

	spin_lock_irqsave(&rbspi->lock, flags);
	if (m->spi->chip_select == 2 && rbspi->cs_wait == -1) {
		if (rbspi->cur_jiffies != jiffies) {
			rbspi->cur_jiffies = jiffies;
			rbspi->req_till_schedule = 3000;
		}
		else if (rbspi->req_till_schedule < 0 && !in_interrupt()) {
			rbspi->req_till_schedule = (1 << 30);
			spin_unlock_irqrestore(&rbspi->lock, flags);
			schedule();
			spin_lock_irqsave(&rbspi->lock, flags);
		}
	}
	list_add_tail(&m->queue, &rbspi->queue);
	if (rbspi->busy ||
	    (rbspi->cs_wait >= 0 && rbspi->cs_wait != m->spi->chip_select)) {
		/* job will be done later */
		spin_unlock_irqrestore(&rbspi->lock, flags);
		return 0;
	}

	/* process job in current context */
	rb400_spi_process_queue_locked(rbspi, &flags);
	spin_unlock_irqrestore(&rbspi->lock, flags);

	return 0;
}

static int rb400_spi_setup(struct spi_device *spi)
{
	struct rb400_spi *rbspi = spi_master_get_devdata(spi->master);
	unsigned long flags;

	if (spi->mode & ~(SPI_CS_HIGH)) {
		printk("RB400 SPI: mode %x not supported\n",
		       (unsigned)spi->mode);
		return -EINVAL;
	}
	if (spi->bits_per_word != 8 && spi->bits_per_word != 0) {
		printk("RB400 SPI: bits_per_word %u not supported\n",
		       (unsigned)spi->bits_per_word);
		return -EINVAL;
	}

	spin_lock_irqsave(&rbspi->lock, flags);
	if (rbspi->cs_wait == spi->chip_select && !rbspi->busy) {
		rbspi->cs_wait = -1;
		rb400_spi_process_queue_locked(rbspi, &flags);
	}
	spin_unlock_irqrestore(&rbspi->lock, flags);
	return 0;
}

static unsigned get_spi_ctrl(unsigned hz_max, const char *name) {
	void __iomem *pll_config = ioremap(REG_PLL_CONFIG, PAGE_SIZE);
	unsigned ahb_div = ((rb400_readl((unsigned) pll_config) >> 20) & 0x7)
	    + 1;
	unsigned ahb_clock = (mips_hpt_frequency  + ahb_div - 1) / ahb_div;
	unsigned div = (ahb_clock - 1) / (2 * hz_max);
	if (div == 0) {
		// CPU has a bug at (div == 0) - first bit read is random
		++div;
	}
	if (name) {
		unsigned ahb_khz = (ahb_clock + 500) / 1000;
		unsigned div_real = 2 * (div + 1);
		printk(KERN_INFO "%s SPI clock %u kHz (AHB %u kHz / %u)\n",
		       name,
		       ahb_khz / div_real,
		       ahb_khz, div_real);
	}
	iounmap(pll_config);
	return SPI_CTRL_FASTEST + div;
}

static void rb400_spi_add_devices(struct spi_master *master, const void *data) {
	struct spi_board_info **info;
	for (info = (struct spi_board_info **)data; *info != NULL; ++info) {
		(void) spi_new_device(master, *info);
	}
}

static int rb400_spi_probe(struct platform_device *pdev)
{
	struct spi_master *master;
	struct rb400_spi *rbspi;
	int err = 0;

	master = spi_alloc_master(&pdev->dev, sizeof(*rbspi));
	if (master == NULL) {
		dev_err(&pdev->dev, "No memory for spi_master\n");
		err = -ENOMEM;
		goto err_nomem;
	}
	master->bus_num = 0;
	master->num_chipselect = 4;
	master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_CS_HIGH;
	master->setup    = rb400_spi_setup;
	master->transfer = rb400_spi_transfer;

	rbspi = spi_master_get_devdata(master);
	memset(rbspi, 0, sizeof(*rbspi));
	rbspi->spi_ctrl_flash = get_spi_ctrl(SPI_FLASH_HZ, "FLASH");
	rbspi->spi_ctrl_fread = get_spi_ctrl(SPI_CPLD_HZ, "CPLD");
	rbspi->cs_wait = -1;

	spin_lock_init(&rbspi->lock);
	INIT_LIST_HEAD(&rbspi->queue);

	err = spi_register_master(master);
	if (err) {
		dev_err(&pdev->dev, "Failed to register SPI master\n");
		goto err_register;
	}
	rb400_spi_add_devices(master, pdev->dev.platform_data);
	return 0;

err_register:
	spi_master_put(master);;
err_nomem:
	return err;
}

static struct platform_driver rb400_spi_drv = {
	.probe		= rb400_spi_probe,
        .driver		= {
		.name	= "rb400-spi",
		.owner	= THIS_MODULE,
        },
};

static int __init rb400_spi_init(void)
{
	spi_base = ioremap(SPI_BASE, 0x01000000);
	if (!spi_base)
		return -ENOMEM;

        return platform_driver_register(&rb400_spi_drv);
}

static void __exit rb400_spi_exit(void)
{
        platform_driver_unregister(&rb400_spi_drv);
	iounmap(spi_base);
}

module_init(rb400_spi_init);
module_exit(rb400_spi_exit);
