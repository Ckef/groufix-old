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

#include "groufix/core/platform/x11.h"

/******************************************************/
unsigned int _gfx_platform_get_num_monitors(void)
{
	if(!_gfx_x11) return 0;
	return gfx_vector_get_size(&_gfx_x11->monitors);
}

/******************************************************/
GFX_PlatformMonitor _gfx_platform_get_monitor(

		unsigned int num)
{
	if(!_gfx_x11) return NULL;

	/* Validate the number first */
	if(num >= gfx_vector_get_size(&_gfx_x11->monitors)) return NULL;
	return gfx_vector_at(&_gfx_x11->monitors, num);
}

/******************************************************/
GFX_PlatformMonitor _gfx_platform_get_default_monitor(void)
{
	if(!_gfx_x11) return NULL;
	return _gfx_x11->monitors.begin;
}

/******************************************************/
void _gfx_platform_monitor_get_size(

		GFX_PlatformMonitor  handle,
		unsigned int*        width,
		unsigned int*        height)
{
	GFX_X11_Monitor* monitor = (GFX_X11_Monitor*)handle;
	*width = monitor->width;
	*height = monitor->height;
}

/******************************************************/
unsigned int _gfx_platform_monitor_get_num_modes(

		GFX_PlatformMonitor handle)
{
	return ((GFX_X11_Monitor*)handle)->numModes;
}

/******************************************************/
int _gfx_platform_monitor_get_mode(

		GFX_PlatformMonitor  handle,
		unsigned int         num,
		GFXDisplayMode*      mode)
{
	GFX_X11_Monitor* monitor = (GFX_X11_Monitor*)handle;
	if(num >= monitor->numModes) return 0;

	GFX_X11_Mode* it =
		gfx_vector_at(&_gfx_x11->modes, monitor->modes[num]);

	*mode = it->mode;
	return 1;
}
