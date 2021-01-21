#include <linux/platform_device.h>
#include <linux/serial.h>
#include <linux/serial_8250.h>
#include <linux/spi/spi.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/flash.h>
#include <linux/i2c-gpio.h>
#include <linux/mdio-gpio.h>
#include <linux/poeout.h>
#include <asm/bootinfo.h>
#include <asm/rb/boards.h>
#include <asm/rb/rb400.h>
#include <asm/rb/booter.h>
#include <asm/serial.h>

struct hwopt_device {
	struct platform_device *plat;
	unsigned val;
	unsigned mask;
};

static int platform_add_hwopt_devices(struct hwopt_device *devs, int cnt) {
	int i;
	unsigned hw_opt = 0;
	read_booter_cfg(ID_HW_OPTIONS, &hw_opt, 4);
	printk("hw_options %x\n", hw_opt);

	for (i = 0; i < cnt; ++i) {
		struct hwopt_device *dev = &devs[i];
		if ((hw_opt & (dev->val | dev->mask)) == dev->val) {
			printk("adding platform device %s\n", dev->plat->name);
			platform_device_register(dev->plat);
		}
	}
	return 0;
}

static struct platform_device rb100_uart_device = {
	.name	= "rb100-uart",
	.id	= -1,
};

static struct platform_device rb500_nand_device = {
	.name	= "rb500-nand",
	.id	= -1,
};

static struct platform_device rb500r5_nand_device = {
	.name	= "rb500r5-nand",
	.id	= -1,
};

static struct platform_device rb700_nand_device = {
	.name	= "rb700-nand",
	.id	= -1,
	.dev	= {
		.platform_data = (void *)0,
	},
};

static struct platform_device rb700_nolatch_nand_device = {
	.name	= "rb700-nand",
	.id	= -1,
	.dev	= {
		.platform_data = (void *)1,
	},
};

static struct platform_device rb700_tiny_nand_device = {
	.name	= "rb700-nand",
	.id	= -1,
	.dev	= {
		.platform_data = (void *)2,
	},
};

static struct platform_device rb750g_nand_device = {
	.name	= "rb750g-nand",
	.id	= -1,
};

static struct platform_device rb100_nand_device = {
	.name	= "rb100-nand",
	.id	= -1,
};

static struct platform_device cr_nand_device = {
	.name	= "cr-nand",
	.id	= -1,
};

static struct platform_device mr_nand_device = {
	.name	= "mr-nand",
	.id	= -1,
};

static struct platform_device ar9342_nand_device = {
	.name	= "ar9342-nand",
	.id	= -1,
};

static struct platform_device ar9344_nand_device = {
	.name	= "ar9344-nand",
	.id	= -1,
};

static struct spi_board_info rb400_spi_misc = {
	.modalias = "rb400-spi-misc",
	.max_speed_hz = 10 * 1000 * 1000,
	.bus_num = 0,
	.chip_select = 0,
	.mode = SPI_MODE_0,
};

static struct rb400_ssr_info rb400_ssr_data = {
	.strobe = 6,
	.bytes = 1,
	.val = (SSR_BIT_nLED1 | SSR_BIT_nLED2 |
		SSR_BIT_nLED3 | SSR_BIT_nLED4 |
		SSR_BIT_nLED5 |
		SSR_BIT_FAN_ON | SSR_BIT_USB_EN),
};
static struct spi_board_info rb400l_spi_misc = {
	.modalias = "rb400-spi-misc",
	.max_speed_hz = 10 * 1000 * 1000,
	.bus_num = 0,
	.chip_select = 0,
	.mode = SPI_MODE_0,
	.platform_data = &rb400_ssr_data,
};

static struct mtd_partition nor_part_table[] = {
    {
	.name = "RouterOS",
	.offset = 128 * 1024,
	.size = MTDPART_SIZ_FULL
    },
    {
	.name = "RouterBoot",
	.offset = 0,
	.size = 128 * 1024,
    },
};
static struct flash_platform_data nor_data = {
        .parts = nor_part_table,
        .use_4b_cmd = 1,
        .nr_parts = 2,
};

static struct spi_board_info rb400_spi_nand = {
	.modalias = "rb400-spi-nand",
	.max_speed_hz = 33 * 1000 * 1000,
	.bus_num = 0,
	.chip_select = 1,
	.mode = SPI_MODE_0,
};
static struct spi_board_info rb400_spi_microsd = {
	.modalias = "mmc_spi",
	.max_speed_hz = 10 * 1000 * 1000,
	.bus_num = 0,
	.chip_select = 2,
	.mode = SPI_MODE_0,
};
static struct spi_board_info rb700_spi_attiny = {
	.modalias = "rb700-spi-attiny",
	.max_speed_hz = 2 * 1000 * 1000,
	.bus_num = 0,
	.chip_select = 3,
	.mode = SPI_MODE_0,
};
static struct spi_board_info rb700_spi_nand = {
	.modalias = "rb700-spi-nand",
	.max_speed_hz = 0,
	.bus_num = 0,
	.chip_select = 1,
	.mode = SPI_CS_HIGH,
};

static struct spi_board_info *rb400_spi_info[] = {
	&rb400_spi_misc,
	&rb400_spi_nand,
	NULL
};

static struct spi_board_info *rb433_spi_info[] = {
	&rb400_spi_misc,
	&rb400_spi_nand,
	&rb400_spi_microsd,
	NULL
};

static struct spi_board_info *rb400l_spi_info[] = {
	&rb400l_spi_misc,
	NULL
};

static struct spi_board_info *rb750_spi_info[] = {
	&rb400_spi_misc,
	NULL
};

static struct spi_board_info *rb711_spi_info[] = {
	&rb400_spi_misc,
	&rb700_spi_nand,
	NULL
};

static struct spi_board_info *omni_spi_info[] = {
	&rb400_spi_misc,
	&rb700_spi_attiny,
	&rb700_spi_nand,
	NULL
};

static struct platform_device omni_spi_device = {
	.name	= "rb400-spi",
	.id	= -1,
	.dev	= {
		.platform_data = omni_spi_info,
	},
};

static struct platform_device rb400_spi_device = {
	.name	= "rb400-spi",
	.id	= -1,
	.dev	= {
		.platform_data = rb400_spi_info,
	},
};

static struct platform_device rb400l_spi_device = {
	.name	= "rb400-spi",
	.id	= -1,
	.dev	= {
		.platform_data = rb400l_spi_info,
	},
};

static struct platform_device rb433_spi_device = {
	.name	= "rb400-spi",
	.id	= -1,
	.dev	= {
		.platform_data = rb433_spi_info,
	},
};

static struct platform_device rb750_spi_device = {
	.name	= "rb400-spi",
	.id	= -1,
	.dev	= {
		.platform_data = rb750_spi_info,
	},
};

static struct platform_device rb711_spi_device = {
	.name	= "rb400-spi",
	.id	= -1,
	.dev	= {
		.platform_data = rb711_spi_info,
	},
};

static struct rb400_ssr_info rb900_ssr_data = {
	.strobe = 17,
	.bytes = 2,
	.val = 0x0c00
};
static struct spi_board_info rb900_spi_misc = {
	.modalias = "rb400-spi-misc",
	.max_speed_hz = 33333334,
	.bus_num = 0,
	.chip_select = 0,
	.mode = SPI_MODE_3,
	.platform_data = &rb900_ssr_data,
};

static struct spi_board_info rb2011_spi_lcd = {
	.modalias = "rb2011-spi-lcd",
	.max_speed_hz = 15 * 1000 * 1000,
	.bus_num = 0,
	.chip_select = 1,
	.mode = SPI_MODE_0,
};

static struct spi_board_info rb2011_spi_ts = {
	.modalias = "rb2011-spi-ts",
	.max_speed_hz = 2500 * 1000,
	.bus_num = 0,
	.chip_select = 2,
	.mode = SPI_MODE_3,
	.platform_data = (void *) 16,
};

static struct spi_board_info *rb2011_spi_info[] = {
	&rb900_spi_misc,
	&rb2011_spi_lcd,
	&rb2011_spi_ts,
	NULL
};

static struct platform_device rb2011_spi_device = {
	.name	= "rb900-spi",
	.id	= -1,
	.dev	= {
		.platform_data = rb2011_spi_info,
	},
};

static struct spi_board_info *rb2011L_spi_info[] = {
	&rb900_spi_misc,
	NULL
};

static struct platform_device rb2011L_spi_device = {
	.name	= "rb900-spi",
	.id	= -1,
	.dev	= {
		.platform_data = rb2011L_spi_info,
	},
};

static struct spi_board_info ar9342_spi_ssr = {
	.modalias = "ar9342-spi-ssr",
	.max_speed_hz = 33333334,
	.bus_num = 0,
	.chip_select = 4,
	.mode = SPI_MODE_3,
};
static struct spi_board_info *ar9342_spi_info[] = {
	&rb900_spi_misc,
	&ar9342_spi_ssr,
	NULL
};
static struct platform_device ar9342_spi_device = {
	.name	= "rb900-spi",
	.id	= -1,
	.dev	= {
		.platform_data = ar9342_spi_info,
	},
};

static struct spi_board_info music_spi_nand = {
	.modalias = "music-spi-nand",
	.max_speed_hz = 33333334,
	.bus_num = 0,
	.chip_select = 2,
	.mode = SPI_MODE_0,
};
static struct spi_board_info music_spi_lcd = {
	.modalias = "rb2011-spi-lcd",
	.max_speed_hz = 15 * 1000 * 1000,
	.bus_num = 0,
	.chip_select = 5,
	.mode = SPI_MODE_0,
};
static struct rb400_spi_ctrl music_ts_ctrl = {
	.cmd_prefix = 1 << 2,	// CMD_SEL_TS for Xilinx
};
static struct spi_board_info music_spi_ts = {
	.modalias = "rb2011-spi-ts",
	.max_speed_hz = 2500 * 1000,
	.bus_num = 0,
	.chip_select = 6,
	.mode = SPI_MODE_0,
	.platform_data = (void *) 2,
	.controller_data = &music_ts_ctrl,
};

static struct spi_board_info *music_spi_info[] = {
	&rb400_spi_misc,
	&music_spi_nand,
	&music_spi_lcd,
	&music_spi_ts,
	NULL
};
static struct platform_device music_spi_device = {
	.name	= "rb400-spi",
	.id	= -1,
	.dev	= {
		.platform_data = music_spi_info,
	},
};

static struct rb400_ssr_info crs212_ssr_data = {
	.bytes = 3,
	.val = 0xffffff
};
static struct rb400_spi_ctrl crs212_ssr_ctrl = {
	.cmd_prefix = 1 << 3,	// CMD_SEL_SSR for Xilinx
	.cs_as_strobe = 1,
};
static struct spi_board_info crs212_spi_ssr = {
	.modalias = "ccs-spi-ssr",
	.max_speed_hz = 10 * 1000 * 1000,
	.bus_num = 0,
	.chip_select = 10,
	.mode = SPI_MODE_0,
	.platform_data = &crs212_ssr_data,
	.controller_data = &crs212_ssr_ctrl,
};
static struct spi_board_info *crs212_spi_info[] = {
	&rb400_spi_misc,
	&music_spi_nand,
	&music_spi_lcd,
	&music_spi_ts,
	&crs212_spi_ssr,
	NULL
};
static struct platform_device crs212_spi_device = {
	.name	= "rb400-spi",
	.id	= -1,
	.dev	= {
		.platform_data = crs212_spi_info,
	},
};

