/*
 * linux/arch/arm/mach-omap2/usb-musb.c
 *
 * This file will contain the board specific details for the
 * MENTOR USB OTG controller on OMAP3430
 *
 * Copyright (C) 2007-2008 Texas Instruments
 * Copyright (C) 2008 Nokia Corporation
 * Author: Vikram Pandita
 *
 * Generalization by:
 * Felipe Balbi <felipe.balbi@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>

#include <linux/usb/android_composite.h>
#include <linux/usb/musb.h>

#include <asm/sizes.h>

#include <mach/hardware.h>
#include <mach/irqs.h>
#include <plat/mux.h>
#include <plat/usb.h>

#define OTG_SYSCONFIG	   0x404
#define OTG_SYSC_SOFTRESET BIT(1)
#define OTG_SYSSTATUS     0x408
#define OTG_SYSS_RESETDONE BIT(0)

static struct platform_device dummy_pdev = {
	.dev = {
		.bus = &platform_bus_type,
	},
};

static void __iomem *otg_base;
static struct clk *otg_clk;

#define  MIDLEMODE       12      /* bit position */
#define  FORCESTDBY      (0 << MIDLEMODE)
#define  NOSTDBY         (1 << MIDLEMODE)
#define  SMARTSTDBY      (2 << MIDLEMODE)
#define  SIDLEMODE       3       /* bit position */
#define  FORCEIDLE       (0 << SIDLEMODE)
#define  NOIDLE          (1 << SIDLEMODE)
#define  SMARTIDLE       (2 << SIDLEMODE)


static void __init usb_musb_pm_init(void)
{
	struct device *dev = &dummy_pdev.dev;

	if (!cpu_is_omap34xx())
		return;

	otg_base = ioremap(OMAP34XX_HSUSB_OTG_BASE, SZ_4K);
	if (WARN_ON(!otg_base))
		return;

	dev_set_name(dev, "musb_hdrc");
	otg_clk = clk_get(dev, "ick");

	if (otg_clk && clk_enable(otg_clk)) {
		printk(KERN_WARNING
			"%s: Unable to enable clocks for MUSB, "
			"cannot reset.\n",  __func__);
	} else {
		/* Reset OTG controller. After reset, it will be in
		 * force-idle, force-standby mode. */
		__raw_writel(OTG_SYSC_SOFTRESET, otg_base + OTG_SYSCONFIG);

		while (!(OTG_SYSS_RESETDONE &
					__raw_readl(otg_base + OTG_SYSSTATUS)))
			cpu_relax();
	}

	if (otg_clk)
		clk_disable(otg_clk);
}

#ifdef CONFIG_ANDROID

#ifdef CONFIG_ARCH_OMAP4
#define DIE_ID_REG_BASE 		(L4_44XX_PHYS + 0x2000)
#define DIE_ID_REG_OFFSET		0x200
#else
#define DIE_ID_REG_BASE 		(L4_WK_34XX_PHYS + 0xA000)
#define DIE_ID_REG_OFFSET		0x218
#endif /* CONFIG_ARCH_OMAP4 */

#define MAX_USB_SERIAL_NUM		33
#define OMAP_VENDOR_ID  		0x0451
#define OMAP_PRODUCT_ID 		0xffff

static char device_serial[MAX_USB_SERIAL_NUM];

static char *usb_functions_all[] = {
#ifdef CONFIG_USB_ANDROID_MASS_STORAGE
	"usb_mass_storage",
#endif
#ifdef CONFIG_USB_ANDROID_ADB
	"adb",
#endif
#ifdef CONFIG_USB_ANDROID_ACM
	"acm",
#endif
#ifdef CONFIG_USB_ANDROID_RNDIS
	"rndis"
#endif
};

static struct android_usb_product usb_products[] = {
	{
		.product_id     = OMAP_PRODUCT_ID,
		.num_functions  = ARRAY_SIZE(usb_functions_all),
		.functions      = usb_functions_all,
	},
};

