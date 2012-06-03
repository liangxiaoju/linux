/* linux/arch/arm/mach-s3c64xx/mach-mini6410.c
 *
 * Copyright 2010 Darius Augulis <augulis.darius@gmail.com>
 * Copyright 2008 Openmoko, Inc.
 * Copyright 2008 Simtec Electronics
 *	Ben Dooks <ben@simtec.co.uk>
 *	http://armlinux.simtec.co.uk/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
*/

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/fb.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/dm9000.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/serial_core.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>
#include <linux/leds.h>

#include <asm/hardware/vic.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include <mach/map.h>
#include <mach/regs-gpio.h>
#include <mach/regs-modem.h>
#include <mach/regs-srom.h>

#include <plat/adc.h>
#include <plat/cpu.h>
#include <plat/devs.h>
#include <plat/fb.h>
#include <plat/nand.h>
#include <plat/regs-serial.h>
#include <plat/ts.h>
#include <plat/regs-fb-v4.h>
#include <plat/sdhci.h>
#include <plat/udc-hs.h>
#include <plat/audio.h>

#include <video/platform_lcd.h>

#include "common.h"

#define UCON S3C2410_UCON_DEFAULT
#define ULCON (S3C2410_LCON_CS8 | S3C2410_LCON_PNONE | S3C2410_LCON_STOPB)
#define UFCON (S3C2410_UFCON_RXTRIG8 | S3C2410_UFCON_FIFOMODE)

static struct s3c2410_uartcfg mini6410_uartcfgs[] __initdata = {
	[0] = {
		.hwport	= 0,
		.flags	= 0,
		.ucon	= UCON,
		.ulcon	= ULCON,
		.ufcon	= UFCON,
	},
	[1] = {
		.hwport	= 1,
		.flags	= 0,
		.ucon	= UCON,
		.ulcon	= ULCON,
		.ufcon	= UFCON,
	},
	[2] = {
		.hwport	= 2,
		.flags	= 0,
		.ucon	= UCON,
		.ulcon	= ULCON,
		.ufcon	= UFCON,
	},
	[3] = {
		.hwport	= 3,
		.flags	= 0,
		.ucon	= UCON,
		.ulcon	= ULCON,
		.ufcon	= UFCON,
	},
};

/* DM9000AEP 10/100 ethernet controller */

static struct resource mini6410_dm9k_resource[] = {
	[0] = {
		.start	= S3C64XX_PA_XM0CSN1,
		.end	= S3C64XX_PA_XM0CSN1 + 1,
		.flags	= IORESOURCE_MEM
	},
	[1] = {
		.start	= S3C64XX_PA_XM0CSN1 + 4,
		.end	= S3C64XX_PA_XM0CSN1 + 5,
		.flags	= IORESOURCE_MEM
	},
	[2] = {
		.start	= S3C_EINT(7),
		.end	= S3C_EINT(7),
		.flags	= IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL
	}
};

static struct dm9000_plat_data mini6410_dm9k_pdata = {
	.flags		= (DM9000_PLATF_16BITONLY | DM9000_PLATF_NO_EEPROM),
};

static struct platform_device mini6410_device_eth = {
	.name		= "dm9000",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(mini6410_dm9k_resource),
	.resource	= mini6410_dm9k_resource,
	.dev		= {
		.platform_data	= &mini6410_dm9k_pdata,
	},
};

static struct mtd_partition mini6410_nand_part[] = {
	[0] = {
		.name	= "uboot",
		.size	= SZ_1M,
		.offset	= 0,
	},
	[1] = {
		.name	= "kernel",
		.size	= SZ_2M,
		.offset	= SZ_1M,
	},
	[2] = {
		.name	= "rootfs",
		.size	= MTDPART_SIZ_FULL,
		.offset	= SZ_1M + SZ_2M,
	},
};

static struct s3c2410_nand_set mini6410_nand_sets[] = {
	[0] = {
		.name		= "nand",
		.nr_chips	= 1,
		.nr_partitions	= ARRAY_SIZE(mini6410_nand_part),
		.partitions	= mini6410_nand_part,
	},
};

