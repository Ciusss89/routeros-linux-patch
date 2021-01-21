#ifndef MT_RB400_H
#define MT_RB400_H

#define IRQ_BASE 16
#define PCI_IRQ_BASE 48

#define GPIO_IRQ_BASE	96
#define GPIO_COUNT	32
#define GPIO_IRQ	2

static inline unsigned rb400_readl(unsigned addr) {
    return *(volatile unsigned *) addr;
}

static inline void rb400_writel(unsigned val, unsigned addr) {
    *(volatile unsigned *) addr = val;
}

static inline void rb400_modreg(unsigned addr, unsigned set, unsigned clear) {
    rb400_writel((rb400_readl(addr) & ~clear) | set, addr);
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
				      unsigned cnt, unsigned addr_size);

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

#define SSR_BIT_nLED1	(1 << 0)
#define SSR_BIT_nLED2	(1 << 1)
#define SSR_BIT_nLED3	(1 << 2)
#define SSR_BIT_nLED4	(1 << 3)
#define SSR_BIT_nLED5	(1 << 4)
#define SSR_BIT_FAN_ON	(1 << 6)
#define SSR_BIT_USB_EN	(1 << 7)

struct rb400_ssr_info {
    unsigned strobe;
    unsigned bytes;
    unsigned val;
};

struct rb400_spi_ctrl {
    unsigned char gpio_as_cs;
    unsigned char gpio_as_strobe;
    unsigned char cmd_prefix;
    unsigned char cs_as_strobe:1;
};

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

extern int ar9342_change_gpo(unsigned off, unsigned on);

extern int rb_change_or_get_cfg(unsigned off, unsigned on, unsigned *get);
extern int rb_change_cfg(unsigned off, unsigned on);

#define RB400_SPI_SWD_READ         0x04
#define RB400_SPI_SWD_ACK_OK       0x01
#define RB400_SPI_SWD_PARITY_ERROR 0x80
extern int rb400_spi_swd(struct spi_device *spi, unsigned cmd, unsigned *data);

int is_ar7240(void);

void rb400_beepled(int on);
unsigned register_wifi_gpo(void *obj,
			   void (*set_gpo)(void *, unsigned, unsigned));

struct rb400_leds {
    unsigned user_led;
    unsigned led1;
    unsigned led2;
    unsigned led3;
    unsigned led4;
    unsigned led5;
    unsigned usb_power_off;
    unsigned power_led;
    unsigned wlan_led;
    unsigned pcie_power_off;
    unsigned pcie2_power_off;
    unsigned lcd;
    unsigned button;
    unsigned pin_hole;
    unsigned fan_off;
    unsigned user_led2;
    unsigned sfp_led;
    unsigned link_act_led;
    unsigned all_leds;
    unsigned _eof;
};

enum pled_type {
    PLED_TYPE_GPIO = 0,
    PLED_TYPE_GPIO_OE = 1,
    PLED_TYPE_SHARED_GPIO = 2,
    PLED_TYPE_SHARED_RB400 = 4,
    PLED_TYPE_SSR_RB400 = 5,
    PLED_TYPE_SHARED_RB700 = 7,
    PLED_TYPE_SSR_MUSIC = 8,
    PLED_TYPE_SHARED_RB900 = 9,
    PLED_TYPE_SSR_RB900 = 10,
    PLED_TYPE_WIFI = 15,
};

#define PLED_CFG_INV		(1U << 31)
#define PLED_CFG_ON		(1 << 30)
#define PLED_CFG_KEEP		(1 << 29)
#define PLED_VALID		(1 << 28)

#define PLED_GET_BIT(val)	(1U << (((val) >> 8) & 0x1f))
#define PLED_GET_TYPE(val)	((val) & 0xff)

#define PLED(bit,type,cfg)	(((type) & 0xff) |		\
				 (((bit) & 0x1f) << 8) |	\
				 (cfg) | PLED_VALID)
#define PLDI(bit,type)		PLED(bit, type, PLED_CFG_INV)
#define PLD(bit,type)		PLED(bit, type, 0)

#define RB400_GPIO_INVERT_OE	(1 << 0)
#define RB400_GPIO11_RESET_FN	(1 << 1)
#define RB400_GPIO_BASE2	(1 << 2)

extern void (*music_reset_notify)(void);
extern unsigned music_change_ssr_cfg(unsigned off, unsigned on);
#endif
