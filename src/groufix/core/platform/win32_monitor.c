/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/platform/win32.h"
#include "groufix/core/internal.h"

/******************************************************/
unsigned int _gfx_platform_get_num_monitors(void)
{
	return gfx_vector_get_size(&_gfx_win32.monitors);
}

/******************************************************/
GFX_PlatformMonitor _gfx_platform_get_monitor(

		unsigned int num)
{
	/* Validate the number first */
	if(num >= gfx_vector_get_size(&_gfx_win32.monitors)) return NULL;
	return gfx_vector_at(&_gfx_win32.monitors, num);
}

/******************************************************/
GFX_PlatformMonitor _gfx_platform_get_default_monitor(void)
{
	return _gfx_win32.monitors.begin;
}

/******************************************************/
void _gfx_platform_monitor_get_size(

		GFX_PlatformMonitor  handle,
		unsigned int*        width,
		unsigned int*        height)
{
	GFX_Win32_Monitor* monitor = (GFX_Win32_Monitor*)handle;
	*width = monitor->width;
	*height = monitor->height;
}

/******************************************************/
unsigned int _gfx_platform_monitor_get_num_modes(

		GFX_PlatformMonitor handle)
{
	return ((GFX_Win32_Monitor*)handle)->numModes;
}

/******************************************************/
int _gfx_platform_monitor_get_mode(

		GFX_PlatformMonitor  handle,
		unsigned int         num,
		GFXDisplayMode*      mode)
{
	GFX_Win32_Monitor* monitor = (GFX_Win32_Monitor*)handle;
	if(num >= monitor->numModes) return 0;

	DEVMODE* dev =
		gfx_vector_at(&_gfx_win32.modes, monitor->modes + num);

	mode->width   = dev->dmPelsWidth;
	mode->height  = dev->dmPelsHeight;
	mode->refresh = dev->dmDisplayFrequency;

	/* Split depth */
	_gfx_split_depth(
		dev->dmBitsPerPel,
		&mode->depth.redBits,
		&mode->depth.greenBits,
		&mode->depth.blueBits
	);

	return 1;
}
