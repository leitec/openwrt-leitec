/*
 *  Airlink101 AR725W support
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

#define AR725W_GPIO_WPS_RED_LED         8
#define AR725W_GPIO_WPS_BLUE_LED        13

static struct gpio_led ar725w_leds_gpio[] __initdata = {
	{
		.name		= "ar725w:red:wps",
		.gpio		= AR725W_GPIO_WPS_RED_LED,
                .active_low     = 0,
	},
        {
                .name           = "ar725w:blue:wps",
                .gpio           = AR725W_GPIO_WPS_BLUE_LED,
                .active_low     = 0,
        }
};

static void __init rt_ar725w_init(void)
{
	rt288x_gpio_init(RT2880_GPIO_MODE_UART0 | RT2880_GPIO_MODE_I2C |
                            RT2880_GPIO_MODE_SPI);

	rt288x_register_flash(0);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(ar725w_leds_gpio),
				  ar725w_leds_gpio);

#if 0
	ramips_register_gpio_buttons(-1, V11ST_FE_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(v11st_fe_gpio_buttons),
				     v11st_fe_gpio_buttons);
#endif

	rt288x_register_wifi();

	/* Board is connected to an IC+ IP175C Fast Ethernet switch */
	rt288x_eth_data.speed = SPEED_100;
	rt288x_eth_data.duplex = DUPLEX_FULL;
	rt288x_eth_data.tx_fc = 1;
	rt288x_eth_data.rx_fc = 1;
	rt288x_eth_data.phy_mask = BIT(0);
	rt288x_register_ethernet();

	rt288x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_AR725W, "AR725W", "Airlink101 AR725W", rt_ar725w_init);
