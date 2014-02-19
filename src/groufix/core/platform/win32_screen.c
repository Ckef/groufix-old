/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Groufix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Groufix.  If not, see <http://www.gnu.org/licenses/>.
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
GFX_PlatformScreen _gfx_platform_get_screen(unsigned int num)
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
void _gfx_platform_screen_get_size(GFX_PlatformScreen handle, unsigned int* width, unsigned int* height)
{
	GFX_Win32_Screen* screen = (GFX_Win32_Screen*)handle;
	*width = screen->width;
	*height = screen->height;
}
