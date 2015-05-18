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
unsigned int _gfx_platform_get_num_screens(void)
{
	if(!_gfx_x11) return 0;
	return ScreenCount(_gfx_x11->display);
}

/******************************************************/
GFX_PlatformScreen _gfx_platform_get_screen(

		unsigned int num)
{
	if(!_gfx_x11) return NULL;

	/* Validate the number first */
	if(num >= ScreenCount(_gfx_x11->display)) return NULL;
	return ScreenOfDisplay(_gfx_x11->display, num);
}

/******************************************************/
GFX_PlatformScreen _gfx_platform_get_default_screen(void)
{
	if(!_gfx_x11) return NULL;
	return DefaultScreenOfDisplay(_gfx_x11->display);
}

/******************************************************/
void _gfx_platform_screen_get_size(

		GFX_PlatformScreen  handle,
		unsigned int*       width,
		unsigned int*       height)
{
	*width = WidthOfScreen((Screen*)handle);
	*height = HeightOfScreen((Screen*)handle);
}

/******************************************************/
unsigned int _gfx_platform_screen_get_num_modes(

		GFX_PlatformScreen handle)
{
	return 0;
}

/******************************************************/
int _gfx_platform_screen_get_mode(

		GFX_PlatformScreen  handle,
		unsigned int        num,
		GFXDisplayMode*     mode)
{
	return 0;
}
