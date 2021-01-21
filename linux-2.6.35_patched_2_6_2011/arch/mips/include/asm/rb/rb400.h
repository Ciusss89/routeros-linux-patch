#ifndef MT_RB400_H
#define MT_RB400_H

#define IRQ_BASE 16
#define PCI_IRQ_BASE 48

static inline unsigned rb400_readl(unsigned addr) {
    return *(volatile unsigned *) addr;
}

static inline void rb400_writel(unsigned val, unsigned addr) {
    *(volatile unsigned *) addr = val;
}

/* SPI commands processed by CPLD */
#define SPI_CMD_WRITE_NAND	0x08	/* send cmd, n x send data, read idle */
#define SPI_CMD_READ_NAND	0x0a	/* send cmd, send idle, n x read data */
#define SPI_CMD_WRITE_CFG	0x09	/* send cmd, n x send cfg */
#define SPI_CMD_READ_FAST	0x0b	/* send cmd, 3 x addr, */
					/*	     send idle, n x read data */
#define   SPI_READ_NAND_OFS	    0x800000
#define SPI_CMD_LED5_ON		0x0c	/* send cmd */
#define SPI_CMD_LED5_OFF	0x0d	/* send cmd */
#define SPI_CMD_SDPWR_ON	0x0e	/* send cmd */
#define SPI_CMD_SDPWR_OFF	0x0f	/* send cmd */
#define SPI_CMD_MON_VOLTAGE	0x11	/* send cmd */
#define SPI_CMD_MON_TEMP	0x12	/* send cmd */
#define SPI_CMD_READ_VOLTS	0x1a	/* send cmd, 3 x read data (LSB 1st) */

struct spi_message;
struct spi_device;

extern struct spi_device *rb400_spi_get(void);
extern int rb400_spi_sync(struct spi_message *message);

extern int rb400_spiflash_read_verify(unsigned addr,
				      uint8_t *rdata, const uint8_t *vdata,
				      unsigned cnt);

#define CFG_BIT_nCE	0x80
#define CFG_BIT_CLE	0x40
#define CFG_BIT_ALE	0x20
#define CFG_BIT_FAN	0x10
#define CFG_BIT_nLED4	0x08
#define CFG_BIT_nLED3	0x04
#define CFG_BIT_nLED2	0x02
#define CFG_BIT_nLED1	0x01
#define CFG_BIT_nLED5  0x100

extern void rb400_change_cfg(unsigned off, unsigned on);


#define GPO_RB700_LATCH_EN	(1 << 0)
#define GPO_RB700_nPLED		(1 << 1)
#define GPO_RB700_MON_SEL	(1 << 9)
#define GPO_RB700_NOLATCH_nULED	(1 << 11)
#define GPO_RB700_nULED		(1 << 12)
#define GPO_RB700_USB_nPWROFF	(1 << 13)
#define GPO_RB700_nLINK0	(1 << 13)
#define GPO_RB700_nLINK1	(1 << 14)
#define GPO_RB700_nLINK2	(1 << 15)
#define GPO_RB700_nLINK3	(1 << 16)
#define GPO_RB700_nLINK4	(1 << 17)

extern int rb700_change_gpo(unsigned off, unsigned on);
extern void rb700_beepled(int on);

int is_ar7240(void);

unsigned register_wifi_gpo(void *obj,
			   void (*set_gpo)(void *, unsigned, unsigned));

#endif