static struct spi_board_info large_spi_nor = {
	.modalias = "m25p80",
	.max_speed_hz = 80 * 1000 * 1000,
	.bus_num = 0,
	.chip_select = 4,
	.mode = SPI_MODE_0,
        .platform_data = &nor_data,
};

static struct spi_board_info slow_large_spi_nor = {
	.modalias = "m25p80",
	.max_speed_hz = 30 * 1000 * 1000,
	.bus_num = 0,
	.chip_select = 4,
	.mode = SPI_MODE_0,
        .platform_data = &nor_data,
};

static struct spi_board_info *rb900l_spi_info[] = {
	&rb400_spi_misc,
	&large_spi_nor,
	NULL
};
static struct platform_device rb900l_spi_device = {
	.name	= "rb900-spi",
	.id	= -1,
	.dev	= {
		.platform_data = rb900l_spi_info,
	},
};

static struct spi_board_info *waspL_spi_info[] = {
	&large_spi_nor,
	NULL
};
static struct platform_device waspL_spi_device = {
	.name	= "rb900-spi",
	.id	= -1,
	.dev	= {
		.platform_data = waspL_spi_info,
	},
};

static struct spi_board_info *scorpionL_spi_info[] = {
	&slow_large_spi_nor,
	&rb2011_spi_ts,
	NULL
};
static struct platform_device scorpionL_spi_device = {
	.name	= "rb900-spi",
	.id	= -1,
	.dev	= {
		.platform_data = scorpionL_spi_info,
	},
};

static struct platform_device flash_nor_device = {
	.name	= "flash-nor",
	.id	= -1,
};

static struct platform_device flash_spi_device = {
	.name	= "flash-spi",
	.id	= -1,
};

static struct platform_device korina_device = {
	.name	= "korina",
	.id	= -1,
};

static struct platform_device rb100_led_device = {
	.name	= "rb100-led",
	.id	= -1,
};

static struct platform_device rb112_led_device = {
	.name	= "rb112-led",
	.id	= -1,
};

static struct rb400_leds rb400_led_map = {
	.user_led	= PLD(4, PLED_TYPE_GPIO),
	.led1		= PLD(0, PLED_TYPE_SHARED_RB400),
	.led2		= PLD(1, PLED_TYPE_SHARED_RB400),
	.led3		= PLD(2, PLED_TYPE_SHARED_RB400),
	.led4		= PLD(3, PLED_TYPE_SHARED_RB400),
	.led5		= PLD(8, PLED_TYPE_SHARED_RB400),
};
static struct platform_device rb400_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb400_led_map,
	},
};

static struct rb400_leds rb411l_led_map = {
	.user_led	= PLD(4, PLED_TYPE_GPIO),
	.led1		= PLDI(4, PLED_TYPE_SSR_RB400),
	.led2		= PLDI(3, PLED_TYPE_SSR_RB400),
	.led3		= PLDI(2, PLED_TYPE_SSR_RB400),
	.led4		= PLDI(1, PLED_TYPE_SSR_RB400),
	.led5		= PLDI(0, PLED_TYPE_SSR_RB400),
	.usb_power_off	= PLDI(7, PLED_TYPE_SSR_RB400),
};
static struct platform_device rb411l_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb411l_led_map,
	},
};

static struct rb400_leds rb450g_led_map = {
	.user_led	= PLD(4, PLED_TYPE_GPIO),
	.led1		= PLD(8, PLED_TYPE_SHARED_RB400),
};
static struct platform_device rb450g_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb450g_led_map,
	},
};

static struct rb400_leds rb493g_led_map = {
	.user_led	= PLD(4, PLED_TYPE_GPIO),
	.led1		= PLD(0, PLED_TYPE_SHARED_RB400),
	.led2		= PLD(2, PLED_TYPE_SHARED_RB400),
	.led3		= PLD(1, PLED_TYPE_SHARED_RB400),
	.led4		= PLD(3, PLED_TYPE_SHARED_RB400),
	.led5		= PLD(8, PLED_TYPE_SHARED_RB400),
};
static struct platform_device rb493g_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb493g_led_map,
	},
};

static struct rb400_leds rb700_led_map = {
	.user_led	= PLDI(12, PLED_TYPE_SHARED_RB700),
};
static struct platform_device rb700_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb700_led_map,
	},
};

static struct rb400_leds rb751_led_map = {
	.user_led	= PLDI(12, PLED_TYPE_SHARED_RB700),
	.wlan_led	= PLDI(9, PLED_TYPE_WIFI),
};
static struct platform_device rb751_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb751_led_map,
	},
};

static struct rb400_leds sxt5d_led_map = {
	.user_led	= PLDI(12, PLED_TYPE_SHARED_RB700),
	.led1		= PLD(5, PLED_TYPE_WIFI),
	.led2		= PLD(6, PLED_TYPE_WIFI),
	.led3		= PLD(7, PLED_TYPE_WIFI),
	.led4		= PLD(8, PLED_TYPE_WIFI),
	.led5		= PLD(9, PLED_TYPE_WIFI),
	.usb_power_off	= PLD(3, PLED_TYPE_WIFI),
};
static struct platform_device sxt5d_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &sxt5d_led_map,
	},
};

static struct rb400_leds groove_led_map = {
	.user_led	= PLDI(12, PLED_TYPE_SHARED_RB700),
	.led1		= PLD(9, PLED_TYPE_WIFI),
	.led2		= PLD(8, PLED_TYPE_WIFI),
	.led3		= PLD(7, PLED_TYPE_WIFI),
	.led4		= PLD(6, PLED_TYPE_WIFI),
	.led5		= PLD(5, PLED_TYPE_WIFI),
	.usb_power_off	= PLD(3, PLED_TYPE_WIFI),
};
static struct platform_device groove_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &groove_led_map,
	},
};

static struct rb400_leds rb711_led_map = {
	.user_led	= PLDI(1, PLED_TYPE_SHARED_RB700),
	.led1		= PLDI(12, PLED_TYPE_SHARED_RB700),
	.led2		= PLDI(13, PLED_TYPE_SHARED_RB700),
	.led3		= PLDI(14, PLED_TYPE_SHARED_RB700),
	.led4		= PLDI(15, PLED_TYPE_SHARED_RB700),
	.led5		= PLDI(16, PLED_TYPE_SHARED_RB700),
};
static struct platform_device rb711_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb711_led_map,
	},
};

static struct rb400_leds rb711r3_led_map = {
	.user_led	= PLDI(1, PLED_TYPE_SHARED_RB700),
	.led1		= PLDI(12, PLED_TYPE_SHARED_RB700),
	.led2		= PLDI(14, PLED_TYPE_SHARED_RB700),
	.led3		= PLDI(15, PLED_TYPE_SHARED_RB700),
	.led4		= PLDI(16, PLED_TYPE_SHARED_RB700),
	.usb_power_off	= PLD(4, PLED_TYPE_WIFI),
};
static struct platform_device rb711r3_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb711r3_led_map,
	},
};

static struct rb400_leds rb711g_led_map = {
	.user_led	= PLDI(1, PLED_TYPE_SHARED_RB700),
	.led1		= PLDI(12, PLED_TYPE_SHARED_RB700),
	.led2		= PLDI(14, PLED_TYPE_SHARED_RB700),
	.led3		= PLDI(15, PLED_TYPE_SHARED_RB700),
	.led4		= PLDI(16, PLED_TYPE_SHARED_RB700),
	.usb_power_off	= PLDI(13, PLED_TYPE_SHARED_RB700),
};
static struct platform_device rb711g_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb711g_led_map,
	},
};

static struct rb400_leds rb711gt_led_map = {
	.user_led	= PLDI(12, PLED_TYPE_SHARED_RB700),
	.led1		= PLD(10, PLED_TYPE_WIFI),
	.led2		= PLD(12, PLED_TYPE_WIFI),
	.led3		= PLD(13, PLED_TYPE_WIFI),
	.led4		= PLD(14, PLED_TYPE_WIFI),
	.led5		= PLD(15, PLED_TYPE_WIFI),
	.usb_power_off	= PLD(0, PLED_TYPE_WIFI),
	.power_led	= PLED(1, PLED_TYPE_WIFI, PLED_CFG_INV | PLED_CFG_ON),
};
static struct platform_device rb711gt_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb711gt_led_map,
	},
};

static struct rb400_leds rb951g_led_map = {
	.user_led	= PLD(4, PLED_TYPE_WIFI),
	.usb_power_off	= PLDI(20, PLED_TYPE_GPIO),
	.wlan_led	= PLD(0, PLED_TYPE_WIFI),
	.button		= PLDI(15, PLED_TYPE_GPIO),
	.pin_hole	= PLDI(14, PLED_TYPE_SHARED_GPIO),
};
static struct platform_device rb951g_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb951g_led_map,
	},
};

static struct rb400_leds rb2011l_led_map = {
	.user_led	= PLD(4, PLED_TYPE_WIFI),
	.button		= PLDI(15, PLED_TYPE_GPIO),
	.pin_hole	= PLDI(14, PLED_TYPE_SHARED_GPIO),
};
static struct platform_device rb2011L_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb2011l_led_map,
	},
};

static struct rb400_leds rb2011_led_map = {
	.sfp_led	= PLD(4, PLED_TYPE_WIFI),
	.usb_power_off	= PLDI(20, PLED_TYPE_GPIO_OE),
	.lcd		= PLED(2, PLED_TYPE_GPIO, PLED_CFG_KEEP),
	.button		= PLDI(15, PLED_TYPE_GPIO),
	.pin_hole	= PLDI(14, PLED_TYPE_SHARED_GPIO),
};
static struct platform_device rb2011_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb2011_led_map,
	},
};

static struct rb400_leds crs125_led_map = {
	.user_led2	= PLDI(11, PLED_TYPE_GPIO),
	.usb_power_off	= PLDI(20, PLED_TYPE_GPIO_OE),
	.button		= PLDI(15, PLED_TYPE_GPIO),
	.lcd		= PLED(2, PLED_TYPE_GPIO, PLED_CFG_KEEP),
};
static struct platform_device crs125_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &crs125_led_map,
	},
};

static struct rb400_leds crs125_wifi_led_map = {
	.wlan_led	= PLDI(11, PLED_TYPE_GPIO),
	.usb_power_off	= PLDI(20, PLED_TYPE_GPIO_OE),
	.button		= PLDI(15, PLED_TYPE_GPIO),
	.lcd		= PLED(2, PLED_TYPE_GPIO, PLED_CFG_KEEP),
};
static struct platform_device crs125_wifi_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &crs125_wifi_led_map,
	},
};

static struct rb400_leds rb2011r5_led_map = {
	.sfp_led	= PLD(4, PLED_TYPE_WIFI),
	.usb_power_off	= PLDI(20, PLED_TYPE_GPIO_OE),
	.lcd		= PLDI(0, PLED_TYPE_WIFI),
	.button		= PLDI(15, PLED_TYPE_GPIO),
	.pin_hole	= PLDI(14, PLED_TYPE_SHARED_GPIO),
};
static struct platform_device rb2011r5_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb2011r5_led_map,
	},
};

