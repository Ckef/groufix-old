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

#include "groufix/core/pipeline/internal.h"
#include "groufix/core/errors.h"

#include <stdlib.h>

/******************************************************/
/* Current window data key */
static GFX_PlatformKey _gfx_current_window;

/* Main window (main context) */
static GFX_Window* _gfx_main_window = NULL;

/* Created windows */
static GFXVector* _gfx_windows = NULL;

/* Total number of living (non-zombie) windows */
static unsigned int _gfx_alive_windows = 0;

/* Number of on-screen windows */
static unsigned int _gfx_public_windows = 0;

/* OpenGL context request */
static GFXContext _gfx_context = {
	GFX_CONTEXT_MAJOR_MIN,
	GFX_CONTEXT_MINOR_MIN
};

/******************************************************/
static int _gfx_window_context_create(

		GFX_PlatformWindow window)
{
	/* Get the main window to share with (as all windows will share everything) */
	GFX_PlatformWindow share = NULL;
	if(_gfx_windows) share = (*(GFX_Window**)_gfx_windows->begin)->handle;

	if(share == window) share = NULL;

	/* Get maximum context */
	GFXContext max = {
		GFX_CONTEXT_MAJOR_MAX,
		GFX_CONTEXT_MINOR_MAX
	};

	/* Iterate from max to min until a valid context was created */
	while(
		max.major > _gfx_context.major ||
		(max.major == _gfx_context.major &&
		max.minor >= _gfx_context.minor))
	{
		/* Try to create it */
		if(_gfx_platform_context_create(window, max.major, max.minor, share))
			return 1;

		/* Previous version */
		if(!max.minor)
		{
			--max.major;
			max.minor = GFX_CONTEXT_ALL_MINORS_MAX;
		}
		else --max.minor;
	}

	/* Nope. */
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"The requested OpenGL Context version could not be created."
	);

	return 0;
}

/******************************************************/
static GFX_Window* _gfx_window_create_common(

		GFXScreen       screen,
		GFXColorDepth   depth,
		const char*     name,
		int             x,
		int             y,
		unsigned int    w,
		unsigned int    h,
		GFXWindowFlags  flags)
{
	/* Setup top level window */
	GFX_Window* window = calloc(1, sizeof(GFX_Window));
	if(!window)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Window could not be allocated."
		);
		return NULL;
	}

	/* Create data key */
	if(!_gfx_alive_windows)
	{
		if(!_gfx_platform_key_init(&_gfx_current_window))
			return NULL;
	}

	/* Create platform window */
	GFX_PlatformAttributes attr;

	if(screen) attr.screen = (GFX_PlatformScreen)screen;
	else attr.screen = _gfx_platform_get_default_screen();

	attr.name   = name;
	attr.width  = w;
	attr.height = h;
	attr.x      = x;
	attr.y      = y;
	attr.depth  = depth;
	attr.flags  = flags;

	window->handle = _gfx_platform_window_create(&attr);

	if(!window->handle)
	{
		/* Unknown error */
		gfx_errors_push(
			GFX_ERROR_UNKNOWN,
			"Platform window could not be created."
		);
		free(window);

		if(!_gfx_alive_windows)
			_gfx_platform_key_clear(_gfx_current_window);

		return NULL;
	}

	/* Create context */
	if(_gfx_window_context_create(window->handle))
	{
		/* Up alive windows so the renderer can make use of it */
		++_gfx_alive_windows;
		_gfx_window_make_current(window);

		/* Load renderer and initialize window */
		_gfx_platform_context_get(
			&window->context.major,
			&window->context.minor
		);

		_gfx_renderer_load();
		_gfx_states_set_default(&window->state);
		_gfx_states_force_set(&window->state, window);

		/* Try to prepare the window for post processing */
		if(_gfx_pipe_process_prepare())
		{
			/* And finally initialize the render object container */
			_gfx_render_objects_init(&window->objects);

			return window;
		}

		/* Nevermind */
		--_gfx_alive_windows;
	}

	/* Destroy key & window */
	if(!_gfx_alive_windows)
		_gfx_platform_key_clear(_gfx_current_window);

	_gfx_platform_window_free(window->handle);
	free(window);

	return NULL;
}

