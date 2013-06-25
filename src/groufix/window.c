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
#include "groufix/containers/vector.h"

#include <stdlib.h>

/******************************************************/
static Vector* _gfx_windows = NULL;

/******************************************************/
static int _gfx_insert_window(GFXWindow* window)
{
	/* Create vector if it doesn't exist yet */
	if(!_gfx_windows)
	{
		_gfx_windows = vector_create(sizeof(GFXWindow*));
		if(!_gfx_windows) return 0;
	}
	return vector_insert(_gfx_windows, &window, _gfx_windows->end) != _gfx_windows->end;
}

/******************************************************/
unsigned int gfx_get_num_windows(void)
{
	if(!_gfx_windows) return 0;
	return vector_get_size(_gfx_windows);
}

/******************************************************/
GFXWindow* gfx_get_window(unsigned int num)
{
	if(num >= gfx_get_num_windows()) return NULL;
	return *((GFXWindow**)vector_at(_gfx_windows, num));
}

/******************************************************/
GFXWindow* gfx_window_create(GFXScreen* screen, GFXWindowDepth* depth, const char* name, unsigned int width, unsigned int height, int x, int y)
{
	/* Get screen */
	GFX_Platform_Screen scr;
	if(screen) scr = screen->handle;
	else scr = _gfx_platform_get_default_screen();

	/* Create platform window */
	GFX_Platform_Attributes attr;
	attr.screen    = scr;
	attr.name      = name;
	attr.width     = width;
	attr.height    = height;
	attr.x         = x;
	attr.y         = y;
	attr.redBits   = depth->redBits;
	attr.greenBits = depth->greenBits;
	attr.blueBits  = depth->blueBits;

	GFX_Platform_Window handle = _gfx_platform_create_window(&attr);
	if(!handle) return NULL;

	/* Create top level window */
	GFXWindow* window = (GFXWindow*)calloc(1, sizeof(GFXWindow));
	window->handle = handle;

	/* Create context and insert in the vector */
	if(!_gfx_platform_create_context(handle) || !_gfx_insert_window(window))
	{
		_gfx_platform_destroy_window(handle);
		free(window);

		return NULL;
	}

	/* Make the window visible */
	_gfx_platform_window_show(handle);

	return window;
}

/******************************************************/
void gfx_window_free(GFXWindow* window)
{
	/* Destroy window */
	_gfx_platform_destroy_window(window->handle);
	free(window);

	/* Remove the window from storage */
	vector_erase(_gfx_windows, vector_find(_gfx_windows, &window));
	if(!vector_get_size(_gfx_windows))
	{
		vector_free(_gfx_windows);
		_gfx_windows = NULL;
	}
}

/******************************************************/
GFXScreen gfx_window_get_screen(GFXWindow* window)
{
	GFXScreen scr;
	scr.handle = _gfx_platform_window_get_screen(window->handle);

	return scr;
}

/******************************************************/
char* gfx_window_get_name(GFXWindow* window)
{
	return _gfx_platform_window_get_name(window->handle);
}

/******************************************************/
void gfx_window_get_size(GFXWindow* window, unsigned int* width, unsigned int* height)
{
	_gfx_platform_window_get_size(window->handle, width, height);
}

/******************************************************/
void gfx_window_get_position(GFXWindow* window, int* x, int* y)
{
	_gfx_platform_window_get_position(window->handle, x, y);
}

/******************************************************/
void gfx_window_set_name(GFXWindow* window, const char* name)
{
	_gfx_platform_window_set_name(window->handle, name);
}

/******************************************************/
void gfx_window_set_size(GFXWindow* window, unsigned int width, unsigned int height)
{
	_gfx_platform_window_set_size(window->handle, width, height);
}

/******************************************************/
void gfx_window_set_position(GFXWindow* window, int x, int y)
{
	_gfx_platform_window_set_position(window->handle, x, y);
}

/******************************************************/
void gfx_window_show(GFXWindow* window)
{
	_gfx_platform_window_show(window->handle);
}

/******************************************************/
void gfx_window_hide(GFXWindow* window)
{
	_gfx_platform_window_hide(window->handle);
}
