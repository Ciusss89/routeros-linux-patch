#include <linux/init.h>
#include <linux/leds.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <asm/rb/boards.h>
#include <asm/rb/rb400.h>

#define GPIO_BASE	0x18040000
#define GPIO_BASE_MUSIC	0x18080000
#define GPIO_OE_REG	    0x0000
#define GPIO_IN_REG	    0x0004
#define GPIO_O1_REG	    0x0008
#define GPIO_S1_REG	    0x000c
#define GPIO_S0_REG	    0x0010

struct rb_led_data {
	struct led_classdev cdev;
	unsigned map;
};
static struct rb_led_data rb_leds[] = {
	{ .cdev = { .name = "user-led" } },
	{ .cdev = { .name = "led1" } },
	{ .cdev = { .name = "led2" } },
	{ .cdev = { .name = "led3" } },
	{ .cdev = { .name = "led4" } },
	{ .cdev = { .name = "led5" } },
	{ .cdev = { .name = "usb-power-off" } },
	{ .cdev = { .name = "power-led" } },
	{ .cdev = { .name = "wlan-led" } },
	{ .cdev = { .name = "mpcie-power-off" } },
	{ .cdev = { .name = "mpcie2-power-off" } },
	{ .cdev = { .name = "lcd" } },
	{ .cdev = { .name = "button" }, .map = PLED_CFG_INPUT },
	{ .cdev = { .name = "pin_hole" }, .map = PLED_CFG_INPUT },
	{ .cdev = { .name = "fan_off" } },
	{ .cdev = { .name = "user-led2" } },
	{ .cdev = { .name = "sfp-led" } },
	{ .cdev = { .name = "link-act-led" } },
	{ .cdev = { .name = "all-leds" } },
	{ .cdev = { .name = "omni_led" } },
	{ .cdev = { .name = "ap_omni_led" } },
	{ .cdev = { .name = "ap_dir_led" } },
	{ .cdev = { .name = "control" } },
	{ .cdev = { .name = "heater" } },
	{ .cdev = { .name = "mode-button" }, .map = PLED_CFG_INPUT },
	/* XXX: keep in sync with enum pled_name in <asm/rb/rb400.h> */
};

static void (*set_wifi_gpo)(void *obj, unsigned off, unsigned on) = NULL;
static void *set_wifi_gpo_obj = NULL;
static unsigned wifi_gpo_mask = 0;
static void __iomem *gpio_base = NULL;

unsigned register_wifi_gpo(void *obj,
			   void (*set_gpo)(void *, unsigned, unsigned)) {
	set_wifi_gpo_obj = obj;
	set_wifi_gpo = set_gpo;
	return wifi_gpo_mask;
}
EXPORT_SYMBOL(register_wifi_gpo);

static void gpio_writel(unsigned val, unsigned ofs) {
	if (gpio_base) {
		rb400_writel(val, (unsigned) gpio_base + ofs);
	}
}

static unsigned gpio_readl(unsigned ofs) {
	if (gpio_base) {
		return rb400_readl((unsigned) gpio_base + ofs);
	}
	return 0;
}

static void gpio_rmw(unsigned ofs, unsigned off, unsigned on) {
	if (gpio_base) {
		unsigned val = rb400_readl((unsigned) gpio_base + ofs);
		val = (val | on) & ~off;
		rb400_writel(val, (unsigned) gpio_base + ofs);
	}
}

static void rb400_led_set(struct led_classdev *led_cdev,
			  enum led_brightness brightness)
{
	unsigned map = container_of(led_cdev, struct rb_led_data, cdev)->map;
	unsigned bit = PLED_GET_BIT(map);
	int val = (!brightness) ^ (!(map & PLED_CFG_INV));
	unsigned on = val ? bit : 0;
	unsigned off = val ? 0: bit;

	switch(PLED_GET_TYPE(map)) {
	case PLED_TYPE_GPIOLIB:
		gpio_set_value(PLED_GET_BIT_NUM(map), val);
		break;
	case PLED_TYPE_GPIO:
		gpio_writel(bit, val ? GPIO_S1_REG : GPIO_S0_REG);
		break;
	case PLED_TYPE_GPIO_OE:
		gpio_rmw(GPIO_OE_REG, off, on);
		break;
#ifdef CONFIG_SPI_RB400
	case PLED_TYPE_SHARED_RB400:
		rb400_change_cfg(on, off);
		break;
	case PLED_TYPE_SSR_RB400:
		rb_change_cfg(off, on);
		break;
#endif
#ifdef CONFIG_MTD_NAND_RB400
	case PLED_TYPE_SHARED_RB700:
		rb700_change_gpo(off, on);
		break;
	case PLED_TYPE_SSR_MUSIC:
		music_change_ssr_cfg(off, on);
		break;
	case PLED_TYPE_SHARED_RB900:
		ar9342_change_gpo(off, on);
		break;
	case PLED_TYPE_SSR_RB900:
		ar9342_change_gpo(off << 20, on << 20);
		break;
#endif
	case PLED_TYPE_WIFI:
		if (set_wifi_gpo) (*set_wifi_gpo)(set_wifi_gpo_obj, on, off);
		wifi_gpo_mask &= ~(bit << 16);
		wifi_gpo_mask |= ((on << 16) | bit);
		break;
	}
}