static struct rb400_leds rb2011r5_user_led_map = {
	.user_led	= PLD(4, PLED_TYPE_WIFI),
	.usb_power_off	= PLDI(20, PLED_TYPE_GPIO_OE),
	.lcd		= PLDI(0, PLED_TYPE_WIFI),
	.button		= PLDI(15, PLED_TYPE_GPIO),
	.pin_hole	= PLDI(14, PLED_TYPE_SHARED_GPIO),
};
static struct platform_device rb2011r5_user_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb2011r5_user_led_map,
	},
};

static struct rb400_leds sxtg_led_map = {
	.user_led	= PLDI(12, PLED_TYPE_SHARED_RB700),
	.led1		= PLD(5, PLED_TYPE_WIFI),
	.led2		= PLD(6, PLED_TYPE_WIFI),
	.led3		= PLD(7, PLED_TYPE_WIFI),
	.led4		= PLD(8, PLED_TYPE_WIFI),
	.led5		= PLD(9, PLED_TYPE_WIFI),
	.usb_power_off	= PLD(3, PLED_TYPE_WIFI),
	.power_led	= PLD(1, PLED_TYPE_WIFI),
};
static struct platform_device sxtg_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &sxtg_led_map,
	},
};

static struct rb400_leds rb750g_led_map = {
	.user_led	= PLD(4, PLED_TYPE_GPIO),
};
static struct platform_device rb750g_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb750g_led_map,
	},
};

static struct rb400_leds rb750gl_led_map = {
	.user_led	= PLDI(12, PLED_TYPE_SHARED_RB700),
	.usb_power_off	= PLDI(13, PLED_TYPE_SHARED_RB700),
};
static struct platform_device rb750gl_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb750gl_led_map,
	},
};

static struct rb400_leds rb751g_led_map = {
	.user_led	= PLDI(12, PLED_TYPE_SHARED_RB700),
	.usb_power_off	= PLDI(13, PLED_TYPE_SHARED_RB700),
	.wlan_led	= PLDI(9, PLED_TYPE_WIFI),
};
static struct platform_device rb751g_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb751g_led_map,
	},
};

static struct rb400_leds rb912g_led_map = {
	.user_led	= PLD(2, PLED_TYPE_SHARED_RB900),
	.led1		= PLD(0, PLED_TYPE_SSR_RB900),
	.led2		= PLD(1, PLED_TYPE_SSR_RB900),
	.led3		= PLD(2, PLED_TYPE_SSR_RB900),
	.led4		= PLD(3, PLED_TYPE_SSR_RB900),
	.led5		= PLD(4, PLED_TYPE_SSR_RB900),
	.usb_power_off	= PLDI(6, PLED_TYPE_SSR_RB900),
	.power_led	= PLED(1, PLED_TYPE_SHARED_RB900, PLED_CFG_ON),
	.pcie_power_off	= PLDI(7, PLED_TYPE_SSR_RB900),
	.button		= PLDI(15, PLED_TYPE_SHARED_RB900),
	.pin_hole	= PLDI(14, PLED_TYPE_SHARED_RB900),
};
static struct platform_device rb912g_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb912g_led_map,
	},
};

static struct rb400_leds rb922g_led_map = {
	.user_led	= PLDI(12, PLED_TYPE_GPIO),
	.led1		= PLDI(1, PLED_TYPE_WIFI),
	.led2		= PLDI(8, PLED_TYPE_WIFI),
	.led3		= PLDI(9, PLED_TYPE_WIFI),
	.led4		= PLDI(10, PLED_TYPE_WIFI),
	.led5		= PLDI(11, PLED_TYPE_WIFI),
	.usb_power_off	= PLD(13, PLED_TYPE_GPIO),
	.power_led	= PLED(0, PLED_TYPE_WIFI, PLED_CFG_ON),
	.pcie_power_off	= PLDI(17, PLED_TYPE_WIFI),
	.button		= PLDI(20, PLED_TYPE_GPIO),
	.fan_off	= PLDI(14, PLED_TYPE_GPIO),
};
static struct platform_device rb922g_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb922g_led_map,
	},
};

static struct rb400_leds sxtg5c_led_map = {
	.user_led	= PLD(16, PLED_TYPE_GPIO),
	.led1		= PLDI(1, PLED_TYPE_WIFI),
	.led2		= PLDI(8, PLED_TYPE_WIFI),
	.led3		= PLDI(9, PLED_TYPE_WIFI),
	.led4		= PLDI(10, PLED_TYPE_WIFI),
	.led5		= PLDI(11, PLED_TYPE_WIFI),
	.usb_power_off	= PLDI(11, PLED_TYPE_GPIO),
	.power_led	= PLED(7, PLED_TYPE_WIFI, PLED_CFG_ON),
	.button		= PLDI(20, PLED_TYPE_GPIO),
	.pin_hole	= PLDI(23, PLED_TYPE_SHARED_GPIO),
};
static struct platform_device sxtg5c_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &sxtg5c_led_map,
	},
};

static struct rb400_leds rb951HnD_led_map = {
	.user_led	= PLDI(3, PLED_TYPE_GPIO_OE),
	.led1		= PLDI(13, PLED_TYPE_GPIO),
	.led2		= PLDI(12, PLED_TYPE_GPIO),
	.led3		= PLDI(4, PLED_TYPE_GPIO),
	.led4		= PLDI(21, PLED_TYPE_GPIO),
	.led5		= PLDI(16, PLED_TYPE_GPIO),
	.usb_power_off	= PLDI(20, PLED_TYPE_GPIO),
	.wlan_led	= PLDI(11, PLED_TYPE_GPIO),
	.button		= PLDI(15, PLED_TYPE_GPIO),
	.pin_hole	= PLDI(14, PLED_TYPE_SHARED_GPIO),
};
static struct platform_device rb951HnD_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb951HnD_led_map,
	},
};

static struct rb400_leds sxt2d_led_map = {
	.user_led	= PLDI(3, PLED_TYPE_GPIO_OE),
	.led1		= PLDI(16, PLED_TYPE_GPIO),
	.led2		= PLDI(21, PLED_TYPE_GPIO),
	.led3		= PLDI(4, PLED_TYPE_GPIO),
	.led4		= PLDI(12, PLED_TYPE_GPIO),
	.led5		= PLDI(13, PLED_TYPE_GPIO),
	.power_led	= PLDI(11, PLED_TYPE_GPIO_OE),
	.button		= PLDI(15, PLED_TYPE_GPIO),
	.pin_hole	= PLDI(14, PLED_TYPE_SHARED_GPIO),
};
static struct platform_device sxt2d_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &sxt2d_led_map,
	},
};

static struct rb400_leds sxt5n_led_map = {
	.user_led	= PLDI(3, PLED_TYPE_GPIO_OE),
	.led1		= PLDI(13, PLED_TYPE_GPIO),
	.led2		= PLDI(12, PLED_TYPE_GPIO),
	.led3		= PLDI(4, PLED_TYPE_GPIO),
	.led4		= PLDI(21, PLED_TYPE_GPIO),
	.led5		= PLDI(18, PLED_TYPE_GPIO),
	.usb_power_off	= PLDI(20, PLED_TYPE_GPIO),
	.power_led	= PLDI(11, PLED_TYPE_GPIO_OE),
	.button		= PLDI(15, PLED_TYPE_GPIO),
	.pin_hole	= PLDI(14, PLED_TYPE_SHARED_GPIO),
};
static struct platform_device sxt5n_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &sxt5n_led_map,
	},
};

static struct rb400_leds rb911L_led_map = {
	.user_led	= PLDI(3, PLED_TYPE_GPIO_OE),
	.led1		= PLDI(13, PLED_TYPE_GPIO),
	.led2		= PLDI(12, PLED_TYPE_GPIO),
	.led3		= PLDI(4, PLED_TYPE_GPIO),
	.led4		= PLDI(21, PLED_TYPE_GPIO),
	.led5		= PLDI(18, PLED_TYPE_GPIO),
	.usb_power_off	= PLDI(20, PLED_TYPE_GPIO),
	.power_led	= PLDI(11, PLED_TYPE_GPIO_OE),
	.button		= PLDI(15, PLED_TYPE_GPIO),
	.pin_hole	= PLDI(14, PLED_TYPE_GPIO),
};
static struct platform_device rb911L_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb911L_led_map,
	},
};

static struct rb400_leds rb953_led_map = {
	.user_led	= PLD(16, PLED_TYPE_GPIO),
	.led1		= PLDI(4, PLED_TYPE_SSR_RB400),
	.led2		= PLDI(5, PLED_TYPE_SSR_RB400),
	.led3		= PLDI(6, PLED_TYPE_SSR_RB400),
	.led4		= PLDI(7, PLED_TYPE_SSR_RB400),
	.led5		= PLDI(8, PLED_TYPE_SSR_RB400),
	.usb_power_off	= PLDI(12, PLED_TYPE_SSR_RB400),
	.power_led	= PLED(9, PLED_TYPE_SSR_RB400,
			       PLED_CFG_INV | PLED_CFG_KEEP),
	.pcie_power_off	= PLDI(10, PLED_TYPE_SSR_RB400),
	.pcie2_power_off = PLDI(11, PLED_TYPE_SSR_RB400),
	.button		= PLDI(20, PLED_TYPE_GPIO),
	.pin_hole	= PLDI(23, PLED_TYPE_SHARED_GPIO),
};
static struct platform_device rb953_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb953_led_map,
	},
};

static struct rb400_leds crs226_led_map = {
	.lcd		= PLED(7, PLED_TYPE_SSR_MUSIC,
			       PLED_CFG_INV | PLED_CFG_KEEP),
	.button		= PLDI(0, PLED_TYPE_GPIO),
};
static struct platform_device crs226_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &crs226_led_map,
	},
};

static struct rb400_leds ccs112_led_map = {
	.button		= PLDI(0, PLED_TYPE_GPIO),
};
static struct platform_device ccs112_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &ccs112_led_map,
	},
};

static struct platform_device rb500_led_device = {
	.name	= "rb500-led",
	.id	= -1,
};

static struct platform_device cr_led_device = {
	.name	= "cr-led",
	.id	= -1,
};

static struct platform_device mr_led_device = {
	.name	= "mr-led",
	.id	= -1,
};

static struct platform_device rb400_gpio_device = {
	.name	= "rb400-gpio",
	.id	= -1,
};

static struct platform_device ar9340_gpio_device = {
	.name	= "rb400-gpio",
	.id	= -1,
	.dev	= {
		.platform_data = (void *)RB400_GPIO_INVERT_OE,
	},
};
static struct platform_device ar9344_gpio_device = {
	.name	= "rb400-gpio",
	.id	= -1,
	.dev	= {
		.platform_data = (void *)(RB400_GPIO_INVERT_OE |
					  RB400_GPIO11_RESET_FN),
	},
};

static struct platform_device ccs_gpio_device = {
	.name	= "rb400-gpio",
	.id	= -1,
	.dev	= {
		.platform_data = (void *)RB400_GPIO_BASE2,
	},
};

