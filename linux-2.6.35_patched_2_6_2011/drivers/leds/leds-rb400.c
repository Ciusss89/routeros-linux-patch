#include <linux/init.h>
#include <linux/leds.h>
#include <linux/platform_device.h>
#include <asm/rb/rb400.h>

#define REG_GPIO_SET	KSEG1ADDR(0x1804000C)
#define REG_GPIO_CLR	KSEG1ADDR(0x18040010)
#define   GPIO_LIGHT	0x010

struct rb_led_data {
	struct led_classdev cdev;
	unsigned mask;
};
static struct rb_led_data rb_leds[] = {
	{ .cdev = { .name = "user-led" } },
	{ .cdev = { .name = "led1" } },
	{ .cdev = { .name = "led2" } },
	{ .cdev = { .name = "led3" } },
	{ .cdev = { .name = "led4" } },
	{ .cdev = { .name = "led5" } },
	{ .cdev = { .name = "usb-power-off" } },
	{ .cdev = { .name = NULL } },
};

static void (*set_wifi_gpo)(void *obj, unsigned off, unsigned on) = NULL;
static void *set_wifi_gpo_obj = NULL;
static unsigned wifi_gpo_mask = 0;

unsigned register_wifi_gpo(void *obj,
			   void (*set_gpo)(void *, unsigned, unsigned)) {
	set_wifi_gpo_obj = obj;
	set_wifi_gpo = set_gpo;
	return wifi_gpo_mask;
}
EXPORT_SYMBOL(register_wifi_gpo);

static void rb_led_set_wifi(struct led_classdev *led_cdev,
			    enum led_brightness brightness)
{
	unsigned mask = container_of(led_cdev, struct rb_led_data, cdev)->mask;
	if (set_wifi_gpo == NULL) return;
	if (brightness)
		(*set_wifi_gpo)(set_wifi_gpo_obj, mask & 0xffff, mask >> 16);
	else
		(*set_wifi_gpo)(set_wifi_gpo_obj, mask >> 16, mask & 0xffff);
}

static void rb_led_set_gpo(struct led_classdev *led_cdev,
			   enum led_brightness brightness)
{
	unsigned mask = container_of(led_cdev, struct rb_led_data, cdev)->mask;
	if (brightness)
		rb400_writel(mask, REG_GPIO_SET);
	else
		rb400_writel(mask, REG_GPIO_CLR);
}

static void rb_led_set_rb400(struct led_classdev *led_cdev,
			     enum led_brightness brightness)
{
	unsigned mask = container_of(led_cdev, struct rb_led_data, cdev)->mask;
	if (brightness)
		rb400_change_cfg(mask, 0);
	else
		rb400_change_cfg(0, mask);
}

static void rb_led_set_rb700(struct led_classdev *led_cdev,
			     enum led_brightness brightness)
{
	unsigned mask = container_of(led_cdev, struct rb_led_data, cdev)->mask;
	if (brightness)
		rb700_change_gpo(mask, 0);
	else
		rb700_change_gpo(0, mask);
}

static void *rb_led_set[0x10] = {
	rb_led_set_gpo, NULL, NULL, NULL,
	rb_led_set_rb400,
	NULL,
	NULL,
	rb_led_set_rb700, NULL,
	NULL,
	NULL, NULL, NULL, NULL, NULL,
	rb_led_set_wifi
};

static int rb400_led_probe(struct platform_device *pdev)
{
	int *map = (int *)pdev->dev.platform_data;
	struct rb_led_data *data = rb_leds;

	for ( ; *map >= 0 && data->cdev.name != NULL; ++map, ++data) {
		data->mask = *map >> 4;
		data->cdev.brightness_set = rb_led_set[*map & 0xf];
		if (data->mask != 0 && data->cdev.brightness_set != NULL) {
			led_classdev_register(&pdev->dev, &data->cdev);
			if (data->cdev.brightness_set == rb_led_set_wifi) {
				wifi_gpo_mask |= data->mask;
			}
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

static int __init rb400_led_init(void)
{
	return platform_driver_register(&rb400_led_driver);
}

static void __exit rb400_led_exit(void)
{
	platform_driver_unregister(&rb400_led_driver);
}

module_init(rb400_led_init);
module_exit(rb400_led_exit);
