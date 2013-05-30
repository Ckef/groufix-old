/**
 * Groufix  :  Graphics Engine produced by Ckef Worx
 * www      :  http://www.ejb.ckef-worx.com
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "groufix/platform.h"

#include <X11/Xlib.h>

//******************************************************/
void* _gfx_platform_get_screen(int num)
{
	if(!_gfx_server) return NULL;

	/* Validate the number first */
	if(num < 0 || num >= ScreenCount((Display*)_gfx_server->display)) return NULL;
	return (void*)ScreenOfDisplay((Display*)_gfx_server->display, num);
}

//******************************************************/
void* _gfx_platform_get_default_screen()
{
	if(!_gfx_server) return NULL;
	return (void*)DefaultScreenOfDisplay((Display*)_gfx_server->display);
}

//******************************************************/
int _gfx_platform_get_num_screens(void)
{
	if(!_gfx_server) return 0;
	return ScreenCount((Display*)_gfx_server->display);
}

//******************************************************/
int _gfx_platform_screen_get_width(void* handle)
{
	return WidthOfScreen((Screen*)handle);
}

//******************************************************/
int _gfx_platform_screen_get_height(void* handle)
{
	return HeightOfScreen((Screen*)handle);
}
