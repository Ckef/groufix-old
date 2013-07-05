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

/******************************************************/
unsigned int gfx_get_num_screens(void)
{
	return _gfx_platform_get_num_screens();
}

/******************************************************/
GFXScreen gfx_get_screen(unsigned int num)
{
	return (GFXScreen)_gfx_platform_get_screen(num);
}

/******************************************************/
GFXScreen gfx_get_default_screen(void)
{
	return (GFXScreen)_gfx_platform_get_default_screen();
}

/******************************************************/
void gfx_screen_get_size(GFXScreen screen, unsigned int* width, unsigned int* height)
{
	_gfx_platform_screen_get_size((GFX_Platform_Screen)screen, width, height);
}