static struct i2c_gpio_platform_data rb2011_i2c_gpio_data = {
    .sda_pin = 11,
    .scl_pin = 21,
    .udelay = 5,
    .timeout = 1,
    .sda_is_open_drain = 0,
    .scl_is_open_drain = 0,
    .scl_is_output_only = 0,
};
static struct platform_device rb2011_i2c_gpio_device = {
	.name	= "i2c-gpio",
	.id	= -1,
	.dev	= {
		.platform_data = &rb2011_i2c_gpio_data,
	},
};

static struct i2c_gpio_platform_data rb953_i2c_gpio_data = {
    .sda_pin = 18,
    .scl_pin = 19,
    .udelay = 5,
    .timeout = 1,
    .sda_is_open_drain = 1,
    .scl_is_open_drain = 1,
    .scl_is_output_only = 0,
};
static struct platform_device rb953_i2c_gpio_device = {
	.name	= "i2c-gpio",
	.id	= -1,
	.dev	= {
		.platform_data = &rb953_i2c_gpio_data,
	},
};

static struct platform_device rb2011_gpio_i2c_sfp_device = {
	.name	= "rb400-gpio-i2c-sfp",
	.id	= -1,
};


static int rb411_eth_port_map[] = { 1, 0, 1, -1 };
static struct platform_device rb411_eth_device = {
	.name	= "ag7100",
	.id	= -1,
	.dev	= {
		.platform_data = rb411_eth_port_map,
	},
};

static int rb411g_eth_port_map[] = { 1, 0, 0, -1 };
static struct platform_device rb411g_eth_device = {
	.name	= "ag7100",
	.id	= -1,
	.dev	= {
		.platform_data = rb411g_eth_port_map,
	},
};

static int rb433_eth_port_map[] = { 2, 1, 4, -1, 0, 175, 1, 2, -1 };
static struct platform_device rb433_eth_device = {
	.name	= "ag7100",
	.id	= -1,
	.dev	= {
		.platform_data = rb433_eth_port_map,
	},
};

static int rb433gl_eth_port_map[] = { 1, 0, 8327, 4, 1, 2, -1 };
static struct platform_device rb433gl_eth_device = {
	.name	= "ag7100",
	.id	= -1,
	.dev	= {
		.platform_data = rb433gl_eth_port_map,
	},
};

static int rb953_eth_port_map[] = { 2, 0, 8327, 0, 1, 2, 5, -1, 1, -1, -1 };
static struct platform_device rb953_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = rb953_eth_port_map,
	},
};

static int rb433ul_eth_port_map[] = { 2, 1, 4, -1, 0, 8236, 0, 1, -1 };
static struct platform_device rb433ul_eth_device = {
	.name	= "ag7100",
	.id	= -1,
	.dev	= {
		.platform_data = rb433ul_eth_port_map,
	},
};

static int rb435g_eth_port_map[] = { 2, 1, 4, -1, 0, 8316, 0, 1, -1 };
static struct platform_device rb435g_eth_device = {
	.name	= "ag7100",
	.id	= -1,
	.dev	= {
		.platform_data = rb435g_eth_port_map,
	},
};

static int rb450_eth_port_map[] = { 2, 1, 4, -1, 0, 175, 3, 2, 1, 0, -1 };
static struct platform_device rb450_eth_device = {
	.name	= "ag7100",
	.id	= -1,
	.dev	= {
		.platform_data = rb450_eth_port_map,
	},
};

static int rb450g_eth_port_map[] = { 2, 1, 4, -1, 0, 8316, 0, 3, 2, 1, -1 };
static struct platform_device rb450g_eth_device = {
	.name	= "ag7100",
	.id	= -1,
	.dev	= {
		.platform_data = rb450g_eth_port_map,
	},
};

static int rb711_eth_port_map[] = { 1, 0, 4, -1 };
static struct platform_device rb711_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = rb711_eth_port_map,
	},
};

static int rb711r3_eth_port_map[] = { 1, 1, 7240, 0, -1 };
static struct platform_device rb711r3_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = rb711r3_eth_port_map,
	},
};

static int mAP_eth_port_map[] = { 1, 1, 7240, 0, 1, -1 };
static struct platform_device mAP_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = mAP_eth_port_map,
	},
};

static int rb750_eth_port_map[] = { 2, 0, 4, -1, 1, 7240, 3, 2, 1, 0, -1 };
static struct platform_device rb750_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = rb750_eth_port_map,
	},
};

static int rb493g_eth_port_map[] = { 3, 0, 8316, 4, -1,
				     1, 8316, 1, 2, 3, 0, -1,
				     0, 8316, 1, 3, 2, 0, -1 };
static struct platform_device rb493g_eth_device = {
	.name	= "ag7100",
	.id	= -1,
	.dev	= {
		.platform_data = rb493g_eth_port_map,
	},
};

static int rb750g_eth_port_map[] = { 1, 0, 8316, 0, 1, 2, 3, 4, -1 };
static struct platform_device rb750g_eth_device = {
	.name	= "ag7100",
	.id	= -1,
	.dev	= {
		.platform_data = rb750g_eth_port_map,
	},
};

static int rb711g_eth_port_map[] = { 1, 0, 0, -1 };
static struct platform_device rb711g_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = rb711g_eth_port_map,
	},
};

static int rb922g_eth_port_map[] = { 2, 0, 4, -1, 1, -1, -1 };
static struct platform_device rb922g_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = rb922g_eth_port_map,
	},
};

static int rb951HnD_eth_port_map[] = { 1, 1, 8227, 4, 3, 2, 1, 0, -1 };
static struct platform_device rb951HnD_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = rb951HnD_eth_port_map,
	},
};

static int sxt2d_eth_port_map[] = { 1, 1, 8227, 0, -1 };
static struct platform_device sxt2d_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = sxt2d_eth_port_map,
	},
};

static int rb2011S_eth_port_map[] = { 2,
				      0, 8327, 5, 0, 1, 2, 3, 4, -1,
				      1, 8227, 4, 3, 2, 1, 0, -1 };
static struct platform_device rb2011S_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = rb2011S_eth_port_map,
	},
};


static int rb2011L_eth_port_map[] = { 2,
				      0, 8327, 0, 1, 2, 3, 4, -1,
				      1, 8227, 4, 3, 2, 1, 0, -1 };
static struct platform_device rb2011L_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = rb2011L_eth_port_map,
	},
};

static int rb951G_eth_port_map[] = { 1,
				     0, 8327, 0, 1, 2, 3, 4, -1, };
static struct platform_device rb951G_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = rb951G_eth_port_map,
	},
};

static int rb750gl_eth_port_map[] = { 1, 0, 8327, 0, 1, 2, 3, 4, -1 };
static struct platform_device rb750gl_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = rb750gl_eth_port_map,
	},
};

static int groove52_eth_port_map[] = { 1, 0, 0, -1 };
static struct platform_device groove52_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = groove52_eth_port_map,
	},
};

static int rb912g_eth_port_map[] = { 1, 0, 0, -1 };
static struct platform_device rb912g_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = rb912g_eth_port_map,
	},
};

static int rb493_eth_port_map[] = { 2, 0, 1, -1,
				    1, 178, 0, 1, 2, 3, 4, 5, 6, 7, -1 };
static struct platform_device rb493_eth_device = {
	.name	= "ag7100",
	.id	= -1,
	.dev	= {
		.platform_data = rb493_eth_port_map,
	},
};

static int rb112_eth_port_map[] = { -1, 0, -1 };

static struct platform_device rb112_eth_device = {
	.name	= "admtek",
	.id	= -1,
	.dev	= {
		.platform_data = rb112_eth_port_map,
	},
};

static int rb153_eth_port_map[] = { -1, 0, 4, 3, 2, 1, -1 };

static struct platform_device rb153_eth_device = {
	.name	= "admtek",
	.id	= -1,
	.dev	= {
		.platform_data = rb153_eth_port_map,
	},
};

static int rb133_eth_port_map[] = { -1, 2, 1, 0, -1 };

static struct platform_device rb133_eth_device = {
	.name	= "admtek",
	.id	= -1,
	.dev	= {
		.platform_data = rb133_eth_port_map,
	},
};

static int rb133c_eth_port_map[] = { -1, 2, -1 };

static struct platform_device rb133c_eth_device = {
	.name	= "admtek",
	.id	= -1,
	.dev	= {
		.platform_data = rb133c_eth_port_map,
	},
};

static int rb192_eth_port_map[] = { 5, 0, 4, 3, 2, 1, 101, 102, 103, 104, -1 };

static struct platform_device rb192_eth_device = {
	.name	= "admtek",
	.id	= -1,
	.dev	= {
		.platform_data = rb192_eth_port_map,
	},
};

static int mr_eth_port_map[] = { -1, 4, 3, 2, 1, 0, -1 };

static struct platform_device mr_eth_device = {
	.name	= "admtek",
	.id	= -1,
	.dev	= {
		.platform_data = mr_eth_port_map,
	},
};

static struct platform_device cr_eth_device = {
	.name	= "cr-ether",
	.id	= -1,
};

static struct platform_device cr_wlan_device = {
	.name	= "cr-wlan",
	.id	= -1,
};

static struct platform_device ar9330_wlan_device = {
	.name	= "rb750-wlan",
	.id	= -1,
};

static struct platform_device rb500_cf_device = {
	.name	= "rb500-cf",
	.id	= -1,
};

static struct platform_device rb100_cf_device = {
	.name	= "rb100-cf",
	.id	= -1,
};

static u64 dmamask = ~(u32) 0;

static struct resource rb400_ohci_resources[] = {
	[0] = {
		.start		= 0x1c000000,
		.end		= 0x1cffffff,
		.flags		= IORESOURCE_MEM,
	},
	[1] = {
		.start		= 22,
		.end		= 22,
		.flags		= IORESOURCE_IRQ,
	},
};