/******************************************************/
static int _gfx_window_insert(

		const GFX_Window* window)
{
	/* Create vector if it doesn't exist yet */
	if(!_gfx_windows)
	{
		_gfx_windows = gfx_vector_create(sizeof(GFX_Window*));
		if(!_gfx_windows) return 0;
	}

	/* Try to insert, destroy on failure */
	/* Insert all off-screen windows at the end */
	/* Insert all on-screen windows at the beginning */
	size_t index = window->offscreen ?
		gfx_vector_get_size(_gfx_windows) :
		_gfx_public_windows;

	GFXVectorIterator it = gfx_vector_insert_at(
		_gfx_windows,
		&window,
		index
	);

	if(it == _gfx_windows->end)
	{
		if(_gfx_windows->begin == _gfx_windows->end)
		{
			gfx_vector_free(_gfx_windows);
			_gfx_windows = NULL;
		}
		return 0;
	}

	/* Count on-screen windows */
	_gfx_public_windows += window->offscreen ? 0 : 1;

	return 1;
}

/******************************************************/
static void _gfx_window_erase(

		GFX_Window* window)
{
	if(_gfx_windows)
	{
		/* Erase from vector */
		GFXVectorIterator it;
		for(
			it = _gfx_windows->begin;
			it != _gfx_windows->end;
			it = gfx_vector_next(_gfx_windows, it))
		{
			if(window == *(GFX_Window**)it)
			{
				gfx_vector_erase(_gfx_windows, it);

				/* Also count on-screen windows */
				_gfx_public_windows -= window->offscreen ? 0 : 1;

				break;
			}
		}

		/* Free vector */
		if(_gfx_windows->begin == _gfx_windows->end)
		{
			gfx_vector_free(_gfx_windows);
			_gfx_windows = NULL;
		}
	}
}

/******************************************************/
GFX_Window* _gfx_window_get_from_handle(

		GFX_PlatformWindow handle)
{
	if(!_gfx_windows) return NULL;

	GFXVectorIterator it;
	for(
		it = _gfx_windows->begin;
		it != _gfx_windows->end;
		it = gfx_vector_next(_gfx_windows, it))
	{
		if((*(GFX_Window**)it)->handle == handle)
			return *(GFX_Window**)it;
	}

	return NULL;
}

/******************************************************/
GFX_Window* _gfx_window_create(

		GFXColorDepth  depth,
		unsigned int   w,
		unsigned int   h)
{
	/* Create the window */
	GFX_Window* window = _gfx_window_create_common(
		NULL,
		depth,
		"",
		0, 0,
		w, h,
		GFX_WINDOW_HIDDEN
	);

	/* Make main window current again */
	_gfx_window_make_current(_gfx_main_window);
	if(!window) return NULL;

	/* Insert the window */
	window->offscreen = 1;

	if(!_gfx_window_insert(window))
	{
		gfx_window_free((GFXWindow*)window);
		return NULL;
	}

	return window;
}

/******************************************************/
void _gfx_window_destroy(

		GFX_Window* window)
{
	if(!window->handle) return;

	/* Erase from windows */
	_gfx_window_erase(window);
	_gfx_window_make_current(window);

	/* Find a new main window */
	if(_gfx_main_window == window)
	{
		if(_gfx_public_windows)
		{
			/* Save objects at window's context */
			_gfx_render_objects_save(&window->objects);
			_gfx_main_window = *(GFX_Window**)_gfx_windows->begin;
		}

		else _gfx_main_window = NULL;
	}

	/* Free objects, unprepare and unload */
	_gfx_render_objects_free(&window->objects);
	_gfx_pipe_process_unprepare(_gfx_windows ? 0 : 1);
	_gfx_renderer_unload();

	/* Braaaaaaains! */
	_gfx_platform_window_free(window->handle);
	window->handle = NULL;

	if(--_gfx_alive_windows)
	{
		/* Make main active again and restore objects */
		_gfx_window_make_current(_gfx_main_window);

		if(_gfx_main_window) _gfx_render_objects_restore(
			&window->objects,
			&_gfx_main_window->objects
		);
	}

	/* Destruct key as no windows exist anymore */
	else _gfx_platform_key_clear(_gfx_current_window);
}

