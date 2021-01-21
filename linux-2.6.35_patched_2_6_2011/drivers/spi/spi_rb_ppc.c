#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/of.h>
#include <asm/rb_aux.h>
#include <asm/io.h>

#include <linux/spi/spi.h>

#define RB_PPC_DEBUG 0
#define USE_SPINLOCK 0

#if USE_SPINLOCK
#define INIT_LOCK(lock)			spin_lock_init(lock);
/* WARNING: disabling of interrupts makes watchdog to reboot RB on big tasks */
#define LOCK_AND_SAVE(lock,flags)	spin_lock_irqsave(lock,flags)
#define UNLOCK_AND_RESTORE(lock,flags)	spin_unlock_irqrestore(lock,flags)
#else
#define INIT_LOCK(lock)			do {} while (0)
#define LOCK_AND_SAVE(lock,flags)	do {} while (0)
#define UNLOCK_AND_RESTORE(lock,flags)	do {} while (0)
#endif


struct rb_ppc_spi {
#if USE_SPINLOCK
	spinlock_t		lock;
#endif
	struct list_head	queue;
	union {
		void *		baddr;
	};
	void (*set_cs)(struct spi_device *spi, int on);
	int (*txrx)(struct rb_ppc_spi *rbspi, struct spi_transfer *t);
	int			cs_wait;
	int			busy:1;
};

static void set_cs(struct spi_device *spi, int on) {
	int bit = (int) spi->controller_data;
	if (spi->mode & SPI_CS_HIGH) on = !on;
	if (on) {
		change_latch(bit, 0);
	}
	else {
		change_latch(0, bit);
	}
}

static uint8_t get_spi_byte(struct rb_ppc_spi *rbspi) {
	return in_8(rbspi->baddr);
}

static void do_spi_byte(struct rb_ppc_spi *rbspi, uint8_t byte) {
	out_8(rbspi->baddr, byte);
#if RB_PPC_DEBUG
	printk("spi_byte sent 0x%x got 0x%x\n",
	       (unsigned)byte,
	       (unsigned)get_spi_byte(rbspi));
#endif
}

static int rb_ppc_spi_txrx(struct rb_ppc_spi *rbspi, struct spi_transfer *t)
{
	const unsigned char *rxv_ptr = NULL;
	const unsigned char *tx_ptr = t->tx_buf;
	unsigned char *rx_ptr = t->rx_buf;
	unsigned i;

#if RB_PPC_DEBUG
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
		do_spi_byte(rbspi, sdata);
		if (rx_ptr) {
			rx_ptr[i] = get_spi_byte(rbspi);
		}
		else if (rxv_ptr) {
			if (rxv_ptr[i] != get_spi_byte(rbspi)) {
				return i;
			}
		}
	}
	return i;
}

static void mpc8544_spi_init(struct rb_ppc_spi *rbspi)
{
	rbspi->set_cs = &set_cs;
	rbspi->txrx = &rb_ppc_spi_txrx;
}

static int rb_ppc_spi_msg(struct rb_ppc_spi *rbspi,
			 struct spi_message *m) {
	struct spi_transfer *t = NULL;

	m->status = 0;
	if (list_empty(&m->transfers))
		return -1;

	(rbspi->set_cs)(m->spi, 0);

	list_for_each_entry (t, &m->transfers, transfer_list) {
		int len = (rbspi->txrx)(rbspi, t);
		if (len != t->len) {
			m->status = -EMSGSIZE;
			break;
		}
		m->actual_length += len;

		if (t->cs_change) {
			if (list_is_last(&t->transfer_list, &m->transfers)) {
				/* wait for continuation */
#if RB_PPC_DEBUG
				printk("cs %d cont\n", m->spi->chip_select);
#endif
				return m->spi->chip_select;
			}
			(rbspi->set_cs)(m->spi, 1);
			ndelay(100);
			(rbspi->set_cs)(m->spi, 0);	// TODO: optimize
		}
	}

	(rbspi->set_cs)(m->spi, 1);
	return -1;
}

static void rb_ppc_spi_process_queue_locked(struct rb_ppc_spi *rbspi,
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
		UNLOCK_AND_RESTORE(&rbspi->lock, *flags);

		cs = rb_ppc_spi_msg(rbspi, m);
		m->complete(m->context);

		LOCK_AND_SAVE(&rbspi->lock, *flags);
	}
	rbspi->cs_wait = cs;
	rbspi->busy = 0;
	if (cs >= 0) {
		// TODO: add timer to unlock cs after 1s inactivity
	}
}