static struct platform_device rb400_ohci_device = {
	.name		= "rb400-ohci",
	.id		= 0,
	.dev		= {
		.dma_mask		= &dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
	.num_resources	= ARRAY_SIZE(rb400_ohci_resources),
	.resource	= rb400_ohci_resources,
};

static struct resource rb400_ehci_resources[] = {
	[0] = {
		.start		= 0x1b000000,
		.end		= 0x1bffffff,
		.flags		= IORESOURCE_MEM,
	},
	[1] = {
		.start		= 3,
		.end		= 3,
		.flags		= IORESOURCE_IRQ,
	},
};

static struct platform_device rb400_ehci_device = {
	.name		= "rb400-ehci",
	.id		= 0,
	.dev		= {
		.dma_mask		= &dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
	.num_resources	= ARRAY_SIZE(rb400_ehci_resources),
	.resource	= rb400_ehci_resources,
};

static struct poeout_port rb951HnD_poeout_port = {
	.eth_port = 4,
	.gpo_on = 2,
	.gpi_status = 17,
};
static struct platform_device rb951HnD_poeout_device = {
	.name	= "poe-out-simple",
	.id	= -1,
	.dev	= {
		.platform_data = &rb951HnD_poeout_port,
	},
};

static struct poeout_port rb2011r5_poeout_port = {
	.eth_port = 10,
	.gpo_on = 2,
	.gpi_status = 13,
	.gpi_status_valid = 3,
};
static struct platform_device rb2011r5_poeout_device = {
	.name	= "poe-out-simple",
	.id	= -1,
	.dev	= {
		.platform_data = &rb2011r5_poeout_port,
	},
};

static struct poeout_port rb2011Lr5_poeout_port = {
	.eth_port = 9,
	.gpo_on = 2,
	.gpi_status = 13,
	.gpi_status_valid = 3,
};
static struct platform_device rb2011Lr5_poeout_device = {
	.name	= "poe-out-simple",
	.id	= -1,
	.dev	= {
		.platform_data = &rb2011Lr5_poeout_port,
	},
};

static struct poeout_port mAP_poeout_port = {
        .eth_port = 1,
        .gpo_on = 26,
        .gpi_status = 27,
};
static struct platform_device mAP_poeout_device = {
        .name   = "poe-out-simple",
        .id     = -1,
        .dev    = {
                .platform_data = &mAP_poeout_port,
        },
};

static struct platform_device *rb112_devices[] = {
	&rb100_uart_device,
	&rb100_nand_device,
	&rb112_eth_device,
	&flash_nor_device,
	&rb112_led_device,
};

static struct platform_device *rb153_devices[] = {
	&rb100_uart_device,
	&rb100_nand_device,
	&rb153_eth_device,
	&rb100_cf_device,
	&flash_nor_device,
	&rb100_led_device,
};

static struct platform_device *rb133_devices[] = {
	&rb100_uart_device,
	&rb100_nand_device,
	&rb133_eth_device,
	&flash_nor_device,
	&rb100_led_device,
};

static struct platform_device *rb133c_devices[] = {
	&rb100_uart_device,
	&rb100_nand_device,
	&rb133c_eth_device,
	&flash_nor_device,
	&rb100_led_device,
};

static struct platform_device *rb192_devices[] = {
	&rb100_uart_device,
	&rb100_nand_device,
	&rb192_eth_device,
	&flash_nor_device,
	&rb100_led_device,
};

static struct platform_device *mr_devices[] = {
	&rb100_uart_device,
	&mr_nand_device,
	&mr_eth_device,
	&flash_nor_device,
	&mr_led_device,
};

static struct platform_device *rb500_devices[] = {
	&rb500_nand_device,
	&korina_device,
	&rb500_cf_device,
	&flash_nor_device,
	&rb500_led_device,
};

static struct platform_device *rb500r5_devices[] = {
	&rb500r5_nand_device,
	&korina_device,
	&rb500_cf_device,
	&flash_nor_device,
	&rb500_led_device,
};

static struct platform_device *rb411_devices[] = {
	&rb400_spi_device,
	&rb411_eth_device,
	&rb400_led_device,
};

static struct platform_device *rb411ul_devices[] = {
	&rb400_ehci_device,
	&rb400_ohci_device,
	&rb400_gpio_device,
	&rb400l_spi_device,
	&rb750g_nand_device,
	&rb411_eth_device,
	&rb411l_led_device,
};

static struct platform_device *rb411l_devices[] = {
	&rb400_gpio_device,
	&rb400l_spi_device,
	&rb750g_nand_device,
	&rb411_eth_device,
	&rb411l_led_device,
};

static struct platform_device *rb411g_devices[] = {
	&rb400_ehci_device,
	&rb400_ohci_device,
	&rb400_gpio_device,
	&rb400l_spi_device,
	&rb750g_nand_device,
	&rb411g_eth_device,
	&rb411l_led_device,
};

static struct platform_device *rb411u_devices[] = {
	&rb400_ehci_device,
	&rb400_ohci_device,
	&rb400_spi_device,
	&rb411_eth_device,
	&rb400_led_device,
};

static struct platform_device *rb433_devices[] = {
	&rb433_spi_device,
	&rb433_eth_device,
	&rb400_led_device,
};

static struct platform_device *rb433u_devices[] = {
	&rb400_ehci_device,
	&rb400_ohci_device,
	&rb433_spi_device,
	&rb433_eth_device,
	&rb400_led_device,
};

static struct platform_device *rb433gl_devices[] = {
	&rb400_ehci_device,
	&rb400_ohci_device,
	&rb400_gpio_device,
	&rb400l_spi_device,
	&rb433gl_eth_device,
	&rb750g_nand_device,
	&rb411l_led_device,
};

static struct platform_device *rb433ul_devices[] = {
	&rb400_ehci_device,
	&rb400_ohci_device,
	&rb400_gpio_device,
	&rb400l_spi_device,
	&rb433ul_eth_device,
	&rb750g_nand_device,
	&rb411l_led_device,
};

static struct platform_device *rb433l_devices[] = {
	&rb400_gpio_device,
	&rb400l_spi_device,
	&rb433ul_eth_device,
	&rb750g_nand_device,
	&rb411l_led_device,
};

static struct platform_device *rb435g_devices[] = {
	&rb400_ehci_device,
	&rb400_ohci_device,
	&rb433_spi_device,
	&rb435g_eth_device,
	&rb400_led_device,
};

static struct platform_device *rb450_devices[] = {
	&rb400_spi_device,
	&rb450_eth_device,
	&rb400_led_device,
};

static struct platform_device *rb450g_devices[] = {
	&rb433_spi_device,
	&rb450g_eth_device,
	&rb450g_led_device,
};

static struct platform_device *rb493_devices[] = {
	&rb400_spi_device,
	&rb493_eth_device,
	&rb400_led_device,
};

static struct platform_device *rb493g_devices[] = {
	&rb400_ehci_device,
	&rb400_ohci_device,
	&rb433_spi_device,
	&rb493g_eth_device,
	&rb493g_led_device,
};

static struct platform_device *rb750g_devices[] = {
	&rb750_spi_device,
	&rb750g_nand_device,
	&rb750g_eth_device,
	&rb750g_led_device,
};

static struct platform_device *cr_devices[] = {
	&cr_nand_device,
	&cr_eth_device,
	&cr_wlan_device,
	&flash_spi_device,
	&cr_led_device,
};

static struct resource rb700_ohci_resources[] = {
	[0] = {
		.start		= 0x1b000000,
		.end		= 0x1bffffff,
		.flags		= IORESOURCE_MEM,
	},
	[1] = {
		.start		= 3,
		.end		= 3,
		.flags		= IORESOURCE_IRQ,
	},
};

static struct platform_device rb700_ohci_device = {
	.name		= "rb400-ohci",
	.id		= 0,
	.dev		= {
		.dma_mask		= &dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
	.num_resources	= ARRAY_SIZE(rb700_ohci_resources),
	.resource	= rb700_ohci_resources,
};

static struct resource rb700_ehci_resources[] = {
	[0] = {
		.start		= 0x1b000000,
		.end		= 0x1b3fffff,
		.flags		= IORESOURCE_MEM,
	},
	[1] = {
		.start		= 3,
		.end		= 3,
		.flags		= IORESOURCE_IRQ,
	},
};

static struct resource rb900_ehci_resources[] = {
	[0] = {
		.start		= 0x1b400000,
		.end		= 0x1b7fffff,
		.flags		= IORESOURCE_MEM,
	},
	[1] = {
		.start		= 3,
		.end		= 3,
		.flags		= IORESOURCE_IRQ,
	},
};

static struct platform_device rb700_ehci_device = {
	.name		= "rb400-ehci",
	.id		= 0,
	.dev		= {
		.dma_mask		= &dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
	.num_resources	= ARRAY_SIZE(rb700_ehci_resources),
	.resource	= rb700_ehci_resources,
};

static struct platform_device rb900_ehci_device = {
	.name		= "rb900-ehci",
	.id		= 1,
	.dev		= {
		.dma_mask		= &dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
	.num_resources	= ARRAY_SIZE(rb900_ehci_resources),
	.resource	= rb900_ehci_resources,
};

static struct platform_device *rb711_devices[] = {
	&rb700_ehci_device,
	&rb700_ohci_device,
	&rb711_spi_device,
	&rb700_nand_device,
	&rb711_led_device,
	&rb711_eth_device,
};

static struct platform_device *rb711r3_devices[] = {
	&rb700_ehci_device,
	&rb700_ohci_device,
	&rb711_spi_device,
	&rb700_nand_device,
	&rb711r3_led_device,
	&rb711r3_eth_device,
};

static struct platform_device *rb_omni_5fe_devices[] = {
	&rb700_ehci_device,
	&rb700_ohci_device,
	&omni_spi_device,
	&rb700_nolatch_nand_device,
	&sxt5d_led_device,
	&rb750_eth_device,
};

static struct platform_device *rb751g_devices[] = {
	&rb700_ehci_device,
	&rb700_ohci_device,
	&rb711_spi_device,
	&rb750gl_eth_device,
	&rb700_nolatch_nand_device,
	&rb751g_led_device,
};

static struct platform_device *rb711g_devices[] = {
	&rb700_ehci_device,
	&rb700_ohci_device,
	&rb711_spi_device,
	&rb700_nand_device,
	&rb711g_eth_device,
	&rb711g_led_device,
};

static struct platform_device *rb711gt_devices[] = {
	&rb700_ehci_device,
	&rb700_ohci_device,
	&rb711_spi_device,
	&rb700_nolatch_nand_device,
	&rb711g_eth_device,
	&rb711gt_led_device,
};

static struct platform_device *rb_sxtg_devices[] = {
	&rb700_ehci_device,
	&rb700_ohci_device,
	&rb711_spi_device,
	&rb700_nolatch_nand_device,
	&rb711g_eth_device,
	&sxtg_led_device,
};

static struct platform_device *rb912g_devices[] = {
	&rb700_ehci_device,
	&ar9342_spi_device,
	&rb912g_eth_device,
	&ar9342_nand_device,
	&rb912g_led_device,
	&ar9330_wlan_device,
};

static struct hwopt_device rb750gl_devices[] = {
	{ &rb700_ehci_device, HW_OPT_HAS_USB, 0 },
	{ &rb700_ohci_device, HW_OPT_HAS_USB, 0 },
	{ &rb711_spi_device, 0, 0 },
	{ &rb700_nolatch_nand_device, 0, 0 },
	{ &rb750gl_led_device, 0, 0 },
	{ &rb750gl_eth_device, 0, 0 },
};


static struct hwopt_device rb951hnd_devices[] = {
	{ &rb700_ehci_device, HW_OPT_HAS_USB, 0 },
	{ &rb2011L_spi_device, 0, 0 },
	{ &ar9340_gpio_device, 0, 0 },
	{ &rb951HnD_poeout_device, HW_OPT_HAS_POE_OUT, 0 },
	{ &rb951HnD_eth_device, 0, 0 },
	{ &rb951HnD_led_device, 0, 0 },
	{ &ar9344_nand_device, 0, 0 },
	{ &ar9330_wlan_device, 0, 0 },
};

static struct platform_device *sxt2d_devices[] = {
	&rb700_ehci_device,
	&rb2011L_spi_device,
	&sxt2d_led_device,
	&sxt2d_eth_device,
	&ar9344_nand_device,
	&ar9330_wlan_device,
};

static struct platform_device *rb911L_devices[] = {
	&rb700_ehci_device,
	&waspL_spi_device,
	&rb911L_led_device,
	&sxt2d_eth_device,
	&ar9330_wlan_device,
};

static struct hwopt_device sxt5n_devices[] = {
	{ &rb700_ehci_device, HW_OPT_HAS_USB, 0 },
	{ &rb2011_spi_device, HW_OPT_HAS_TS_FOR_ADC, HW_OPT_NO_NAND },
	{ &rb2011L_spi_device, 0, HW_OPT_HAS_TS_FOR_ADC | HW_OPT_NO_NAND },
	{ &waspL_spi_device, HW_OPT_NO_NAND, 0 },
	{ &sxt5n_led_device, 0, 0 },
	{ &sxt2d_eth_device, 0, 0 },
	{ &ar9344_nand_device, 0, HW_OPT_NO_NAND },
	{ &ar9330_wlan_device, 0, 0 },
};

static struct hwopt_device sxtg5p_devices[] = {
	{ &rb700_ehci_device, 0, 0 },
	{ &rb2011_spi_device, 0, HW_OPT_NO_NAND },
	{ &scorpionL_spi_device, HW_OPT_NO_NAND, 0 },
	{ &sxt5n_led_device, 0, 0 },
	{ &rb711g_eth_device, 0, 0 },
	{ &ar9344_nand_device, 0, HW_OPT_NO_NAND },
	{ &ar9330_wlan_device, 0, 0 },
};

static struct platform_device *rb2011G_devices[] = {
	&rb700_ehci_device,
	&rb2011_spi_device,
	&rb2011S_eth_device,
	&ar9344_nand_device,
	&ar9330_wlan_device,
	&rb2011_led_device,
};

static struct platform_device *rb2011US_devices[] = {
	&rb700_ehci_device,
	&rb2011_spi_device,
	&rb2011S_eth_device,
	&ar9344_nand_device,
	&rb2011_led_device,
};

static struct platform_device *rb2011L_devices[] = {
	&rb2011L_spi_device,
	&rb2011L_led_device,
	&rb2011L_eth_device,
	&ar9344_nand_device,
};

static struct platform_device *rb2011LS_devices[] = {
	&rb2011L_spi_device,
	&rb2011S_eth_device,
	&ar9344_nand_device,
	&rb2011_led_device,
};

static struct hwopt_device rb2011r5_devices[] = {
	{ &rb700_ehci_device, HW_OPT_HAS_USB, 0 },
	{ &rb2011_spi_device, HW_OPT_HAS_LCD, 0 },
	{ &rb2011L_spi_device, 0, HW_OPT_HAS_LCD },
	{ &rb2011r5_poeout_device, HW_OPT_HAS_POE_OUT | HW_OPT_HAS_SFP, 0 },
	{ &rb2011Lr5_poeout_device, HW_OPT_HAS_POE_OUT, HW_OPT_HAS_SFP },
	{ &rb2011S_eth_device, HW_OPT_HAS_SFP, 0 },
	{ &rb2011L_eth_device, 0, HW_OPT_HAS_SFP },
	{ &ar9344_nand_device, 0, 0 },
	{ &ar9330_wlan_device, HW_OPT_HAS_WIFI, 0 },
	{ &rb2011r5_led_device, HW_OPT_HAS_SFP, 0 },
	{ &rb2011r5_user_led_device, 0, HW_OPT_HAS_SFP },
	{ &ar9344_gpio_device, 0, 0 },
	{ &rb2011_i2c_gpio_device, HW_OPT_HAS_SFP, 0 },
	{ &rb2011_gpio_i2c_sfp_device, HW_OPT_HAS_SFP, 0 },
};

static struct platform_device *rb951G_devices[] = {
	&rb700_ehci_device,
	&rb2011_spi_device,
	&rb951G_eth_device,
	&ar9344_nand_device,
	&ar9330_wlan_device,
	&rb951g_led_device,
};

static struct platform_device *rb953gs_devices[] = {
	&rb700_ehci_device,
	&rb900_ehci_device,
	&rb2011L_spi_device,
	&ar9344_nand_device,
	&ar9330_wlan_device,
	&rb953_eth_device,
	&ar9340_gpio_device,
	&rb953_i2c_gpio_device,
	&rb2011_gpio_i2c_sfp_device,
	&rb953_led_device,
};

static struct spi_board_info *rb941_spi_info[] = {
	&rb900_spi_misc,
	&slow_large_spi_nor,
	NULL
};
static struct platform_device rb941_spi_device = {
	.name	= "rb900-spi",
	.id	= -1,
	.dev	= {
		.platform_data = rb941_spi_info,
	},
};

static struct rb400_ssr_info hb_ssr_data = {
	.strobe = 11,
	.bytes = 1,
	.val = 0xff
};
static struct spi_board_info hb_ssr_spi_misc = {
	.modalias = "rb400-spi-misc",
	.max_speed_hz = 33333334,
	.bus_num = 0,
	.chip_select = 0,
	.mode = SPI_MODE_3,
	.platform_data = &hb_ssr_data,
};

static struct spi_board_info *hb_ssr_spi_info[] = {
	&hb_ssr_spi_misc,
	&slow_large_spi_nor,
	NULL
};
static struct platform_device hb_ssr_spi_device = {
	.name	= "rb900-spi",
	.id	= -1,
	.dev	= {
		.platform_data = hb_ssr_spi_info,
	},
};

static struct rb400_leds rb750r2_led_map = {
	.user_led	= PLD(4, PLED_TYPE_GPIO),
	.led1		= PLDI(0, PLED_TYPE_SSR_RB400),
	.led2		= PLDI(1, PLED_TYPE_SSR_RB400),
	.led3		= PLDI(2, PLED_TYPE_SSR_RB400),
	.led4		= PLDI(3, PLED_TYPE_SSR_RB400),
	.led5		= PLDI(4, PLED_TYPE_SSR_RB400),
	.usb_power_off	= PLDI(5, PLED_TYPE_SSR_RB400),
	.button		= PLDI(16, PLED_TYPE_GPIO),
};
static struct platform_device rb750r2_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb750r2_led_map,
	},
};

static struct rb400_leds hex_led_map = {
	.user_led	= PLD(4, PLED_TYPE_GPIO),
	.led1		= PLDI(0, PLED_TYPE_SSR_RB400),
	.led2		= PLDI(1, PLED_TYPE_SSR_RB400),
	.led3		= PLDI(2, PLED_TYPE_SSR_RB400),
	.led4		= PLDI(3, PLED_TYPE_SSR_RB400),
	.led5		= PLDI(4, PLED_TYPE_SSR_RB400),
	.usb_power_off	= PLDI(5, PLED_TYPE_SSR_RB400),
};
static struct platform_device hex_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &hex_led_map,
	},
};

static struct spi_board_info *rb750r2_spi_info[] = {
	&hb_ssr_spi_misc,
	&slow_large_spi_nor,
	&rb700_spi_attiny,
	NULL
};
static struct platform_device rb750r2_spi_device = {
	.name	= "rb900-spi",
	.id	= -1,
	.dev	= {
		.platform_data = rb750r2_spi_info,
	},
};

static struct hwopt_device rb750r2_devices[] = {
	{ &rb700_ehci_device, HW_OPT_HAS_USB, 0 },
	{ &ar9340_gpio_device, 0, 0 },
	{ &rb750r2_spi_device, HW_OPT_HAS_ATTINY, 0 },
	{ &hb_ssr_spi_device, 0, HW_OPT_HAS_ATTINY },
	{ &rb750r2_led_device, HW_OPT_HAS_WIFI, 0 },
	{ &hex_led_device, 0, HW_OPT_HAS_WIFI },
	{ &rb951HnD_eth_device, 0, 0 },
	{ &ar9330_wlan_device, HW_OPT_HAS_WIFI, 0 },
};

static int rbHAP_eth_port_map[] = { 1, 1, 8227, 3, 2, 1, 0, -1 };
static struct platform_device rbHAP_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = rbHAP_eth_port_map,
	},
};

