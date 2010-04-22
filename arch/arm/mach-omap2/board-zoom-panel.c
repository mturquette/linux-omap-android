#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/input/matrix_keypad.h>
#include <linux/gpio.h>
#include <linux/i2c/twl.h>
#include <linux/regulator/machine.h>


#include <plat/display.h>
#include <plat/mcspi.h>
#include <linux/spi/spi.h>
#include "mux.h"


#define ENABLE_VAUX2_DEDICATED          0x09
#define ENABLE_VAUX2_DEV_GRP            0x20
#define ENABLE_VAUX3_DEDICATED          0x03
#define ENABLE_VAUX3_DEV_GRP            0x20

#define ENABLE_VPLL2_DEDICATED          0x05
#define ENABLE_VPLL2_DEV_GRP            0xE0
#define TWL4030_VPLL2_DEV_GRP           0x33
#define TWL4030_VPLL2_DEDICATED         0x36


#define LCD_PANEL_BACKLIGHT_GPIO 	(7 + OMAP_MAX_GPIO_LINES)
#define LCD_PANEL_ENABLE_GPIO       (15 + OMAP_MAX_GPIO_LINES)

#define LCD_PANEL_RESET_GPIO		55
#define LCD_PANEL_QVGA_GPIO			56
#define TV_PANEL_ENABLE_GPIO		95


static int zoom_panel_enable_lcd(struct omap_dss_device *dssdev);
static void zoom_panel_disable_lcd(struct omap_dss_device *dssdev);

/*--------------------------------------------------------------------------*/
static struct omap_dss_device zoom_lcd_device = {
	.name = "lcd",
	.driver_name = "NEC_panel",
	.type = OMAP_DISPLAY_TYPE_DPI,
	.phy.dpi.data_lines = 24,
	.platform_enable = zoom_panel_enable_lcd,
	.platform_disable = zoom_panel_disable_lcd,
};

static struct omap_dss_device *zoom_dss_devices[] = {
	&zoom_lcd_device,
};

static struct omap_dss_board_info zoom_dss_data = {
	.num_devices = ARRAY_SIZE(zoom_dss_devices),
	.devices = zoom_dss_devices,
	.default_device = &zoom_lcd_device,
};

static struct platform_device zoom_dss_device = {
	.name          = "omapdss",
	.id            = -1,
	.dev            = {
		.platform_data = &zoom_dss_data,
	},
};

/*--------------------------------------------------------------------------*/
static struct regulator_consumer_supply zoom2_vdds_dsi_supply = {
	.supply		= "vdds_dsi",
	.dev		= &zoom_dss_device.dev,
};

static struct regulator_consumer_supply zoom_vdda_dac_supply = {
	.supply         = "vdda_dac",
	.dev            = &zoom_dss_device.dev,
};

struct regulator_init_data zoom_vdac = {
	.constraints = {
		.min_uV                 = 1800000,
		.max_uV                 = 1800000,
		.valid_modes_mask       = REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask         = REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies  = 1,
	.consumer_supplies      = &zoom_vdda_dac_supply,
};

struct regulator_init_data zoom2_vdsi = {
	.constraints = {
		.min_uV                 = 1800000,
		.max_uV                 = 1800000,
		.valid_modes_mask       = REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask         = REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies  = 1,
	.consumer_supplies      = &zoom2_vdds_dsi_supply,
};


/*--------------------------------------------------------------------------*/
static int zoom_panel_power_enable(int enable)
{
	twl_i2c_write_u8(TWL4030_MODULE_PM_RECEIVER,
				(enable) ? ENABLE_VPLL2_DEDICATED : 0,
				TWL4030_VPLL2_DEDICATED);
	twl_i2c_write_u8(TWL4030_MODULE_PM_RECEIVER,
				(enable) ? ENABLE_VPLL2_DEV_GRP : 0,
				TWL4030_VPLL2_DEV_GRP);
	return 0;
}

static int zoom_panel_enable_lcd(struct omap_dss_device *dssdev)
{
	zoom_panel_power_enable(1);
	gpio_request(LCD_PANEL_BACKLIGHT_GPIO, "lcd backlight");
	gpio_direction_output(LCD_PANEL_BACKLIGHT_GPIO, 1);

	return 0;
}

static void zoom_panel_disable_lcd(struct omap_dss_device *dssdev)
{
	gpio_direction_output(LCD_PANEL_BACKLIGHT_GPIO, 0);
}

struct platform_device *zoom_devices[] __initdata = {
	&zoom_dss_device,
	/*&zoom_vout_device,*/
};

/*--------------------------------------------------------------------------*/
static struct omap2_mcspi_device_config zoom_lcd_mcspi_config = {
	.turbo_mode             = 0,
	.single_channel         = 1,  /* 0: slave, 1: master */

};

struct spi_board_info zoom_spi_board_info[] __initdata = {
	[0] = {
		.modalias               = "zoom_disp_spi",
		.bus_num                = 1,
		.chip_select            = 2,
		.max_speed_hz           = 375000,
		.controller_data        = &zoom_lcd_mcspi_config,
	},
};

/*--------------------------------------------------------------------------*/
void zoom_lcd_tv_panel_init(void)
{
	unsigned char lcd_panel_reset_gpio;

	if (1) {
		/* Production Zoom2 Board: */
		omap_mux_init_signal("gpio_96", OMAP_PIN_OUTPUT);
		lcd_panel_reset_gpio = 96;
	} else {
		/* Pilot Zoom2 board */
		omap_mux_init_signal("gpio_55", OMAP_PIN_OUTPUT);
		lcd_panel_reset_gpio = 55;
	}
	omap_mux_init_signal("gpio_167", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("mcspi1_cs2", OMAP_PIN_INPUT_PULLDOWN);
	omap_mux_init_signal("gpio_94", OMAP_PIN_INPUT);
	omap_mux_init_signal("gpio_96", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("gpio_56", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("gpio_95", OMAP_PIN_OUTPUT);

	gpio_request(lcd_panel_reset_gpio, "lcd reset");
	gpio_request(LCD_PANEL_QVGA_GPIO, "lcd qvga");
	gpio_request(TV_PANEL_ENABLE_GPIO, "tv enable");

	gpio_direction_output(LCD_PANEL_QVGA_GPIO, 0);
	gpio_direction_output(lcd_panel_reset_gpio, 0);
	gpio_direction_output(LCD_PANEL_BACKLIGHT_GPIO, 0);
	gpio_direction_output(TV_PANEL_ENABLE_GPIO, 0);
	gpio_direction_output(LCD_PANEL_QVGA_GPIO, 1);
	gpio_direction_output(lcd_panel_reset_gpio, 1);

	spi_register_board_info(zoom_spi_board_info, ARRAY_SIZE(zoom_spi_board_info));
	platform_device_register(&zoom_dss_device);
}