/* standard android USB platform data */
static struct android_usb_platform_data andusb_plat = {
	.vendor_id                      = OMAP_VENDOR_ID,
	.product_id                     = OMAP_PRODUCT_ID,
	.manufacturer_name      = "Texas Instruments Inc.",
#ifdef CONFIG_ARCH_OMAP4
	.product_name           = "OMAP4",
#else
	.product_name           = "OMAP3",
#endif /* CONFIG_ARCH_OMAP4 */
	.serial_number          = device_serial,
	.num_products = ARRAY_SIZE(usb_products),
	.products = usb_products,
	.num_functions = ARRAY_SIZE(usb_functions_all),
	.functions = usb_functions_all,
};

static struct platform_device androidusb_device = {
	.name   = "android_usb",
	.id     = -1,
	.dev    = {
		.platform_data  = &andusb_plat,
	},
};

static void usb_gadget_init(void)
{
	unsigned int val[4];
	unsigned int reg;
	reg = DIE_ID_REG_BASE + DIE_ID_REG_OFFSET;

	if (cpu_is_omap44xx()) {
		val[0] = omap_readl(reg);
		val[1] = omap_readl(reg + 0x8);
		val[2] = omap_readl(reg + 0xC);
		val[3] = omap_readl(reg + 0x10);
	} else if (cpu_is_omap34xx()) {
		val[0] = omap_readl(reg);
		val[1] = omap_readl(reg + 0x4);
		val[2] = omap_readl(reg + 0x8);
		val[3] = omap_readl(reg + 0xC);
	}

	snprintf(device_serial, MAX_USB_SERIAL_NUM, "%08X%08X%08X%08X", val[3], val[2], val[1], val[0]);

	platform_device_register(&androidusb_device);
}

#else

static void usb_gadget_init(void)
{
}

#endif /* CONFIG_ANDROID */

void usb_musb_disable_autoidle(void)
{
	__raw_writel(0, otg_base + OTG_SYSCONFIG);
}


void musb_disable_idle(int on)
{
	u32 reg;

	if (!cpu_is_omap34xx())
		return;

	reg = omap_readl(OMAP34XX_HSUSB_OTG_BASE + OTG_SYSCONFIG);

	if (on) {
		reg &= ~SMARTSTDBY;    /* remove possible smartstdby */
		reg |= NOSTDBY;        /* enable no standby */
		reg |= NOIDLE;        /* enable no idle */
	} else {
		reg &= ~NOSTDBY;          /* remove possible nostdby */
		reg &= ~NOIDLE;          /* remove possible noidle */
		reg |= SMARTSTDBY;        /* enable smart standby */
	}

	omap_writel(reg, OMAP34XX_HSUSB_OTG_BASE + OTG_SYSCONFIG);
}

#ifdef CONFIG_USB_MUSB_SOC

static struct resource musb_resources[] = {
	[0] = { /* start and end set dynamically */
		.flags	= IORESOURCE_MEM,
	},
	[1] = {	/* general IRQ */
		.start	= INT_243X_HS_USB_MC,
		.flags	= IORESOURCE_IRQ,
	},
	[2] = {	/* DMA IRQ */
		.start	= INT_243X_HS_USB_DMA,
		.flags	= IORESOURCE_IRQ,
	},
};

static int clk_on;

static int musb_set_clock(struct clk *clk, int state)
{
	if (state) {
		if (clk_on > 0)
			return -ENODEV;

		clk_enable(clk);
		clk_on = 1;
	} else {
		if (clk_on == 0)
			return -ENODEV;

		clk_disable(clk);
		clk_on = 0;
	}

	return 0;
}