static struct rb400_leds rbHAP_led_map = {
	.user_led	= PLDI(14, PLED_TYPE_GPIO),
};
static struct platform_device rbHAP_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rbHAP_led_map,
	},
};

static struct platform_device *rb941HL_devices[] = {
	&rbHAP_led_device,
	&rb941_spi_device,
	&ar9330_wlan_device,
	&ar9340_gpio_device,
	&rbHAP_eth_device
};



static struct platform_device *sxtg5c_devices[] = {
	&rb900_ehci_device,
	&rb2011_spi_device,
	&ar9344_nand_device,
	&rb711g_eth_device,
	&sxtg5c_led_device
};

static struct hwopt_device rb922gs_devices[] = {
	{ &rb700_ehci_device, HW_OPT_HAS_USB, 0 },
	{ &rb900_ehci_device, HW_OPT_HAS_USB, 0 },
	{ &rb2011_spi_device, 0, 0 },
	{ &ar9344_nand_device, 0, 0 },
	{ &rb922g_eth_device, HW_OPT_HAS_SFP, 0 },
	{ &rb711_eth_device, 0, HW_OPT_HAS_SFP },
	{ &rb922g_led_device, 0, 0 },
	{ &ar9340_gpio_device, 0, 0 },
	{ &rb953_i2c_gpio_device, 0, 0 },
	{ &rb2011_gpio_i2c_sfp_device, 0, 0 },
};

static struct mdio_gpio_platform_data rb750gv2_mdio_platform_data = {
	.mdc = 17,
	.mdio = 19,
	.phy_mask = 0,
};

struct platform_device rb750gv2_mdio_device = {
	.name   = "mdio-gpio",
	.id     = 1,
	.dev    = {
		.platform_data = &rb750gv2_mdio_platform_data,
	},
};

static struct rb400_leds rb750gv2_led_map = {
	.user_led	= PLD(14, PLED_TYPE_GPIO),
	.usb_power_off	= PLDI(13, PLED_TYPE_GPIO),
	.power_led	= PLED(16, PLED_TYPE_GPIO, PLED_CFG_ON | PLED_CFG_INV),
	.button		= PLDI(1, PLED_TYPE_GPIO),
};
static struct platform_device rb750gv2_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb750gv2_led_map,
	},
};

static int rb750gv2_eth_port_map[] = {
    1, 1, BIT(29) | BIT(30) | 8327, // use bitbanging (bit 30), switch specific config follows (bit 29)
    BIT(8) | BIT(17), // ath8327 specific_config: cpu port is sgmii (bit 8) | sgmii in phy mode (bit 17)
    0, 1, 2, 3, 4, -1 };
static struct platform_device rb750gv2_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = rb750gv2_eth_port_map,
	},
};
static struct platform_device *rb750Gv2_devices[] = {
	&scorpionL_spi_device,
	&ar9340_gpio_device,
	&rb750gv2_eth_device,
	&rb750gv2_mdio_device,
	&rb750gv2_led_device,
	&rb700_ehci_device,
};



static int crs125g_eth_port_map[] = {
    1, 0, 8513,
    1, 2, 3, 4, 5, 6, 7, 8,
    9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24,
    26 | BIT(17),
    -1
};
static struct platform_device crs125g_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = crs125g_eth_port_map,
	},
};