static int rb_ppc_spi_transfer(struct spi_device *spi,
			      struct spi_message *m) {
	struct rb_ppc_spi *rbspi = spi_master_get_devdata(spi->master);
	unsigned long flags;

	m->actual_length = 0;
	m->status = -EINPROGRESS;

	LOCK_AND_SAVE(&rbspi->lock, flags);
	list_add_tail(&m->queue, &rbspi->queue);
	if (rbspi->busy ||
	    (rbspi->cs_wait >= 0 && rbspi->cs_wait != m->spi->chip_select)) {
		/* job will be done later */
		UNLOCK_AND_RESTORE(&rbspi->lock, flags);
		return 0;
	}

	/* process job in current context */
	rb_ppc_spi_process_queue_locked(rbspi, &flags);
	UNLOCK_AND_RESTORE(&rbspi->lock, flags);
	return 0;
}

static int rb_ppc_spi_setup(struct spi_device *spi)
{
	struct rb_ppc_spi *rbspi = spi_master_get_devdata(spi->master);
	unsigned long flags;

	if ((spi->mode & ~SPI_CS_HIGH) != 3) {
		printk("RB_PPC SPI: mode %x not supported\n",
		       (unsigned)spi->mode);
		return -EINVAL;
	}
	if (spi->bits_per_word != 8 && spi->bits_per_word != 0) {
		printk("RB_PPC SPI: bits_per_word %u not supported\n",
		       (unsigned)spi->bits_per_word);
		return -EINVAL;
	}

	LOCK_AND_SAVE(&rbspi->lock, flags);
	(rbspi->set_cs)(spi, 1);
	if (rbspi->cs_wait == spi->chip_select && !rbspi->busy) {
		rbspi->cs_wait = -1;
		rb_ppc_spi_process_queue_locked(rbspi, &flags);
	}
	UNLOCK_AND_RESTORE(&rbspi->lock, flags);
	return 0;
}

static int rb_ppc_spi_probe(struct of_device *pdev,
			    const struct of_device_id *match)
{
	struct spi_master *master;
	struct rb_ppc_spi *rbspi;
	struct resource res;
	struct spi_board_info spi_microsd = {
		.modalias = "mmc_spi",
		.max_speed_hz = 10 * 1000 * 1000,
		.bus_num = 0,
		.chip_select = 0,
		.mode = SPI_MODE_3,
	};
	const unsigned *ss;
	int sslen = 0;
	int i;
	int err = 0;


	printk("RB_PPC SPI\n");

	if (of_address_to_resource(pdev->dev.of_node, 0, &res)) {
		dev_err(&pdev->dev, "no reg property for SPI master\n");
		err = -ENODEV;
		goto err_nomem;
	}
	ss = of_get_property(pdev->dev.of_node, "slave_select", &sslen);
	sslen /= 4;
	if (sslen < 2) {
		dev_err(&pdev->dev,
			"missing slave_select property for SPI master\n");
		err = -ENODEV;
		goto err_nomem;
	}
	master = spi_alloc_master(&pdev->dev, sizeof(*rbspi));
	if (master == NULL) {
		dev_err(&pdev->dev, "No memory for spi_master\n");
		err = -ENOMEM;
		goto err_nomem;
	}
	master->bus_num = 0;
	master->num_chipselect = sslen - 1;
	master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_CS_HIGH;
	master->setup    = rb_ppc_spi_setup;
	master->transfer = rb_ppc_spi_transfer;

	rbspi = spi_master_get_devdata(master);
	memset(rbspi, 0, sizeof(*rbspi));
	rbspi->cs_wait = -1;
	rbspi->baddr = ioremap_nocache(res.start, res.end - res.start + 1);

	INIT_LOCK(&rbspi->lock);
	INIT_LIST_HEAD(&rbspi->queue);

	if (ss[0] == 0x600) {
	}
	else {
		mpc8544_spi_init(rbspi);
	}

	err = spi_register_master(master);
	if (err) {
		dev_err(&pdev->dev, "Failed to register SPI master\n");
		goto err_register;
	}
	for (i = 1; i < sslen; ++i) {
		spi_microsd.chip_select = i - 1;
		spi_microsd.controller_data = (void *)(1 << ss[i]);
		spi_new_device(master, &spi_microsd);
	}
	return 0;

err_register:
	spi_master_put(master);
err_nomem:
	return err;
}

static struct of_device_id rb_ppc_spi_ids[] = {
	{ .name = "spi" },
	{}
};

static struct of_platform_driver rb_ppc_spi_driver = {
	.driver	= {
		.name = "rbppc-spi",
		.owner = THIS_MODULE,
		.of_match_table = rb_ppc_spi_ids,
	},
	.probe	= rb_ppc_spi_probe,
};

static int __init rb_ppc_spi_init(void)
{
	of_register_platform_driver(&rb_ppc_spi_driver);
	return 0;
}

static void __exit rb_ppc_spi_exit(void)
{
	of_unregister_platform_driver(&rb_ppc_spi_driver);
}

module_init(rb_ppc_spi_init);
module_exit(rb_ppc_spi_exit);
