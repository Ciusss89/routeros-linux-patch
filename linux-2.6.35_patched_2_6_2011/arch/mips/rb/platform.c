#include <linux/platform_device.h>
#include <linux/serial.h>
#include <linux/serial_8250.h>
#include <linux/spi/spi.h>
#include <asm/bootinfo.h>
#include <asm/rb/boards.h>
#include <asm/serial.h>

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

static struct spi_board_info rb400_spi_misc = {
	.modalias = "rb400-spi-misc",
	.max_speed_hz = 10 * 1000 * 1000,
	.bus_num = 0,
	.chip_select = 0,
	.mode = SPI_MODE_0,
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

static struct spi_board_info *rb750_spi_info[] = {
	&rb400_spi_misc,
	NULL
};

static struct platform_device rb400_spi_device = {
	.name	= "rb400-spi",
	.id	= -1,
	.dev	= {
		.platform_data = rb400_spi_info,
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

static int rb400_led_map[] = { 0x100, 0x14, 0x24, 0x44, 0x84, 0x1004, -1 };
static struct platform_device rb400_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = rb400_led_map,
	},
};

static int rb450g_led_map[] = { 0x100, 0x1004, -1 };
static struct platform_device rb450g_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = rb450g_led_map,
	},
};

static int rb493g_led_map[] = { 0x100, 0x14, 0x44, 0x24, 0x84, 0x1004, -1 };
static struct platform_device rb493g_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = rb493g_led_map,
	},
};

static int rb700_led_map[] = { 0x10007, -1 };
static struct platform_device rb700_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = rb700_led_map,
	},
};

static int sxt5d_led_map[] =
	{ 0x10007, 0x20f, 0x40f, 0x80f, 0x100f, 0x200f, 0x8f, -1 };
static struct platform_device sxt5d_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = sxt5d_led_map,
	},
};

static int groove_led_map[] =
	{ 0x10007, 0x200f, 0x100f, 0x80f, 0x40f, 0x20f, 0x8f, -1 };
static struct platform_device groove_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = groove_led_map,
	},
};

static int rb711_led_map[] =
	{ 0x27, 0x10007, 0x20007, 0x40007, 0x80007, 0x100007, -1 };
static struct platform_device rb711_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = rb711_led_map,
	},
};

static int rb711r3_led_map[] =
	{ 0x27, 0x10007, 0x40007, 0x80007, 0x100007, 0, 0x10f, -1 };
static struct platform_device rb711r3_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = rb711r3_led_map,
	},
};

static int rb750g_led_map[] = { 0x100, -1 };
static struct platform_device rb750g_led_device = {
	.name	= "rb400-led",
	.id	= -1,
	.dev	= {
		.platform_data = rb750g_led_map,
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

static int rb411_eth_port_map[] = { 1, 0, 1, -1 };
static struct platform_device rb411_eth_device = {
	.name	= "ag7100",
	.id	= -1,
	.dev	= {
		.platform_data = rb411_eth_port_map,
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

static struct platform_device *rb411u_devices[] = {
	&rb400_ohci_device,
	&rb400_ehci_device,
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
	&rb400_ohci_device,
	&rb400_ehci_device,
	&rb433_spi_device,
	&rb433_eth_device,
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
	&rb400_ohci_device,
	&rb400_ehci_device,
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
		.end		= 0x1bffffff,
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

static struct platform_device *rb711_devices[] = {
	&rb700_ohci_device,
	&rb700_ehci_device,
	&rb750_spi_device,
	&rb700_nand_device,
	&rb711_led_device,
	&rb711_eth_device,
};

static struct platform_device *rb711r3_devices[] = {
	&rb700_ohci_device,
	&rb700_ehci_device,
	&rb750_spi_device,
	&rb700_nand_device,
	&rb711r3_led_device,
	&rb711r3_eth_device,
};








static struct platform_device *rb_sxt5d_devices[] = {
	&rb700_ohci_device,
	&rb700_ehci_device,
	&rb750_spi_device,
	&rb700_nolatch_nand_device,
	&sxt5d_led_device,
	&rb711r3_eth_device
};

static struct platform_device *rb_groove_devices[] = {
	&rb700_ohci_device,
	&rb700_ehci_device,
	&rb750_spi_device,
	&rb700_nolatch_nand_device,
	&groove_led_device,
	&rb711r3_eth_device
};

static struct platform_device *rb750_devices[] = {
	&rb700_ohci_device,
	&rb700_ehci_device,
	&rb750_spi_device,
	&rb700_tiny_nand_device,
	&rb700_led_device,
	&rb750_eth_device
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
		case MACH_MT_RB411U:
			return platform_add_devices(
				rb411u_devices,  ARRAY_SIZE(rb411u_devices));
		case MACH_MT_RB433:
			return platform_add_devices(
				rb433_devices,  ARRAY_SIZE(rb433_devices));
		case MACH_MT_RB433U:
			return platform_add_devices(
				rb433u_devices,  ARRAY_SIZE(rb433u_devices));
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
		case MACH_MT_RB711:
			return platform_add_devices(
				rb711_devices,  ARRAY_SIZE(rb711_devices));
		case MACH_MT_RB711R3:
			return platform_add_devices(
				rb711r3_devices,  ARRAY_SIZE(rb711r3_devices));
		case MACH_MT_RB_SXT5D:
			return platform_add_devices(
			    rb_sxt5d_devices,  ARRAY_SIZE(rb_sxt5d_devices));
		case MACH_MT_RB_GROOVE:
			return platform_add_devices(
			    rb_groove_devices,  ARRAY_SIZE(rb_groove_devices));
		case MACH_MT_RB750:
			return platform_add_devices(
				rb750_devices,  ARRAY_SIZE(rb750_devices));
		}
		break;
	case MACH_GROUP_MT_CR:
		return platform_add_devices(
		    cr_devices, ARRAY_SIZE(cr_devices));
	case MACH_GROUP_MT_VM:
		return 0;
	}

	printk("ERROR: unknown RB version\n");
	return 0;
}

arch_initcall(rb_platform_init);