static enum led_brightness rb400_led_get(struct led_classdev *led_cdev)
{
	unsigned map = container_of(led_cdev, struct rb_led_data, cdev)->map;
	unsigned bit = PLED_GET_BIT(map);
	unsigned val;

	if (!gpio_base) {
		return led_cdev->brightness;
	}
	switch(PLED_GET_TYPE(map)) {
	case PLED_TYPE_GPIOLIB:
		val = gpio_get_value(PLED_GET_BIT_NUM(map)) ? bit : 0;
		break;
	case PLED_TYPE_GPIO:
		val = gpio_readl(GPIO_IN_REG);
		break;
	case PLED_TYPE_GPIO_OE:
		val = gpio_readl(GPIO_OE_REG);
		break;
	case PLED_TYPE_SHARED_GPIO:
		// on AR934x OE is actually input-enable
		if (gpio_readl(GPIO_OE_REG) & bit) {
			printk("rb400_led_get ERROR: "
			       "PLED_TYPE_SHARED_GPIO on input, bitmask %x\n",
				bit);
			return 1;
		}
		// wait for gpio no to be used before make it as input
		while ((gpio_readl(GPIO_O1_REG) & bit) == 0) {
			// wait for NAND_CE# to be released
			schedule();
		}
		gpio_rmw(GPIO_OE_REG, 0, bit);
		udelay(200);
		val = gpio_readl(GPIO_IN_REG);
		gpio_rmw(GPIO_OE_REG, bit, 0);
		break;
#ifdef CONFIG_MTD_NAND_RB400
	case PLED_TYPE_SHARED_RB900:
		do {
			val = ar9342_change_gpo(0, 0);
			if (val == 0) schedule();
		} while (val == 0);
		break;
#endif
	default:
		return led_cdev->brightness;
	}
	return !(val & bit) ^ !(map & PLED_CFG_INV);
}

void rb400_beepled(int on) {
	int i;
	for (i = 0; i < sizeof(rb_leds) / sizeof(rb_leds[0]); ++i) {
		struct rb_led_data *rbled = &rb_leds[i];
		if (rbled->cdev.brightness_set == NULL) continue;
		if (strstr(rbled->cdev.name, "led") == NULL) continue;
		if (strcmp(rbled->cdev.name, "power-led") == 0) continue;
		rb400_led_set(&rbled->cdev, (!on) ^ (!rbled->cdev.brightness));
	}
}
EXPORT_SYMBOL(rb400_beepled);

static int rb400_led_probe(struct platform_device *pdev)
{
	unsigned *map = (unsigned *)pdev->dev.platform_data;
	unsigned base = GPIO_BASE;

	if (mips_machgroup == MACH_GROUP_MT_MUSIC) {
		base = GPIO_BASE_MUSIC;
	}
	gpio_base = ioremap_nocache(base, PAGE_SIZE);

	for ( ; *map != 0; ++map) {
		struct rb_led_data *data;
		int name_idx = PLED_GET_NAME_IDX(*map);
		if (name_idx >= sizeof(rb_leds) / sizeof(rb_leds[0])) {
			printk("rb400-leds ERROR: idx %d too big!\n", name_idx);
			continue;
		}
		data = &(rb_leds[name_idx]);
		data->map |= *map;

		if (PLED_GET_TYPE(data->map) == PLED_TYPE_GPIOLIB) {
			int gpio_num = PLED_GET_BIT_NUM(data->map);
			if (gpio_request(gpio_num, data->cdev.name) < 0) {
				printk("rb400-leds ERROR: "
				       "gpio_request(%d) failed\n", gpio_num);
				continue;
			}
			if (data->map & PLED_CFG_INPUT) {
				gpio_direction_input(gpio_num);
#ifdef KERNEL_SUPPORTS_GPIO_EXPORT_WITH_NAME
				gpiod_export_with_name(gpio_to_desc(gpio_num),
						       false, data->cdev.name);
				continue;
#endif
			}
			else {
				unsigned val = gpio_get_value(gpio_num);
				gpio_direction_output(gpio_num, val);
			}
		}

		data->cdev.brightness_set = rb400_led_set;
		data->cdev.brightness_get = rb400_led_get;
		led_classdev_register(&pdev->dev, &data->cdev);
		if (!(data->map & (PLED_CFG_KEEP | PLED_CFG_INPUT))) {
		    data->cdev.brightness = !!(data->map & PLED_CFG_ON);
		    rb400_led_set(&data->cdev, data->cdev.brightness);
                }
	}
	return 0;
}

static struct platform_driver rb400_led_driver = {
	.probe	= rb400_led_probe,
	.driver	= {
		.name = "rb400-led",
		.owner = THIS_MODULE,
	},
};
module_platform_driver(rb400_led_driver);
