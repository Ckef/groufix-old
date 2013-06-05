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
#include <stdlib.h>
#include <string.h>

/******************************************************/
static void _gfx_x11_add_window(void* handle)
{
	if(_gfx_x11)
	{
		++_gfx_x11->numWindows;
		_gfx_x11->windows = (void**)realloc(_gfx_x11->windows, sizeof(void*) * _gfx_x11->numWindows);
		_gfx_x11->windows[_gfx_x11->numWindows - 1] = handle;
	}
}

/******************************************************/
static void _gfx_x11_remove_window(void* handle)
{
	/* Remove the handle from the array */
	unsigned int i;
	if(_gfx_x11) for(i = 0; i < _gfx_x11->numWindows; ++i)
		if(_gfx_x11->windows[i] == handle)
	{
		--_gfx_x11->numWindows;
		if(!_gfx_x11->numWindows)
		{
			/* Free the array */
			free(_gfx_x11->windows);
			_gfx_x11->windows = NULL;
		}
		else
		{
			/* Move elements and resize array */
			void** dest = _gfx_x11->windows + i;
			memmove(dest, dest + 1, sizeof(void*) * (_gfx_x11->numWindows - i));
			_gfx_x11->windows = (void**)realloc(_gfx_x11->windows, sizeof(void*) * _gfx_x11->numWindows);
		}
		break;
	}
}

/******************************************************/
unsigned int _gfx_platform_get_num_windows(void)
{
	if(!_gfx_x11) return 0;
	return _gfx_x11->numWindows;
}

/******************************************************/
void* _gfx_platform_get_window(unsigned int num)
{
	if(!_gfx_x11) return NULL;

	/* Validate the number first */
	if(num >= _gfx_x11->numWindows) return NULL;
	return _gfx_x11->windows[num];
}

/******************************************************/
void* _gfx_platform_create_window(const GFX_Platform_Attributes* attributes)
{
	if(!_gfx_x11) return NULL;

	/* Create the window's attributes */
	XSetWindowAttributes attr;
	attr.event_mask = NoEventMask;

	/* Create the actual window */
	Window* window = (Window*)malloc(sizeof(Window));
	*window = XCreateWindow(
		_gfx_x11->display,
		RootWindowOfScreen((Screen*)attributes->screen),
		attributes->x,
		attributes->y,
		attributes->width,
		attributes->height,
		0,
		CopyFromParent,
		InputOutput,
		CopyFromParent,
		CWEventMask,
		&attr
	);
	XStoreName(_gfx_x11->display, *window, attributes->name);

	/* Add window to array */
	_gfx_x11_add_window(window);

	return window;
}

/******************************************************/
void _gfx_platform_destroy_window(void* handle)
{
	if(_gfx_x11)
	{
		/* First destroy its context */
		_gfx_platform_window_destroy_context(handle);

		XDestroyWindow(_gfx_x11->display, *((Window*)handle));
		XFlush(_gfx_x11->display);

		/* Remove and free the handle */
		_gfx_x11_remove_window(handle);
		free(handle);
	}
}

/******************************************************/
void _gfx_platform_window_set_size(void* handle, unsigned int width, unsigned int height)
{
	if(_gfx_x11)
	{
		XResizeWindow(_gfx_x11->display, *((Window*)handle), width, height);
		XFlush(_gfx_x11->display);
	}
}

/******************************************************/
void _gfx_platform_window_set_position(void* handle, int x, int y)
{
	if(_gfx_x11)
	{
		XMoveWindow(_gfx_x11->display, *((Window*)handle), x, y);
		XFlush(_gfx_x11->display);
	}
}

/******************************************************/
void _gfx_platform_window_show(void* handle)
{
	if(_gfx_x11)
	{
		XMapWindow(_gfx_x11->display, *((Window*)handle));
		XFlush(_gfx_x11->display);
	}
}

/******************************************************/
void _gfx_platform_window_hide(void* handle)
{
	if(_gfx_x11)
	{
		XUnmapWindow(_gfx_x11->display, *((Window*)handle));
		XFlush(_gfx_x11->display);
	}
}
