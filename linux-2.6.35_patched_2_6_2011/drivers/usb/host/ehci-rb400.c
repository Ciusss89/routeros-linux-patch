#include <linux/platform_device.h>
#include <asm/rb/boards.h>
#include <asm/rb/rb400.h>

#define AR724x_RESET			0x1806001c

#define AR724x_RST_USBSUS_OVRIDE	(1 << 3)
#define AR724x_RST_USB_HOST		(1 << 5)
#define AR724x_RST_USB_PHY		(1 << 4)

static void modreg(unsigned addr, unsigned set, unsigned clear) {
	rb400_writel((rb400_readl(addr) & ~clear) | set, addr);
}

static int is700(void) {
	return mips_machgroup == MACH_GROUP_MT_RB700;
}

static int usb_ehci_rb400_reset(struct usb_hcd *hcd)
{
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	unsigned offset = is700() ? 0x100 : 0;
	int retval;

	ehci->caps = hcd->regs + offset;
	ehci->regs = hcd->regs + offset 
		+ HC_LENGTH(ehci_readl(ehci, &ehci->caps->hc_capbase));
	ehci->hcs_params = ehci_readl(ehci, &ehci->caps->hcs_params);

	if (!is700()) {
		retval = ehci_halt(ehci);
		if (retval)
			return retval;
	}
	
	retval = ehci_init(hcd);
	if (retval)
		return retval;
	
	if (is700()) {
		hcd->has_tt = 1;
	}

	ehci->sbrn = 0x20;
	return ehci_reset(ehci);
}

static int usb_ehci_rb400_probe(const struct hc_driver *driver, 
				struct platform_device *pdev)
{
	struct usb_hcd *hcd;
	struct resource *res;
	int irq;
	int retval = -ENOMEM;

	if (is700() && is_ar7240()) {
		return -ENODEV;
	}

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		dev_err(&pdev->dev, "no IRQ\n");
		return -ENODEV;
	}
	irq = res->start;

	hcd = usb_create_hcd(driver, &pdev->dev, "rb400_usb");
	if (!hcd)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "no mem region\n");
		goto err1;
	}

	hcd->rsrc_start = res->start;
	hcd->rsrc_len = res->end - res->start + 1;

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len,
				driver->description)) {
		dev_err(&pdev->dev, "memory already in use\n");
		retval = -EBUSY;
		goto err1;
	}

	hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len);
	if (!hcd->regs) {
		dev_err(&pdev->dev, "ioremap failed");
		retval = -EFAULT;
		goto err2;
	}

	if (is700()) {
		unsigned ptr;
		struct ehci_hcd *ehci = hcd_to_ehci(hcd);
		ehci->caps = hcd->regs + 0x100;
		ehci->regs = hcd->regs + 0x140;

		ptr = (unsigned) ioremap_nocache(AR724x_RESET, 4);

		modreg(ptr, AR724x_RST_USBSUS_OVRIDE, 0);
		mdelay(10);

		modreg(ptr, AR724x_RST_USBSUS_OVRIDE, AR724x_RST_USB_HOST);
		mdelay(10);
		
		modreg(ptr, AR724x_RST_USBSUS_OVRIDE, AR724x_RST_USB_PHY);
		mdelay(10);

		iounmap((void *) ptr);

	}

	retval = usb_add_hcd(hcd, irq, IRQF_SHARED);
	if (retval != 0)
		goto err3;
	return 0;

  err3:
	iounmap(hcd->regs);
  err2:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
  err1:
	usb_put_hcd(hcd);
	return retval;
}


void usb_ehci_rb400_remove(struct usb_hcd *hcd, struct platform_device *dev)
{
	usb_remove_hcd(hcd);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);
}

static const struct hc_driver ehci_rb400_hc_driver = {
	.description		= hcd_name,
	.product_desc		= "RB400 EHCI",
	.hcd_priv_size		= sizeof(struct ehci_hcd),
	.irq			= ehci_irq,
	.flags			= HCD_MEMORY | HCD_USB2,
	.reset			= usb_ehci_rb400_reset,
	.start			= ehci_run,
	.stop			= ehci_stop,
	.shutdown		= ehci_shutdown,	    
	.urb_enqueue		= ehci_urb_enqueue,
	.urb_dequeue		= ehci_urb_dequeue,
	.endpoint_disable	= ehci_endpoint_disable,
	.get_frame_number	= ehci_get_frame,
	.hub_status_data	= ehci_hub_status_data,
	.hub_control		= ehci_hub_control,
};

static int ehci_rb400_drv_probe(struct platform_device *pdev)
{
	if (usb_disabled())
		return -ENODEV;

	return usb_ehci_rb400_probe(&ehci_rb400_hc_driver, pdev);
}

static int ehci_rb400_drv_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);

	usb_ehci_rb400_remove(hcd, pdev);
	return 0;
}

static struct platform_driver ehci_rb400_driver = {
	.probe	=  ehci_rb400_drv_probe,
	.remove	=  ehci_rb400_drv_remove,
	.driver	= {
		.name = "rb400-ehci"
	},
};