static struct hwopt_device crs125g_devices[] = {
	{ &rb700_ehci_device, HW_OPT_HAS_USB, 0 },
	{ &rb2011_spi_device, HW_OPT_HAS_LCD, 0 },
	{ &rb2011L_spi_device, 0, HW_OPT_HAS_LCD },
	{ &ar9344_nand_device, 0, 0 },
	{ &ar9330_wlan_device, HW_OPT_HAS_WIFI, 0 },
	{ &crs125g_eth_device, 0, 0 },
	{ &crs125_wifi_led_device, HW_OPT_HAS_WIFI, 0 },
	{ &crs125_led_device, 0, HW_OPT_HAS_WIFI },
};

static int crs109_eth_port_map[] = {
    1, 0, 8513,
    1, 2, 3, 4, 5, 6, 7, 8,
    26 | BIT(17),
    -1
};
static struct platform_device crs109_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = crs109_eth_port_map,
	},
};

static int crs110_eth_port_map[] = {
    1, 0, 8513,
    1, 2, 3, 4, 5, 6, 7, 8,
    26 | BIT(17),
    27 | BIT(17),
    -1
};
static struct platform_device crs110_eth_device = {
	.name	= "ag7240",
	.id	= -1,
	.dev	= {
		.platform_data = crs110_eth_port_map,
	},
};

static struct hwopt_device crs109_devices[] = {
	{ &rb700_ehci_device, HW_OPT_HAS_USB, 0 },
	{ &rb2011_spi_device, HW_OPT_HAS_LCD, 0 },
	{ &rb2011L_spi_device, 0, HW_OPT_HAS_LCD },
	{ &ar9344_nand_device, 0, 0 },
	{ &ar9330_wlan_device, HW_OPT_HAS_WIFI, 0 },
	{ &crs109_eth_device, HW_OPT_HAS_USB, 0 },
	{ &crs110_eth_device, 0, HW_OPT_HAS_USB },
	{ &crs125_wifi_led_device, HW_OPT_HAS_WIFI, 0 },
	{ &crs125_led_device, 0, HW_OPT_HAS_WIFI },
};

static struct rb400_leds groove52_led_map = {
	.led1		= PLD(0, PLED_TYPE_WIFI),
	.led2		= PLD(0, PLED_TYPE_SHARED_RB900),
	.led3		= PLD(1, PLED_TYPE_SHARED_RB900),
	.led4		= PLD(2, PLED_TYPE_SHARED_RB900),
	.button		= PLDI(15, PLED_TYPE_SHARED_RB900),
	.pin_hole	= PLDI(14, PLED_TYPE_SHARED_RB900),
};
static struct platform_device groove52_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &groove52_led_map,
	},
};

static struct platform_device *groove52_devices[] = {
	&rb750_spi_device,
	&groove52_eth_device,
	&ar9342_nand_device,
	&ar9330_wlan_device,
	&groove52_led_device,
};

static struct platform_device ar9330_uart_device = {
	.name	= "ar9330-uart",
	.id	= -1,
};
static struct platform_device ar9330_nand_device = {
	.name	= "ar9330-nand",
	.id	= -1,
};
static struct rb400_leds rb951_led_map = {
	.user_led	= PLD(0, PLED_TYPE_GPIO),
	.usb_power_off	= PLD(9, PLED_TYPE_GPIO),
	.button		= PLDI(6, PLED_TYPE_GPIO),
	.pin_hole	= PLDI(7, PLED_TYPE_GPIO),
};
static struct platform_device rb951_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &rb951_led_map,
	},
};

static struct rb400_leds mAP_led_map = {
        .user_led       = PLDI(21, PLED_TYPE_GPIO),
        .led1           = PLDI(18, PLED_TYPE_GPIO),
        .led2           = PLDI(20, PLED_TYPE_GPIO),
        .led3           = PLDI(19, PLED_TYPE_GPIO),
        .led4           = PLDI(22, PLED_TYPE_GPIO),
        .led5           = PLDI(23, PLED_TYPE_GPIO),
	.power_led	= PLD(28, PLED_TYPE_GPIO),
        .button         = PLDI(6, PLED_TYPE_GPIO),
        .pin_hole       = PLDI(7, PLED_TYPE_GPIO),
};
static struct platform_device mAP_led_device = {
        .name   = "rb400-led",
        .id     = -1,
        .dev    = {
                .platform_data = &mAP_led_map,
        },
};

static struct platform_device *rb951_devices[] = {
	&rb700_ehci_device,
	&rb750_spi_device,
	&ar9330_nand_device,
	&ar9330_uart_device,
	&rb750_eth_device,
	&rb951_led_device,
	&ar9330_wlan_device,
};

static struct rb400_leds cm2n_led_map = {
	.led1		= PLDI(14, PLED_TYPE_GPIO),
	.led2		= PLDI(15, PLED_TYPE_GPIO),
	.led3		= PLDI(16, PLED_TYPE_GPIO),
	.led4		= PLDI(17, PLED_TYPE_GPIO),
	.button		= PLDI(6, PLED_TYPE_GPIO),
	.pin_hole	= PLDI(7, PLED_TYPE_GPIO),
	.all_leds	= PLED(28, PLED_TYPE_GPIO, PLED_CFG_ON),
};
static struct platform_device cm2n_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = &cm2n_led_map,
	},
};

static struct platform_device *cm2n_devices[] = {
	&rb900l_spi_device,
	&ar9330_uart_device,
	&cm2n_led_device,
	&rb711r3_eth_device,
	&ar9330_wlan_device,
};

static struct platform_device *mAP_devices[] = {
	&rb400_ehci_device,
	&rb400_ohci_device,
	&rb900l_spi_device,
	&ar9330_uart_device,
	&rb400_gpio_device,
	&mAP_poeout_device,
	&mAP_eth_device,
	&ar9330_wlan_device,
	&mAP_led_device,
};


static struct platform_device *rb_sxt5d_devices[] = {
	&rb700_ehci_device,
	&rb700_ohci_device,
	&rb711_spi_device,
	&rb700_nolatch_nand_device,
	&sxt5d_led_device,
	&rb711r3_eth_device
};

static struct platform_device *rb_groove_devices[] = {
	&rb700_ehci_device,
	&rb700_ohci_device,
	&rb711_spi_device,
	&rb700_nolatch_nand_device,
	&groove_led_device,
	&rb711r3_eth_device
};

static struct platform_device *rb750_devices[] = {
	&rb700_ehci_device,
	&rb700_ohci_device,
	&omni_spi_device,
	&rb700_tiny_nand_device,
	&rb700_led_device,
	&rb750_eth_device
};

static struct platform_device *rb751_devices[] = {
	&rb700_ehci_device,
	&rb700_ohci_device,
	&rb711_spi_device,
	&rb700_tiny_nand_device,
	&rb751_led_device,
	&rb750_eth_device
};

static int ccs226_eth_port_map[] = {
    8519,
    1, 2, 3, 4, 5, 6, 7, 8,
    9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24,
    26 | BIT(16) | BIT(17),
    25 | BIT(16),
    -1
};
static struct platform_device ccs226_eth_device = {
	.name	= "music",
	.id	= -1,
	.dev	= {
		.platform_data = ccs226_eth_port_map,
	},
};

static int crs210_eth_port_map[] = {
    8513,
    1, 2, 3, 4, 5, 6, 7, 8,
    26 | BIT(16) | BIT(17),
    25 | BIT(16),
    -1
};
static struct platform_device crs210_eth_device = {
	.name	= "music",
	.id	= -1,
	.dev	= {
		.platform_data = crs210_eth_port_map,
	},
};
static struct spi_board_info *music_large_spi_info[] = {
	&rb400_spi_misc,
	&large_spi_nor,
	&music_spi_nand,
	&music_spi_lcd,
	&music_spi_ts,
	NULL
};
static struct platform_device music_large_spi_device = {
	.name	= "rb400-spi",
	.id	= -1,
	.dev	= {
		.platform_data = music_large_spi_info,
	},
};
static struct spi_board_info *crs212_large_spi_info[] = {
	&rb400_spi_misc,
	&large_spi_nor,
	&music_spi_nand,
	&music_spi_lcd,
	&music_spi_ts,
	&crs212_spi_ssr,
	NULL
};
static struct platform_device crs212_large_spi_device = {
	.name	= "rb400-spi",
	.id	= -1,
	.dev	= {
		.platform_data = crs212_large_spi_info,
	},
};
static struct hwopt_device ccs226_devices[] = {
	{ &music_large_spi_device, HW_OPT_NO_NAND, 0 },
	{ &music_spi_device, 0, HW_OPT_NO_NAND },
	{ &ccs226_eth_device, 0, 0 },
	{ &crs226_led_device, 0, 0 },
};

static struct hwopt_device crs210_devices[] = {
	{ &music_large_spi_device, HW_OPT_NO_NAND, 0 },
	{ &music_spi_device, 0, HW_OPT_NO_NAND },
	{ &crs210_eth_device, 0, 0 },
	{ &crs226_led_device, 0, 0 },
};

static int crs212_eth_port_map[] = {
    8719,
    5 | BIT(17),
    1 | BIT(17),
    13 | BIT(17),
    9 | BIT(17),
    21 | BIT(17),
    17 | BIT(17),
    27 | BIT(17),
    26 | BIT(17),
    29 | BIT(17),
    28 | BIT(17),
    25 | BIT(16),
    24,
    -1
};
static struct platform_device crs212_eth_device = {
	.name	= "music",
	.id	= -1,
	.dev	= {
		.platform_data = crs212_eth_port_map,
	},
};
static struct hwopt_device crs212_devices[] = {
	{ &crs212_large_spi_device, HW_OPT_NO_NAND, 0 },
	{ &crs212_spi_device, 0, HW_OPT_NO_NAND },
	{ &crs212_eth_device, 0, 0 },
	{ &crs226_led_device, 0, 0 },
};

static int ccs112_eth_port_map[] = {
    8511,
    1, 2, 3, 4, 5, 6, 7, 8,
    27 | BIT(17),
    26 | BIT(17),
    29 | BIT(17),
    28 | BIT(17),
    -1
};
static struct platform_device ccs112_eth_device = {
	.name	= "music",
	.id	= -1,
	.dev	= {
		.platform_data = ccs112_eth_port_map,
	},
};

static struct rb400_ssr_info ccs112_ssr_data = {
	.bytes = 1,
	.val = 0xff
};
static struct rb400_spi_ctrl ccs112_ssr_ctrl = {
	.gpio_as_cs = 1,
	.gpio_as_strobe = 1,
};
static struct spi_board_info ccs_spi_ssr = {
	.modalias = "ccs-spi-ssr",
	.max_speed_hz = 20 * 1000 * 1000,
	.bus_num = 0,
	.chip_select = 2,
	.mode = SPI_MODE_3,
	.platform_data = &ccs112_ssr_data,
	.controller_data = &ccs112_ssr_ctrl,
};
static struct spi_board_info ccs112_spi_lcd = {
	.modalias = "rb2011-spi-lcd",
	.max_speed_hz = 15 * 1000 * 1000,
	.bus_num = 0,
	.chip_select = 8,
	.mode = SPI_MODE_0,
};
static struct spi_board_info ccs112_spi_ts = {
	.modalias = "rb2011-spi-ts",
	.max_speed_hz = 2500 * 1000,
	.bus_num = 0,
	.chip_select = 9,
	.mode = SPI_MODE_0,
	.platform_data = (void *) 0,
};

