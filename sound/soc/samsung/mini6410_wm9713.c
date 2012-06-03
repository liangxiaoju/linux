/*
 * mini6410_wm9713.c  --  SoC audio for MINI6410
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 */

#include <linux/module.h>
#include <sound/soc.h>

static struct snd_soc_card mini6410;

/*
 * Default CFG switch settings to use this driver:
 *
 *   SMDK6410: Set CFG1 1-3 On, CFG2 1-4 Off
 *   SMDKC100: Set CFG6 1-3 On, CFG7 1   On
 *   SMDKC110: Set CFGB10 1-2 Off, CFGB12 1-3 On
 *   SMDKV210: Set CFGB10 1-2 Off, CFGB12 1-3 On
 *   SMDKV310: Set CFG2 1-2 Off, CFG4 All On, CFG7 All Off, CFG8 1-On
 */

/*
 Playback (HeadPhone):-
	$ amixer sset 'Headphone' unmute
	$ amixer sset 'Right Headphone Out Mux' 'Headphone'
	$ amixer sset 'Left Headphone Out Mux' 'Headphone'
	$ amixer sset 'Right HP Mixer PCM' unmute
	$ amixer sset 'Left HP Mixer PCM' unmute

 Capture (LineIn):-
	$ amixer sset 'Right Capture Source' 'Line'
	$ amixer sset 'Left Capture Source' 'Line'
*/

static struct snd_soc_dai_link mini6410_dai = {
	.name = "AC97",
	.stream_name = "AC97 PCM",
	.platform_name = "samsung-audio",
	.cpu_dai_name = "samsung-ac97",
	.codec_dai_name = "wm9713-hifi",
	.codec_name = "wm9713-codec",
};

static struct snd_soc_card mini6410 = {
	.name = "MINI6410 WM9713",
	.owner = THIS_MODULE,
	.dai_link = &mini6410_dai,
	.num_links = 1,
};

static struct platform_device *mini6410_snd_wm9713_device;
static struct platform_device *mini6410_snd_ac97_device;

static int __init mini6410_init(void)
{
	int ret;

	mini6410_snd_wm9713_device = platform_device_alloc("wm9713-codec", -1);
	if (!mini6410_snd_wm9713_device)
		return -ENOMEM;

	ret = platform_device_add(mini6410_snd_wm9713_device);
	if (ret)
		goto err1;

	mini6410_snd_ac97_device = platform_device_alloc("soc-audio", -1);
	if (!mini6410_snd_ac97_device) {
		ret = -ENOMEM;
		goto err2;
	}

	platform_set_drvdata(mini6410_snd_ac97_device, &mini6410);

	ret = platform_device_add(mini6410_snd_ac97_device);
	if (ret)
		goto err3;

	return 0;

err3:
	platform_device_put(mini6410_snd_ac97_device);
err2:
	platform_device_del(mini6410_snd_wm9713_device);
err1:
	platform_device_put(mini6410_snd_wm9713_device);
	return ret;
}

static void __exit mini6410_exit(void)
{
	platform_device_unregister(mini6410_snd_ac97_device);
	platform_device_unregister(mini6410_snd_wm9713_device);
}

module_init(mini6410_init);
module_exit(mini6410_exit);

/* Module information */
MODULE_DESCRIPTION("ALSA SoC MINI6410+WM9713");
MODULE_LICENSE("GPL");