static struct musb_hdrc_eps_bits musb_eps[] = {
	{	"ep1_tx", 10,	},
	{	"ep1_rx", 10,	},
	{	"ep2_tx", 9,	},
	{	"ep2_rx", 9,	},
	{	"ep3_tx", 3,	},
	{	"ep3_rx", 3,	},
	{	"ep4_tx", 3,	},
	{	"ep4_rx", 3,	},
	{	"ep5_tx", 3,	},
	{	"ep5_rx", 3,	},
	{	"ep6_tx", 3,	},
	{	"ep6_rx", 3,	},
	{	"ep7_tx", 3,	},
	{	"ep7_rx", 3,	},
	{	"ep8_tx", 2,	},
	{	"ep8_rx", 2,	},
	{	"ep9_tx", 2,	},
	{	"ep9_rx", 2,	},
	{	"ep10_tx", 2,	},
	{	"ep10_rx", 2,	},
	{	"ep11_tx", 2,	},
	{	"ep11_rx", 2,	},
	{	"ep12_tx", 2,	},
	{	"ep12_rx", 2,	},
	{	"ep13_tx", 2,	},
	{	"ep13_rx", 2,	},
	{	"ep14_tx", 2,	},
	{	"ep14_rx", 2,	},
	{	"ep15_tx", 2,	},
	{	"ep15_rx", 2,	},
};

static struct musb_hdrc_config musb_config = {
	.multipoint	= 1,
	.dyn_fifo	= 1,
	.soft_con	= 1,
	.dma		= 1,
	.num_eps	= 16,
	.dma_channels	= 7,
	.dma_req_chan	= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3),
	.ram_bits	= 12,
	.eps_bits	= musb_eps,
};

static struct musb_hdrc_platform_data musb_plat = {
#ifdef CONFIG_USB_MUSB_OTG
	.mode		= MUSB_OTG,
#elif defined(CONFIG_USB_MUSB_HDRC_HCD)
	.mode		= MUSB_HOST,
#elif defined(CONFIG_USB_GADGET_MUSB_HDRC)
	.mode		= MUSB_PERIPHERAL,
#endif
	/* .clock is set dynamically */
	.set_clock	= musb_set_clock,
	.config		= &musb_config,

	/* REVISIT charge pump on TWL4030 can supply up to
	 * 100 mA ... but this value is board-specific, like
	 * "mode", and should be passed to usb_musb_init().
	 */
	.power		= 50,			/* up to 100 mA */
};

static u64 musb_dmamask = DMA_BIT_MASK(32);

static struct platform_device musb_device = {
	.name		= "musb_hdrc",
	.id		= -1,
	.dev = {
		.dma_mask		= &musb_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
		.platform_data		= &musb_plat,
	},
	.num_resources	= ARRAY_SIZE(musb_resources),
	.resource	= musb_resources,
};

void __init usb_musb_init(struct omap_musb_board_data *board_data)
{
	if (cpu_is_omap243x()) {
		musb_resources[0].start = OMAP243X_HS_BASE;
	} else if (cpu_is_omap34xx()) {
		musb_resources[0].start = OMAP34XX_HSUSB_OTG_BASE;
	} else if (cpu_is_omap44xx()) {
		musb_resources[0].start = OMAP44XX_HSUSB_OTG_BASE;
		musb_resources[1].start = INT_44XX_HS_USB_MC;
		musb_resources[2].start = INT_44XX_HS_USB_DMA;
	}
	musb_resources[0].end = musb_resources[0].start + SZ_8K - 1;

	/*
	 * REVISIT: This line can be removed once all the platforms using
	 * musb_core.c have been converted to use use clkdev.
	 */
	musb_plat.clock = "ick";
	musb_plat.board_data = board_data;
	musb_plat.power = board_data->power >> 1;
	musb_plat.mode = board_data->mode;

	if (platform_device_register(&musb_device) < 0) {
		printk(KERN_ERR "Unable to register HS-USB (MUSB) device\n");
		return;
	}

	usb_musb_pm_init();
	usb_gadget_init();
}

#else
void __init usb_musb_init(struct omap_musb_board_data *board_data)
{
	usb_musb_pm_init();
}
#endif /* CONFIG_USB_MUSB_SOC */
