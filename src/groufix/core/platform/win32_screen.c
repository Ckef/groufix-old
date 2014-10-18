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

/******************************************************/
unsigned int _gfx_platform_get_num_screens(void)
{
	if(!_gfx_win32) return 0;
	return gfx_vector_get_size(&_gfx_win32->screens);
}

/******************************************************/
GFX_PlatformScreen _gfx_platform_get_screen(

		unsigned int num)
{
	if(!_gfx_win32) return NULL;

	/* Validate the number first */
	if(num >= gfx_vector_get_size(&_gfx_win32->screens)) return NULL;
	return gfx_vector_at(&_gfx_win32->screens, num);
}

/******************************************************/
GFX_PlatformScreen _gfx_platform_get_default_screen(void)
{
	if(!_gfx_win32) return NULL;
	return _gfx_win32->screens.begin;
}

/******************************************************/
void _gfx_platform_screen_get_size(

		GFX_PlatformScreen  handle,
		unsigned int*       width,
		unsigned int*       height)
{
	GFX_Win32_Screen* screen = (GFX_Win32_Screen*)handle;
	*width = screen->width;
	*height = screen->height;
}