static struct s3c2410_platform_nand mini6410_nand_info = {
	.tacls		= 25,
	.twrph0		= 55,
	.twrph1		= 40,
	.nr_sets	= ARRAY_SIZE(mini6410_nand_sets),
	.sets		= mini6410_nand_sets,
};

static struct s3c_fb_pd_win mini6410_fb_win[] = {
	{
		.win_mode	= {	/* 4.3" 480x272 */
			.left_margin	= 3,
			.right_margin	= 2,
			.upper_margin	= 1,
			.lower_margin	= 1,
			.hsync_len	= 40,
			.vsync_len	= 1,
			.xres		= 480,
			.yres		= 272,
		},
		.max_bpp	= 32,
		.default_bpp	= 16,
	}, {
		.win_mode	= {	/* 7.0" 800x480 */
			.left_margin	= 8,
			.right_margin	= 13,
			.upper_margin	= 7,
			.lower_margin	= 5,
			.hsync_len	= 3,
			.vsync_len	= 1,
			.xres		= 800,
			.yres		= 480,
		},
		.max_bpp	= 32,
		.default_bpp	= 16,
	},
};

static struct s3c_fb_platdata mini6410_lcd_pdata __initdata = {
	.setup_gpio	= s3c64xx_fb_gpio_setup_24bpp,
	.win[0]		= &mini6410_fb_win[0],
	.vidcon0	= VIDCON0_VIDOUT_RGB | VIDCON0_PNRMODE_RGB,
	.vidcon1	= VIDCON1_INV_HSYNC | VIDCON1_INV_VSYNC,
};

static void mini6410_lcd_power_set(struct plat_lcd_data *pd,
				   unsigned int power)
{
	if (power)
		gpio_direction_output(S3C64XX_GPE(0), 1);
	else
		gpio_direction_output(S3C64XX_GPE(0), 0);
}

static struct plat_lcd_data mini6410_lcd_power_data = {
	.set_power	= mini6410_lcd_power_set,
};

static struct platform_device mini6410_lcd_powerdev = {
	.name			= "platform-lcd",
	.dev.parent		= &s3c_device_fb.dev,
	.dev.platform_data	= &mini6410_lcd_power_data,
};

#define GPIO_KEYS_BUTTON(_code,_gpio,_desc,_type,_wakeup,_active_low,_debounce_interval)	\
	{												\
		.code				= _code,				\
		.gpio				= _gpio,				\
		.desc				= _desc,				\
		.type				= _type,				\
		.wakeup				= _wakeup,				\
		.active_low			= _active_low,			\
		.debounce_interval	= _debounce_interval,	\
	}

static struct gpio_keys_button mini6410_gpio_keys_tables[] = {
	GPIO_KEYS_BUTTON(KEY_UP, S3C64XX_GPN(5), "UP", EV_KEY, 0, 1, 50),
	GPIO_KEYS_BUTTON(KEY_DOWN, S3C64XX_GPN(4), "DOWN", EV_KEY, 0, 1, 50),
	GPIO_KEYS_BUTTON(KEY_LEFT, S3C64XX_GPN(3), "LEFT", EV_KEY, 0, 1, 50),
	GPIO_KEYS_BUTTON(KEY_RIGHT, S3C64XX_GPN(2), "RIGHT", EV_KEY, 0, 1, 50),
	GPIO_KEYS_BUTTON(KEY_ENTER, S3C64XX_GPN(1), "ENTER", EV_KEY, 0, 1, 50),
	GPIO_KEYS_BUTTON(KEY_BACK, S3C64XX_GPN(0), "BACK", EV_KEY, 0, 1, 50),
	GPIO_KEYS_BUTTON(KEY_HOME, S3C64XX_GPL(11), "HOME", EV_KEY, 0, 1, 50),
	GPIO_KEYS_BUTTON(KEY_MENU, S3C64XX_GPL(12), "MENU", EV_KEY, 0, 1, 50),
};

