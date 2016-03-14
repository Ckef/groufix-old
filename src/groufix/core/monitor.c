/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/platform.h"

/******************************************************/
unsigned int gfx_get_num_monitors(void)
{
	return _gfx_platform_get_num_monitors();
}

/******************************************************/
GFXMonitor gfx_get_monitor(

		unsigned int num)
{
	return (GFXMonitor)_gfx_platform_get_monitor(num);
}

/******************************************************/
GFXMonitor gfx_get_default_monitor(void)
{
	return (GFXMonitor)_gfx_platform_get_default_monitor();
}

/******************************************************/
void gfx_monitor_get_size(

		GFXMonitor     monitor,
		unsigned int  *width,
		unsigned int  *height)
{
	monitor = monitor ? monitor :
		(GFXMonitor)_gfx_platform_get_default_monitor();

	if(monitor) _gfx_platform_monitor_get_size(
		(GFX_PlatformMonitor)monitor,
		width,
		height);
}

/******************************************************/
unsigned int gfx_monitor_get_num_modes(

		GFXMonitor monitor)
{
	monitor = monitor ? monitor :
		(GFXMonitor)_gfx_platform_get_default_monitor();

	if(!monitor)
		return 0;

	return _gfx_platform_monitor_get_num_modes(
		(GFX_PlatformMonitor)monitor);
}

/******************************************************/
int gfx_monitor_get_mode(

		GFXMonitor       monitor,
		unsigned int     num,
		GFXDisplayMode  *mode)
{
	monitor = monitor ? monitor :
		(GFXMonitor)_gfx_platform_get_default_monitor();

	if(!monitor)
		return 0;

	return _gfx_platform_monitor_get_mode(
		(GFX_PlatformMonitor)monitor,
		num,
		mode);
}
