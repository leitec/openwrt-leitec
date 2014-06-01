/*
 *  Linksys WRT160Nv2 support
 *
 *  Copyright (C) 2014 Claudio Leite <leitec@staticky.com>
 *
 *  Based on V11ST-FE support code:
 *
 *  Copyright (C) 2012 Florian Fainelli <florian@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/ethtool.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>
#include <asm/mach-ralink/ramips_eth_platform.h>

#include "devices.h"

#define WRT160NV2_GPIO_WPS_AMBER_LED       8
#define WRT160NV2_GPIO_WPS_BLUE_LED       13

#define WRT160NV2_GPIO_POWER_LED          12

#define WRT160NV2_GPIO_BUTTON_WPS	   0
#define WRT160NV2_GPIO_BUTTON_RESET        9

#define WRT160NV2_KEYS_POLL_INTERVAL	20
#define WRT160NV2_KEYS_DEBOUNCE_INTERVAL	(3 * WRT160NV2_KEYS_POLL_INTERVAL)

static struct gpio_led wrt160nv2_leds_gpio[] __initdata = {
        {
                .name           = "wrt160nv2:blue:power",
                .gpio           = WRT160NV2_GPIO_POWER_LED,
                .active_low     = 1,
        },
        {
                .name           = "wrt160nv2:amber:wps",
                .gpio           = WRT160NV2_GPIO_WPS_AMBER_LED,
                .active_low     = 1,
        },
        {
                .name           = "wrt160nv2:blue:wps",
                .gpio           = WRT160NV2_GPIO_WPS_BLUE_LED,
                .active_low     = 1,
        },
};

static struct gpio_keys_button wrt160nv2_gpio_buttons[] __initdata = {
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = WRT160NV2_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WRT160NV2_GPIO_BUTTON_WPS,
                .active_low     = 1,
	},
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WRT160NV2_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WRT160NV2_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}
};

static void __init rt_wrt160nv2_init(void)
{
        u32 t;

	rt288x_gpio_init(
                RT2880_GPIO_MODE_I2C |
                RT2880_GPIO_MODE_UART0 |
                RT2880_GPIO_MODE_SPI |
                RT2880_GPIO_MODE_PCI);

	rt288x_register_flash(0);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(wrt160nv2_leds_gpio),
				  wrt160nv2_leds_gpio);

	ramips_register_gpio_buttons(-1, WRT160NV2_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(wrt160nv2_gpio_buttons),
				     wrt160nv2_gpio_buttons);

        /*
         * Enable GPIOs 8, 10, 13 according to Gemtek
         * GPL sources (Linksys WRT110)
         *
         * Clear bit 6 to <do something>
         */
        t = rt288x_sysc_rr(SYSC_REG_SYSTEM_CONFIG);
        t &= ~BIT(6);
        rt288x_sysc_wr(t, SYSC_REG_SYSTEM_CONFIG);

	rt288x_register_wifi();

	/* Board is connected to a RTL8306 Fast Ethernet switch */
	rt288x_eth_data.speed = SPEED_100;
	rt288x_eth_data.duplex = DUPLEX_FULL;
	rt288x_eth_data.tx_fc = 1;
	rt288x_eth_data.rx_fc = 1;
	rt288x_eth_data.phy_mask = BIT(0);
	rt288x_register_ethernet();

	rt288x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_WRT160NV2, "WRT160NV2", "Linksys WRT160Nv2", rt_wrt160nv2_init);
