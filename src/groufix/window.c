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
#include <string.h>

/******************************************************/
/* Window data actually stored */
typedef struct GFX_Window_Data
{
	GFX_Platform_Window  handle;
	GFXWindow*           window;

} GFX_Window_Data;

/* Created windows */
static Vector* _gfx_windows = NULL;

/* OpenGL context request */
static GFXContext _gfx_context = {
	GFX_CONTEXT_MAJOR_MIN,
	GFX_CONTEXT_MINOR_MIN
};

/******************************************************/
static int _gfx_window_insert(const GFX_Window_Data* data)
{
	/* Create vector if it doesn't exist yet */
	if(!_gfx_windows)
	{
		_gfx_windows = vector_create(sizeof(GFX_Window_Data));
		if(!_gfx_windows) return 0;
	}
	return vector_insert(_gfx_windows, data, _gfx_windows->end) != _gfx_windows->end;
}

/******************************************************/
static int _gfx_window_compare_handle(const VectorIterator it, const void* value)
{
	return ((GFX_Window_Data*)it)->handle == value;
}

/******************************************************/
static int _gfx_window_compare_window(const VectorIterator it, const void* value)
{
	return ((GFX_Window_Data*)it)->window == value;
}

/******************************************************/
static VectorIterator _gfx_window_get_from_handle(GFX_Platform_Window handle)
{
	if(!_gfx_windows) return NULL;

	VectorIterator found = vector_find(_gfx_windows, handle, _gfx_window_compare_handle);
	if(found != _gfx_windows->end) return found;

	return NULL;
}

/******************************************************/
static GFX_Window_Data* _gfx_window_get_from_window(GFXWindow* window)
{
	if(!_gfx_windows) return NULL;

	VectorIterator found = vector_find(_gfx_windows, window, _gfx_window_compare_window);
	if(found != _gfx_windows->end) return found;

	return NULL;
}

/******************************************************/
static int _gfx_window_create_context(GFX_Platform_Window window)
{
	/* Get a window to share with (any, as all windows will share everything) */
	GFX_Platform_Window* share = NULL;
	if(_gfx_windows) share = ((GFX_Window_Data*)_gfx_windows->begin)->handle;

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
GFXWindow* _gfx_platform_get_window_from_handle(GFX_Platform_Window handle)
{
	VectorIterator it = _gfx_window_get_from_handle(handle);
	if(!it) return NULL;

	return ((GFX_Window_Data*)it)->window;
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
	return ((GFX_Window_Data*)vector_at(_gfx_windows, num))->window;
}

/******************************************************/
GFXWindow* gfx_window_create(GFXScreen* screen, const GFXColorDepth* depth, const char* name, unsigned int width, unsigned int height, int x, int y)
{
	/* Setup data */
	GFX_Window_Data data;
	data.window = (GFXWindow*)calloc(1, sizeof(GFXWindow));
	if(!data.window) return NULL;

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

	data.handle = _gfx_platform_create_window(&attr);
	if(!data.handle)
	{
		free(data.window);
		return NULL;
	}

	/* Create context and insert in the vector */
	if(!_gfx_window_create_context(data.handle) || !_gfx_window_insert(&data))
	{
		_gfx_platform_destroy_window(data.handle);
		free(data.window);

		return NULL;
	}

	/* Make the window visible */
	_gfx_platform_window_show(data.handle);

	return data.window;
}

/******************************************************/
void gfx_window_free(GFXWindow* window)
{
	/* Get data */
	GFX_Window_Data* it = (GFX_Window_Data*)_gfx_window_get_from_window(window);
	if(it)
	{
		/* Destroy window */
		_gfx_platform_destroy_window(it->handle);
		free(it->window);

		/* Remove the window from storage */
		vector_erase(_gfx_windows, (VectorIterator)it);
		if(!vector_get_size(_gfx_windows))
		{
			vector_free(_gfx_windows);
			_gfx_windows = NULL;
		}
	}
}

/******************************************************/
GFXScreen gfx_window_get_screen(GFXWindow* window)
{
	GFX_Window_Data* data = (GFX_Window_Data*)_gfx_window_get_from_window(window);
	if(data) return (GFXScreen)_gfx_platform_window_get_screen(data->handle);

	return NULL;
}

/******************************************************/
GFXContext gfx_window_get_context(GFXWindow* window)
{
	GFXContext context;
	context.major = 0;
	context.minor = 0;

	GFX_Window_Data* data = (GFX_Window_Data*)_gfx_window_get_from_window(window);
	if(data) _gfx_platform_context_get(data->handle, &context.major, &context.minor);

	return context;
}

/******************************************************/
char* gfx_window_get_name(GFXWindow* window)
{
	GFX_Window_Data* data = (GFX_Window_Data*)_gfx_window_get_from_window(window);
	if(!data) return NULL;

	return _gfx_platform_window_get_name(data->handle);
}

/******************************************************/
void gfx_window_get_size(GFXWindow* window, unsigned int* width, unsigned int* height)
{
	GFX_Window_Data* data = (GFX_Window_Data*)_gfx_window_get_from_window(window);
	if(data) _gfx_platform_window_get_size(data->handle, width, height);
}

/******************************************************/
void gfx_window_get_position(GFXWindow* window, int* x, int* y)
{
	GFX_Window_Data* data = (GFX_Window_Data*)_gfx_window_get_from_window(window);
	if(data) _gfx_platform_window_get_position(data->handle, x, y);
}

/******************************************************/
void gfx_window_set_name(GFXWindow* window, const char* name)
{
	GFX_Window_Data* data = (GFX_Window_Data*)_gfx_window_get_from_window(window);
	if(data) _gfx_platform_window_set_name(data->handle, name);
}

/******************************************************/
void gfx_window_set_size(GFXWindow* window, unsigned int width, unsigned int height)
{
	GFX_Window_Data* data = (GFX_Window_Data*)_gfx_window_get_from_window(window);
	if(data) _gfx_platform_window_set_size(data->handle, width, height);
}

/******************************************************/
void gfx_window_set_position(GFXWindow* window, int x, int y)
{
	GFX_Window_Data* data = (GFX_Window_Data*)_gfx_window_get_from_window(window);
	if(data) _gfx_platform_window_set_position(data->handle, x, y);
}

/******************************************************/
void gfx_window_show(GFXWindow* window)
{
	GFX_Window_Data* data = (GFX_Window_Data*)_gfx_window_get_from_window(window);
	if(data) _gfx_platform_window_show(data->handle);
}

/******************************************************/
void gfx_window_hide(GFXWindow* window)
{
	GFX_Window_Data* data = (GFX_Window_Data*)_gfx_window_get_from_window(window);
	if(data) _gfx_platform_window_hide(data->handle);
}

/******************************************************/
void gfx_window_swap_buffers(GFXWindow* window)
{
	GFX_Window_Data* data = (GFX_Window_Data*)_gfx_window_get_from_window(window);
	if(data) _gfx_platform_context_swap_buffers(data->handle);
}

/******************************************************/
int _gfx_platform_is_extension_in_string(const char* str, const char* ext)
{
	/* Get extension length */
	size_t len = strlen(ext);
	if(!len) return 0;

	/* Try to find a complete match */
	char* found = strstr(str, ext);
	while(found)
	{
		char* end = found + len;
		if((found == str || *(found - 1) == ' ') && (*end == ' ' || *end == '\0'))
			return 1;

		found = strstr(end, ext);
	}

	return 0;
}
