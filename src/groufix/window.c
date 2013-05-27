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

#include "groufix/window.h"

#include <stdlib.h>

GFXWindow* gfx_create_window()
{
	/* Open actual window */
	GFXWindow* window = (GFXWindow*)calloc(1, sizeof(GFXWindow));

	window->handle = _gfx_platform_create_window();
	_gfx_platform_create_context(window->handle);

	return window;
}

void gfx_destroy_window(GFXWindow* window)
{
	_gfx_platform_destroy_context(window->handle);
	_gfx_platform_destroy_window(window->handle);

	free(window);
}