static struct gpio_keys_platform_data mini6410_gpio_keys_data = {
	.buttons	= mini6410_gpio_keys_tables,
	.nbuttons	= ARRAY_SIZE(mini6410_gpio_keys_tables),
};

static struct platform_device mini6410_device_gpio_keys = {
	.name		= "gpio-keys",
	.dev		= {
		.platform_data	= &mini6410_gpio_keys_data,
	},
};

#define GPIO_LED(_name, _default_trigger, _gpio, _active_low, _retain_state_suspend, _default_state)		\
	{														\
		.name					= _name,					\
		.default_trigger		= _default_trigger,			\
		.gpio					= _gpio,					\
		.active_low				= _active_low,				\
		.retain_state_suspended	= _retain_state_suspend,	\
		.default_state			= _default_state,			\
	}

static struct gpio_led mini6410_gpio_leds[] = {
	GPIO_LED("led-1", "none", S3C64XX_GPK(4), 1, 0, LEDS_GPIO_DEFSTATE_OFF),
	GPIO_LED("led-2", "none", S3C64XX_GPK(5), 1, 0, LEDS_GPIO_DEFSTATE_OFF),
	GPIO_LED("led-3", "none", S3C64XX_GPK(6), 1, 0, LEDS_GPIO_DEFSTATE_OFF),
	GPIO_LED("led-4", "none", S3C64XX_GPK(7), 1, 0, LEDS_GPIO_DEFSTATE_OFF),
};

static struct gpio_led_platform_data mini6410_gpio_leds_pdata = {
	.leds		= mini6410_gpio_leds,
	.num_leds	= ARRAY_SIZE(mini6410_gpio_leds),
};

static struct platform_device mini6410_device_gpio_leds = {
	.name		= "leds-gpio",
	.dev		= {
		.platform_data	= &mini6410_gpio_leds_pdata,
	},
};

static struct platform_device *mini6410_devices[] __initdata = {
	&mini6410_device_eth,
	&s3c_device_hsmmc0,
	&s3c_device_hsmmc1,
	&s3c_device_ohci,
	&s3c_device_usb_hsotg,
	&s3c_device_nand,
	&s3c_device_fb,
	&mini6410_lcd_powerdev,
	&s3c_device_adc,
	&s3c_device_ts,
	&mini6410_device_gpio_keys,
	&mini6410_device_gpio_leds,
	&samsung_asoc_dma,
	&s3c64xx_device_ac97,
};

static void __init mini6410_map_io(void)
{
	u32 tmp;

	s3c64xx_init_io(NULL, 0);
	s3c24xx_init_clocks(12000000);
	s3c24xx_init_uarts(mini6410_uartcfgs, ARRAY_SIZE(mini6410_uartcfgs));

	/* set the LCD type */
	tmp = __raw_readl(S3C64XX_SPCON);
	tmp &= ~S3C64XX_SPCON_LCD_SEL_MASK;
	tmp |= S3C64XX_SPCON_LCD_SEL_RGB;
	__raw_writel(tmp, S3C64XX_SPCON);

	/* remove the LCD bypass */
	tmp = __raw_readl(S3C64XX_MODEM_MIFPCON);
	tmp &= ~MIFPCON_LCD_BYPASS;
	__raw_writel(tmp, S3C64XX_MODEM_MIFPCON);
}

/*
 * mini6410_features string
 *
 * 0-9 LCD configuration
 *
 */
static char mini6410_features_str[12] __initdata = "0";

static int __init mini6410_features_setup(char *str)
{
	if (str)
		strlcpy(mini6410_features_str, str,
			sizeof(mini6410_features_str));
	return 1;
}

__setup("mini6410=", mini6410_features_setup);

#define FEATURE_SCREEN (1 << 0)

struct mini6410_features_t {
	int done;
	int lcd_index;
};

