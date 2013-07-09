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
#include "groufix/containers/vector.h"
#include "groufix/errors.h"

#include <stdlib.h>

/******************************************************/
/* Created windows */
static Vector* _gfx_windows = NULL;

/* OpenGL context request */
static GFXContext _gfx_context = {
	GFX_CONTEXT_MAJOR_MIN,
	GFX_CONTEXT_MINOR_MIN
};

/******************************************************/
static int _gfx_window_insert(const GFX_Internal_Window* window)
{
	/* Create vector if it doesn't exist yet */
	if(!_gfx_windows)
	{
		_gfx_windows = vector_create(sizeof(GFX_Internal_Window*));
		if(!_gfx_windows) return 0;
	}
	return vector_insert(_gfx_windows, &window, _gfx_windows->end) != _gfx_windows->end;
}

/******************************************************/
static int _gfx_window_compare(const VectorIterator it, const void* value)
{
	return (*(GFX_Internal_Window**)it)->handle == value;
}

/******************************************************/
GFX_Internal_Window* _gfx_get_window_from_handle(GFX_Platform_Window handle)
{
	if(!_gfx_windows) return NULL;

	VectorIterator found = vector_find(_gfx_windows, handle, _gfx_window_compare);
	if(found != _gfx_windows->end) return *(GFX_Internal_Window**)found;

	return NULL;
}

/******************************************************/
static int _gfx_window_create_context(GFX_Platform_Window window)
{
	/* Get a window to share with (any, as all windows will share everything) */
	GFX_Platform_Window* share = NULL;
	if(_gfx_windows) share = (*(GFX_Internal_Window**)_gfx_windows->begin)->handle;

	/* Get maximum context */
	GFXContext max = {
		GFX_CONTEXT_MAJOR_MAX,
		GFX_CONTEXT_MINOR_MAX
	};

	/* Iterate from max to min untill a valid context was created */
	while(
		max.major > _gfx_context.major ||
		(max.major == _gfx_context.major &&
		 max.minor >= _gfx_context.minor))
	{
		/* Try to create it */
		if(_gfx_platform_create_context(window, max.major, max.minor, share)) return 1;

		/* Previous version */
		if(!max.minor)
		{
			--max.major;
			max.minor = GFX_CONTEXT_ALL_MINORS_MAX;
		}
		else --max.minor;
	}

	/* Nope. */
	gfx_errors_push(GFX_ERROR_INCOMPATIBLE_CONTEXT, NULL);

	return 0;
}

/******************************************************/
void gfx_request_context(GFXContext context)
{
	/* Get minimal context */
	if(context.major < GFX_CONTEXT_MAJOR_MIN)
	{
		context.major = GFX_CONTEXT_MAJOR_MIN;
		context.minor = GFX_CONTEXT_MINOR_MIN;
	}
	else if(
		context.minor < GFX_CONTEXT_MINOR_MIN &&
		context.major == GFX_CONTEXT_MAJOR_MIN)
	{
		context.minor = GFX_CONTEXT_MINOR_MIN;
	}

	_gfx_context = context;
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
	return *(GFXWindow**)vector_at(_gfx_windows, num);
}

/******************************************************/
GFXWindow* gfx_window_create(GFXScreen* screen, const GFXColorDepth* depth, const char* name, unsigned int width, unsigned int height, int x, int y)
{
	/* Setup top level window */
	GFX_Internal_Window* window = (GFX_Internal_Window*)calloc(1, sizeof(GFX_Internal_Window));
	if(!window) return NULL;

	/* Get screen */
	GFX_Platform_Screen scr;
	if(screen) scr = (GFX_Platform_Screen)(*screen);
	else scr = _gfx_platform_get_default_screen();

	/* Create platform window */
	GFX_Platform_Attributes attr;
	attr.screen    = scr;
	attr.name      = name;
	attr.width     = width;
	attr.height    = height;
	attr.x         = x;
	attr.y         = y;
	attr.depth     = *depth;

	window->handle = _gfx_platform_create_window(&attr);
	if(!window->handle)
	{
		free(window);
		return NULL;
	}

	/* Create context and insert in the vector */
	if(!_gfx_window_create_context(window->handle) || !_gfx_window_insert(window))
	{
		_gfx_platform_destroy_window(window->handle);
		free(window);

		return NULL;
	}

	/* Make the window visible */
	_gfx_platform_window_show(window->handle);

	return (GFXWindow*)window;
}

/******************************************************/
void gfx_window_free(GFXWindow* window)
{
	if(window)
	{
		/* Destroy window */
		GFX_Internal_Window* internal = (GFX_Internal_Window*)window;
		_gfx_platform_destroy_window(internal->handle);

		/* Remove the window from storage */
		vector_erase(_gfx_windows, vector_find(_gfx_windows, internal->handle, _gfx_window_compare));
		if(!vector_get_size(_gfx_windows))
		{
			vector_free(_gfx_windows);
			_gfx_windows = NULL;
		}
		free(internal);
	}
}

/******************************************************/
GFXScreen gfx_window_get_screen(GFXWindow* window)
{
	return (GFXScreen)_gfx_platform_window_get_screen(((GFX_Internal_Window*)window)->handle);
}

/******************************************************/
GFXContext gfx_window_get_context(GFXWindow* window)
{
	GFXContext context;
	context.major = 0;
	context.minor = 0;
	_gfx_platform_context_get(((GFX_Internal_Window*)window)->handle, &context.major, &context.minor);

	return context;
}

/******************************************************/
char* gfx_window_get_name(GFXWindow* window)
{
	return _gfx_platform_window_get_name(((GFX_Internal_Window*)window)->handle);
}

/******************************************************/
void gfx_window_get_size(GFXWindow* window, unsigned int* width, unsigned int* height)
{
	_gfx_platform_window_get_size(((GFX_Internal_Window*)window)->handle, width, height);
}

/******************************************************/
void gfx_window_get_position(GFXWindow* window, int* x, int* y)
{
	_gfx_platform_window_get_position(((GFX_Internal_Window*)window)->handle, x, y);
}

/******************************************************/
void gfx_window_set_name(GFXWindow* window, const char* name)
{
	_gfx_platform_window_set_name(((GFX_Internal_Window*)window)->handle, name);
}

/******************************************************/
void gfx_window_set_size(GFXWindow* window, unsigned int width, unsigned int height)
{
	_gfx_platform_window_set_size(((GFX_Internal_Window*)window)->handle, width, height);
}

/******************************************************/
void gfx_window_set_position(GFXWindow* window, int x, int y)
{
	_gfx_platform_window_set_position(((GFX_Internal_Window*)window)->handle, x, y);
}

/******************************************************/
void gfx_window_show(GFXWindow* window)
{
	_gfx_platform_window_show(((GFX_Internal_Window*)window)->handle);
}

/******************************************************/
void gfx_window_hide(GFXWindow* window)
{
	_gfx_platform_window_hide(((GFX_Internal_Window*)window)->handle);
}

/******************************************************/
void gfx_window_swap_buffers(GFXWindow* window)
{
	_gfx_platform_context_swap_buffers(((GFX_Internal_Window*)window)->handle);
}