static struct spi_board_info ccs112_spi_poe = {
	.modalias = "spi-poe-pse8",
	.max_speed_hz = 2 * 1000 * 1000,
	.bus_num = 0,
	.chip_select = 11,
	.mode = SPI_MODE_0,
};

static struct spi_board_info *ccs112_spi_info[] = {
	&rb400_spi_misc,
	&ccs_spi_ssr,
	&large_spi_nor,
	&ccs112_spi_lcd,
	&ccs112_spi_ts,
	&ccs112_spi_poe,
	NULL
};
static struct platform_device ccs112_spi_device = {
	.name	= "rb400-spi",
	.id	= -1,
	.dev	= {
		.platform_data = ccs112_spi_info,
	},
};

static struct platform_device *ccs112_devices[] = {
	&ccs_gpio_device,
	&ccs112_spi_device,
	&ccs112_eth_device,
	&ccs112_led_device,
};


static struct platform_driver fake_serial8250_driver = {
	.driver = {
		.name = "serial8250",
	},
};

int rb_platform_init(void)
{
	switch (mips_machgroup) {
	case MACH_GROUP_MT_RB100:
		platform_driver_register(&fake_serial8250_driver);

		switch (mips_machtype) {
		case MACH_MT_RB100:
			return platform_add_devices(
			    rb112_devices, ARRAY_SIZE(rb112_devices));
		case MACH_MT_RB150:
			return platform_add_devices(
			    rb153_devices, ARRAY_SIZE(rb153_devices));
		case MACH_MT_RB133:
			return platform_add_devices(
			    rb133_devices, ARRAY_SIZE(rb133_devices));
		case MACH_MT_RB133C:
			return platform_add_devices(
			    rb133c_devices, ARRAY_SIZE(rb133c_devices));
		case MACH_MT_RB192:
			return platform_add_devices(
			    rb192_devices, ARRAY_SIZE(rb192_devices));
		case MACH_MT_MR:
			return platform_add_devices(
			    mr_devices, ARRAY_SIZE(mr_devices));
		}
		break;
	case MACH_GROUP_MT_RB500:
		switch (mips_machtype) {
		case MACH_MT_RB500:
			return platform_add_devices(
			    rb500_devices, ARRAY_SIZE(rb500_devices));
		case MACH_MT_RB500R5:
			return platform_add_devices(
			    rb500r5_devices, ARRAY_SIZE(rb500r5_devices));
		}
		break;
	case MACH_GROUP_MT_RB400:
		switch (mips_machtype) {
		case MACH_MT_RB411:
			return platform_add_devices(
				rb411_devices,  ARRAY_SIZE(rb411_devices));
		case MACH_MT_RB411L:
			return platform_add_devices(
				rb411l_devices,  ARRAY_SIZE(rb411l_devices));
		case MACH_MT_RB411UL:
			return platform_add_devices(
				rb411ul_devices,  ARRAY_SIZE(rb411ul_devices));
		case MACH_MT_RB411G:
			return platform_add_devices(
				rb411g_devices,  ARRAY_SIZE(rb411g_devices));
		case MACH_MT_RB411U:
			return platform_add_devices(
				rb411u_devices,  ARRAY_SIZE(rb411u_devices));
		case MACH_MT_RB433:
			return platform_add_devices(
				rb433_devices,  ARRAY_SIZE(rb433_devices));
		case MACH_MT_RB433U:
			return platform_add_devices(
				rb433u_devices,  ARRAY_SIZE(rb433u_devices));
		case MACH_MT_RB433GL:
			return platform_add_devices(
				rb433gl_devices,  ARRAY_SIZE(rb433gl_devices));
		case MACH_MT_RB433UL:
			return platform_add_devices(
				rb433ul_devices,  ARRAY_SIZE(rb433ul_devices));
		case MACH_MT_RB433L:
			return platform_add_devices(
				rb433l_devices,  ARRAY_SIZE(rb433l_devices));
		case MACH_MT_RB435G:
			return platform_add_devices(
				rb435g_devices,  ARRAY_SIZE(rb435g_devices));
		case MACH_MT_RB450:
			return platform_add_devices(
				rb450_devices,  ARRAY_SIZE(rb450_devices));
		case MACH_MT_RB450G:
			return platform_add_devices(
				rb450g_devices,  ARRAY_SIZE(rb450g_devices));
		case MACH_MT_RB493:
			return platform_add_devices(
				rb493_devices,  ARRAY_SIZE(rb493_devices));
		case MACH_MT_RB493G:
			return platform_add_devices(
				rb493g_devices,  ARRAY_SIZE(rb493g_devices));
		case MACH_MT_RB750G:
			return platform_add_devices(
				rb750g_devices,  ARRAY_SIZE(rb750g_devices));
		}
		break;
	case MACH_GROUP_MT_RB700:
		switch (mips_machtype) {
		case MACH_MT_RB_OMNI:
		case MACH_MT_RB711:
			return platform_add_devices(
				rb711_devices,  ARRAY_SIZE(rb711_devices));
		case MACH_MT_RB711R3:
			return platform_add_devices(
				rb711r3_devices,  ARRAY_SIZE(rb711r3_devices));
		case MACH_MT_RB_GROOVE_5S:
		case MACH_MT_RB_SXT5D:
			return platform_add_devices(
			    rb_sxt5d_devices,  ARRAY_SIZE(rb_sxt5d_devices));
		case MACH_MT_RB751G:
			return platform_add_devices(
			    rb751g_devices,  ARRAY_SIZE(rb751g_devices));
		case MACH_MT_RB711G:
			return platform_add_devices(
			    rb711g_devices,  ARRAY_SIZE(rb711g_devices));
		case MACH_MT_RB711GT:
			return platform_add_devices(
			    rb711gt_devices,  ARRAY_SIZE(rb711gt_devices));
		case MACH_MT_RB_SXTG:
			return platform_add_devices(
			    rb_sxtg_devices,  ARRAY_SIZE(rb_sxtg_devices));
		case MACH_MT_RB912G:
			return platform_add_devices(
			    rb912g_devices, ARRAY_SIZE(rb912g_devices));
		case MACH_MT_RB750GL:
			return platform_add_hwopt_devices(
			    rb750gl_devices,  ARRAY_SIZE(rb750gl_devices));
		case MACH_MT_RB911L:
			return platform_add_devices(
			    rb911L_devices,  ARRAY_SIZE(rb911L_devices));
		case MACH_MT_SXT2D:
			return platform_add_devices(
			    sxt2d_devices,  ARRAY_SIZE(sxt2d_devices));
		case MACH_MT_SXT5N:
			return platform_add_hwopt_devices(
			    sxt5n_devices,  ARRAY_SIZE(sxt5n_devices));
		case MACH_MT_SXTG5P:
			return platform_add_hwopt_devices(
			    sxtg5p_devices,  ARRAY_SIZE(sxtg5p_devices));
		case MACH_MT_SXTG5C:
			return platform_add_devices(
			    sxtg5c_devices,  ARRAY_SIZE(sxtg5c_devices));
		case MACH_MT_RB922GS:
			return platform_add_hwopt_devices(
			    rb922gs_devices,  ARRAY_SIZE(rb922gs_devices));
		case MACH_MT_RB750Gv2:
			return platform_add_devices(
			    rb750Gv2_devices,  ARRAY_SIZE(rb750Gv2_devices));
		case MACH_MT_951HND:
			return platform_add_hwopt_devices(
			    rb951hnd_devices,  ARRAY_SIZE(rb951hnd_devices));
		case MACH_MT_RB951G:
			return platform_add_devices(
			    rb951G_devices,  ARRAY_SIZE(rb951G_devices));
		case MACH_MT_CM2N:
			return platform_add_devices(
			    cm2n_devices,  ARRAY_SIZE(cm2n_devices));
		case MACH_MT_mAP:
			return platform_add_devices(
			    mAP_devices,  ARRAY_SIZE(mAP_devices));
		case MACH_MT_RB2011G:
			return platform_add_devices(
			    rb2011G_devices,  ARRAY_SIZE(rb2011G_devices));
		case MACH_MT_RB2011US:
			return platform_add_devices(
			    rb2011US_devices,  ARRAY_SIZE(rb2011US_devices));
		case MACH_MT_RB2011L:
			return platform_add_devices(
			    rb2011L_devices,  ARRAY_SIZE(rb2011L_devices));
		case MACH_MT_RB2011LS:
			return platform_add_devices(
			    rb2011LS_devices,  ARRAY_SIZE(rb2011LS_devices));
		case MACH_MT_RB2011R5:
			return platform_add_hwopt_devices(
			    rb2011r5_devices,  ARRAY_SIZE(rb2011r5_devices));
		case MACH_MT_RB953GS:
			return platform_add_devices(
			    rb953gs_devices,  ARRAY_SIZE(rb953gs_devices));
		case MACH_MT_RB941HL:
			return platform_add_devices(
			    rb941HL_devices,  ARRAY_SIZE(rb941HL_devices));
		case MACH_MT_RB750r2:
			return platform_add_hwopt_devices(
			    rb750r2_devices,  ARRAY_SIZE(rb750r2_devices));
		case MACH_MT_CRS125G:
			return platform_add_hwopt_devices(
			    crs125g_devices,  ARRAY_SIZE(crs125g_devices));
		case MACH_MT_CRS109:
			return platform_add_hwopt_devices(
			    crs109_devices,  ARRAY_SIZE(crs109_devices));
		case MACH_MT_GROOVE52:
			return platform_add_devices(
			    groove52_devices,  ARRAY_SIZE(groove52_devices));
		case MACH_MT_RB951:
			return platform_add_devices(
			    rb951_devices,  ARRAY_SIZE(rb951_devices));
		case MACH_MT_RB_GROOVE:
			return platform_add_devices(
			    rb_groove_devices,  ARRAY_SIZE(rb_groove_devices));
		case MACH_MT_RB750:
			return platform_add_devices(
				rb750_devices,  ARRAY_SIZE(rb750_devices));
		case MACH_MT_RB751:
			return platform_add_devices(
				rb751_devices,  ARRAY_SIZE(rb751_devices));
		case MACH_MT_RB_OMNI_5FE:
			return platform_add_devices(
				rb_omni_5fe_devices,
				ARRAY_SIZE(rb_omni_5fe_devices));
		}
		break;
	case MACH_GROUP_MT_CR:
		return platform_add_devices(
		    cr_devices, ARRAY_SIZE(cr_devices));
	case MACH_GROUP_MT_VM:
		return 0;
	case MACH_GROUP_MT_MUSIC:
		switch (mips_machtype) {
		case MACH_MT_CCS226:
			return platform_add_hwopt_devices(
				ccs226_devices,  ARRAY_SIZE(ccs226_devices));
		case MACH_MT_CCS112:
			return platform_add_devices(
				ccs112_devices,  ARRAY_SIZE(ccs112_devices));
		case MACH_MT_CRS210:
			return platform_add_hwopt_devices(
				crs210_devices,  ARRAY_SIZE(crs210_devices));
		case MACH_MT_CRS212:
			return platform_add_hwopt_devices(
				crs212_devices,  ARRAY_SIZE(crs212_devices));
		}
		break;
	}

	printk("ERROR: unknown RB version\n");
	return 0;
}

arch_initcall(rb_platform_init);