static void mini6410_parse_features(
		struct mini6410_features_t *features,
		const char *features_str)
{
	const char *fp = features_str;

	features->done = 0;
	features->lcd_index = 0;

	while (*fp) {
		char f = *fp++;

		switch (f) {
		case '0'...'9':	/* tft screen */
			if (features->done & FEATURE_SCREEN) {
				printk(KERN_INFO "MINI6410: '%c' ignored, "
					"screen type already set\n", f);
			} else {
				int li = f - '0';
				if (li >= ARRAY_SIZE(mini6410_fb_win))
					printk(KERN_INFO "MINI6410: '%c' out "
						"of range LCD mode\n", f);
				else {
					features->lcd_index = li;
				}
			}
			features->done |= FEATURE_SCREEN;
			break;
		}
	}
}

static struct s3c_sdhci_platdata sdhci0_pdata = {
	.cd_type		= S3C_SDHCI_CD_INTERNAL,
};

static struct s3c_sdhci_platdata sdhci1_pdata = {
	.cd_type		= S3C_SDHCI_CD_GPIO,
	.ext_cd_gpio	= S3C64XX_GPN(10),
};

static struct s3c_hsotg_plat mini6410_hsotg_pdata;

static void __init mini6410_machine_init(void)
{
	u32 cs1;
	struct mini6410_features_t features = { 0 };

	printk(KERN_INFO "MINI6410: Option string mini6410=%s\n",
			mini6410_features_str);

	/* Parse the feature string */
	mini6410_parse_features(&features, mini6410_features_str);

	mini6410_lcd_pdata.win[0] = &mini6410_fb_win[features.lcd_index];

	printk(KERN_INFO "MINI6410: selected LCD display is %dx%d\n",
		mini6410_lcd_pdata.win[0]->win_mode.xres,
		mini6410_lcd_pdata.win[0]->win_mode.yres);

	s3c_nand_set_platdata(&mini6410_nand_info);
	s3c_fb_set_platdata(&mini6410_lcd_pdata);
	s3c24xx_ts_set_platdata(NULL);
	s3c_sdhci0_set_platdata(&sdhci0_pdata);
	s3c_sdhci1_set_platdata(&sdhci1_pdata);
	s3c_hsotg_set_platdata(&mini6410_hsotg_pdata);
	s3c64xx_ac97_setup_gpio(0);

	/* configure nCS1 width to 16 bits */

	cs1 = __raw_readl(S3C64XX_SROM_BW) &
		~(S3C64XX_SROM_BW__CS_MASK << S3C64XX_SROM_BW__NCS1__SHIFT);
	cs1 |= ((1 << S3C64XX_SROM_BW__DATAWIDTH__SHIFT) |
		(1 << S3C64XX_SROM_BW__WAITENABLE__SHIFT) |
		(1 << S3C64XX_SROM_BW__BYTEENABLE__SHIFT)) <<
			S3C64XX_SROM_BW__NCS1__SHIFT;
	__raw_writel(cs1, S3C64XX_SROM_BW);

	/* set timing for nCS1 suitable for ethernet chip */

	__raw_writel((0 << S3C64XX_SROM_BCX__PMC__SHIFT) |
		(6 << S3C64XX_SROM_BCX__TACP__SHIFT) |
		(4 << S3C64XX_SROM_BCX__TCAH__SHIFT) |
		(1 << S3C64XX_SROM_BCX__TCOH__SHIFT) |
		(13 << S3C64XX_SROM_BCX__TACC__SHIFT) |
		(4 << S3C64XX_SROM_BCX__TCOS__SHIFT) |
		(0 << S3C64XX_SROM_BCX__TACS__SHIFT), S3C64XX_SROM_BC1);

	gpio_request(S3C64XX_GPF(15), "LCD power");
	gpio_request(S3C64XX_GPE(0), "LCD power");

	platform_add_devices(mini6410_devices, ARRAY_SIZE(mini6410_devices));
}

MACHINE_START(MINI6410, "MINI6410")
	/* Maintainer: Darius Augulis <augulis.darius@gmail.com> */
	.atag_offset	= 0x100,
	.init_irq	= s3c6410_init_irq,
	.handle_irq	= vic_handle_irq,
	.map_io		= mini6410_map_io,
	.init_machine	= mini6410_machine_init,
	.timer		= &s3c24xx_timer,
	.restart	= s3c64xx_restart,
MACHINE_END