/******************************************************/
void _gfx_window_make_current(

		GFX_Window* window)
{
	GFX_Window* current =
		_gfx_platform_key_get(_gfx_current_window);

	if(current != window)
	{
		if(!window)
			_gfx_platform_context_make_current(NULL);
		else
			_gfx_platform_context_make_current(window->handle);

		_gfx_platform_key_set(_gfx_current_window, window);
	}
}

/******************************************************/
GFX_Window* _gfx_window_get_current(void)
{
	return _gfx_alive_windows ? _gfx_platform_key_get(_gfx_current_window) : NULL;
}

/******************************************************/
void _gfx_window_swap_buffers(void)
{
	/* Swap buffers */
	GFX_Window* window = _gfx_platform_key_get(_gfx_current_window);

	if(window)
	{
		_gfx_platform_context_swap_buffers(window->handle);

		/* Poll errors while it's current */
		if(gfx_get_error_mode() == GFX_ERROR_MODE_DEBUG)
		{
			/* Loop over all errors */
			GLenum err = window->renderer.GetError();
			while(err != GL_NO_ERROR)
			{
				gfx_errors_push(err, "[DEBUG] An OpenGL error occurred.");
				err = window->renderer.GetError();
			}
		}
	}
}

/******************************************************/
int gfx_is_extension_supported(

		GFXExtension extension)
{
	if(!_gfx_alive_windows) return 0;
	GFX_Window* window = _gfx_platform_key_get(_gfx_current_window);

	return window->ext[extension];
}

/******************************************************/
int gfx_get_limit(

		GFXLimit limit)
{
	if(!_gfx_alive_windows) return -1;
	GFX_Window* window = _gfx_platform_key_get(_gfx_current_window);

	return window->lim[limit];
}

/******************************************************/
void gfx_request_context(

		GFXContext context)
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
	return _gfx_public_windows;
}

/******************************************************/
GFXWindow* gfx_get_window(

		unsigned int num)
{
	if(num >= _gfx_public_windows) return NULL;
	return *(GFXWindow**)gfx_vector_at(_gfx_windows, num);
}

/******************************************************/
GFXWindow* gfx_window_create(

		GFXScreen       screen,
		GFXColorDepth   depth,
		const char*     name,
		int             x,
		int             y,
		unsigned int    w,
		unsigned int    h,
		GFXWindowFlags  flags)
{
	/* Create the window */
	GFX_Window* window = _gfx_window_create_common(
		screen,
		depth,
		name,
		x, y,
		w, h,
		flags
	);

	if(window)
	{
		/* Insert the window */
		if(_gfx_window_insert(window))
		{
			/* Figure out the main window */
			if(!_gfx_main_window)
				_gfx_main_window = window;
			else
				_gfx_window_make_current(_gfx_main_window);

			return (GFXWindow*)window;
		}

		/* Failure */
		gfx_window_free((GFXWindow*)window);
	}

	/* Make main window current again */
	_gfx_window_make_current(_gfx_main_window);

	return NULL;
}

/******************************************************/
GFXWindow* gfx_window_recreate(

		GFXWindow*      window,
		GFXScreen       screen,
		GFXColorDepth   depth,
		GFXWindowFlags  flags)
{
	/* Check if zombie window */
	GFX_Window* internal = (GFX_Window*)window;
	if(!internal->handle || internal->offscreen) return NULL;

	/* Create new window */
	char* name = _gfx_platform_window_get_name(internal->handle);

	unsigned int width;
	unsigned int height;
	int x;
	int y;

	_gfx_platform_window_get_size(
		internal->handle,
		&width,
		&height);

	_gfx_platform_window_get_position(
		internal->handle,
		&x,
		&y);

	GFXWindow* new = gfx_window_create(
		screen,
		depth,
		name,
		x, y,
		width,
		height,
		flags);

	free(name);

	if(!new) return NULL;

	/* Copy event callbacks */
	*new = *window;

	/* Destroy old window and retarget the window */
	_gfx_pipe_process_retarget(internal, (GFX_Window*)new);
	gfx_window_free(window);

	return new;
}

/******************************************************/
void gfx_window_free(

		GFXWindow* window)
{
	if(window)
	{
		_gfx_window_destroy((GFX_Window*)window);
		_gfx_render_objects_clear(&((GFX_Window*)window)->objects);

		free(window);
	}
}

/******************************************************/
int gfx_window_is_open(

		const GFXWindow* window)
{
	if(!window) return 0;
	return ((const GFX_Window*)window)->handle ? 1 : 0;
}

/******************************************************/
GFXScreen gfx_window_get_screen(

		const GFXWindow* window)
{
	/* Check if zombie window */
	const GFX_Window* internal = (GFX_Window*)window;
	if(!internal->handle) return NULL;

	return (GFXScreen)_gfx_platform_window_get_screen(internal->handle);
}

/******************************************************/
GFXContext gfx_window_get_context(

		const GFXWindow* window)
{
	return ((const GFX_Window*)window)->context;
}

/******************************************************/
char* gfx_window_get_name(

		const GFXWindow* window)
{
	/* Check if zombie window */
	const GFX_Window* internal = (GFX_Window*)window;
	if(!internal->handle) return NULL;

	return _gfx_platform_window_get_name(internal->handle);
}

/******************************************************/
void gfx_window_get_size(

		const GFXWindow*  window,
		unsigned int*     width,
		unsigned int*     height)
{
	/* Check if zombie window */
	const GFX_Window* internal = (GFX_Window*)window;
	if(internal->handle) _gfx_platform_window_get_size(
		internal->handle,
		width,
		height
	);
}

/******************************************************/
void gfx_window_get_position(

		const GFXWindow*  window,
		int*              x,
		int*              y)
{
	/* Check if zombie window */
	const GFX_Window* internal = (GFX_Window*)window;
	if(internal->handle) _gfx_platform_window_get_position(
		internal->handle,
		x, y
	);
}

/******************************************************/
void gfx_window_set_name(

		const GFXWindow*  window,
		const char*       name)
{
	/* Check if zombie window */
	const GFX_Window* internal = (GFX_Window*)window;
	if(internal->handle) _gfx_platform_window_set_name(
		internal->handle,
		name
	);
}

/******************************************************/
void gfx_window_set_size(

		const GFXWindow*  window,
		unsigned int      width,
		unsigned int      height)
{
	/* Check if zombie window */
	const GFX_Window* internal = (GFX_Window*)window;
	if(internal->handle) _gfx_platform_window_set_size(
		internal->handle,
		width,
		height
	);
}

/******************************************************/
void gfx_window_set_position(

		const GFXWindow*  window,
		int               x,
		int               y)
{
	/* Check if zombie window */
	const GFX_Window* internal = (GFX_Window*)window;
	if(internal->handle) _gfx_platform_window_set_position(
		internal->handle,
		x, y
	);
}

/******************************************************/
void gfx_window_show(

		const GFXWindow* window)
{
	/* Check if zombie window */
	const GFX_Window* internal = (GFX_Window*)window;
	if(internal->handle) _gfx_platform_window_show(internal->handle);
}

/******************************************************/
void gfx_window_hide(

		const GFXWindow* window)
{
	/* Check if zombie window */
	const GFX_Window* internal = (GFX_Window*)window;
	if(internal->handle) _gfx_platform_window_hide(internal->handle);
}

/******************************************************/
int gfx_window_set_swap_interval(

		const GFXWindow*  window,
		int               num)
{
	/* Check if zombie window */
	const GFX_Window* internal = (GFX_Window*)window;
	if(internal->handle)
	{
		/* Again make sure the main window is current afterwards */
		num = _gfx_platform_context_set_swap_interval(internal->handle, num);
		_gfx_window_make_current(_gfx_main_window);
	}

	return num;
}
