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

#include "groufix/containers/vector.h"
#include "groufix/pipeline/internal.h"
#include "groufix/errors.h"

#include <stdlib.h>

/******************************************************/
/* Current window */
static GFX_Internal_Window* _gfx_current_window = NULL;

/* Main window (main context) */
static GFX_Internal_Window* _gfx_main_window = NULL;

/* Created windows */
static GFXVector* _gfx_windows = NULL;

/* OpenGL context request */
static GFXContext _gfx_context = {
	GFX_CONTEXT_MAJOR_MIN,
	GFX_CONTEXT_MINOR_MIN
};

/******************************************************/
static int _gfx_window_context_create(GFX_Platform_Window window)
{
	/* Get the main window to share with (as all windows will share everything) */
	GFX_Platform_Window* share = NULL;
	if(_gfx_main_window) share = _gfx_main_window->handle;

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
		if(_gfx_platform_context_create(window, max.major, max.minor, share)) return 1;

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
static int _gfx_window_insert(const GFX_Internal_Window* window)
{
	/* Create vector if it doesn't exist yet */
	if(!_gfx_windows)
	{
		_gfx_windows = gfx_vector_create(sizeof(GFX_Internal_Window*));
		if(!_gfx_windows) return 0;
	}
	return gfx_vector_insert(_gfx_windows, &window, _gfx_windows->end) != _gfx_windows->end;
}

/******************************************************/
GFX_Internal_Window* _gfx_window_get_from_handle(GFX_Platform_Window handle)
{
	if(!_gfx_windows) return NULL;

	GFXVectorIterator it;
	for(it = _gfx_windows->begin; it != _gfx_windows->end; it = gfx_vector_next(_gfx_windows, it))
		if((*(GFX_Internal_Window**)it)->handle == handle) return *(GFX_Internal_Window**)it;

	return NULL;
}

/******************************************************/
void _gfx_window_make_current(GFX_Internal_Window* window)
{
	if(window && _gfx_current_window != window)
	{
		_gfx_platform_context_make_current(window->handle);
		_gfx_current_window = window;
	}
}

/******************************************************/
GFX_Internal_Window* _gfx_window_get_current(void)
{
	return _gfx_current_window;
}

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
	return gfx_vector_get_size(_gfx_windows);
}

/******************************************************/
GFXWindow* gfx_get_window(unsigned int num)
{
	if(num >= gfx_get_num_windows()) return NULL;
	return *(GFXWindow**)gfx_vector_at(_gfx_windows, num);
}

/******************************************************/
GFXWindow* gfx_window_create(GFXScreen screen, GFXColorDepth depth, const char* name, unsigned int w, unsigned int h, GFXWindowFlags flags)
{
	/* Setup top level window */
	GFX_Internal_Window* window = calloc(1, sizeof(GFX_Internal_Window));
	if(!window) return NULL;

	/* Get screen */
	GFX_Platform_Screen scr;
	if(screen) scr = (GFX_Platform_Screen)screen;
	else scr = _gfx_platform_get_default_screen();

	/* Create platform window */
	GFX_Platform_Attributes attr;
	attr.screen = scr;
	attr.name   = name;
	attr.width  = w;
	attr.height = h;
	attr.x      = 0;
	attr.y      = 0;
	attr.depth  = depth;
	attr.flags  = flags;

	window->handle = _gfx_platform_window_create(&attr);
	if(!window->handle)
	{
		free(window);
		return NULL;
	}

	/* Create context and insert in the vector */
	if(!_gfx_window_context_create(window->handle) || !_gfx_window_insert(window))
	{
		_gfx_platform_window_free(window->handle);
		free(window);

		return NULL;
	}

	/* Evaluate main window */
	if(!_gfx_main_window) _gfx_main_window = window;

	/* Load extensions of context and make sure to set the main window as current */
	_gfx_window_make_current(window);
	_gfx_extensions_load();
	_gfx_window_make_current(_gfx_main_window);

	return (GFXWindow*)window;
}

/******************************************************/
int gfx_window_recreate(const GFXWindow* window, GFXScreen screen, GFXColorDepth depth, GFXWindowFlags flags)
{
	/* Check if zombie window */
	GFX_Internal_Window* internal = (GFX_Internal_Window*)window;
	if(!internal->handle) return 0;

	/* Get screen */
	GFX_Platform_Screen scr;
	if(screen) scr = (GFX_Platform_Screen)screen;
	else scr = _gfx_platform_get_default_screen();

	/* Create platform window */
	char* name = _gfx_platform_window_get_name(internal->handle);

	GFX_Platform_Attributes attr;
	attr.screen = scr;
	attr.name   = name;
	attr.depth  = depth;
	attr.flags  = flags;

	_gfx_platform_window_get_size(internal->handle, &attr.width, &attr.height);
	_gfx_platform_window_get_position(internal->handle, &attr.x, &attr.y);

	GFX_Platform_Window handle = _gfx_platform_window_create(&attr);

	free(name);
	if(!handle) return 0;

	/* Create context */
	if(!_gfx_window_context_create(handle))
	{
		_gfx_platform_window_free(handle);
		return 0;
	}

	int isMain = (internal == _gfx_main_window);

	/* Save objects if main window and destroy old window */
	_gfx_window_make_current(internal);
	if(isMain) _gfx_hardware_objects_save(&internal->extensions);

	_gfx_pipe_process_untarget(internal);
	_gfx_platform_window_free(internal->handle);

	/* Load new extensions */
	internal->handle = handle;
	_gfx_platform_context_make_current(handle);

	_gfx_extensions_load();
	_gfx_pipe_process_target(internal);

	/* Restore hardware objects if main window & make sure the main window is current */
	if(isMain) _gfx_hardware_objects_restore(&internal->extensions);
	else _gfx_window_make_current(_gfx_main_window);

	return 1;
}

/******************************************************/
void _gfx_window_destroy(GFX_Internal_Window* window)
{
	if(window)
	{
		/* Zombie window */
		if(!window->handle) return;

		/* Untarget all pipe processes */
		_gfx_window_make_current(window);
		_gfx_pipe_process_untarget(window);

		/* Erase from vector */
		GFXVectorIterator it;
		for(it = _gfx_windows->begin; it != _gfx_windows->end; it = gfx_vector_next(_gfx_windows, it))
			if(window == *(GFX_Internal_Window**)it)
			{
				gfx_vector_erase(_gfx_windows, it);
				break;
			}

		/* Welp, no more windows */
		if(_gfx_windows->begin == _gfx_windows->end)
		{
			/* Oh, also do a free request */
			_gfx_hardware_objects_free(&window->extensions);
			_gfx_platform_window_free(window->handle);

			gfx_vector_free(_gfx_windows);
			_gfx_windows = NULL;

			_gfx_current_window = NULL;
			_gfx_main_window = NULL;
		}

		/* If main window, save & restore hardware objects */
		else if(_gfx_main_window == window)
		{
			_gfx_hardware_objects_save(&window->extensions);
			_gfx_platform_window_free(window->handle);

			/* Get new main window */
			_gfx_main_window = *(GFX_Internal_Window**)_gfx_windows->begin;
			_gfx_window_make_current(_gfx_main_window);

			_gfx_hardware_objects_restore(&_gfx_main_window->extensions);
		}
		else
		{
			/* Just, destroy it, thank you very much */
			_gfx_platform_window_free(window->handle);
			_gfx_window_make_current(_gfx_main_window);
		}

		window->handle = NULL;
	}
}

/******************************************************/
void gfx_window_free(GFXWindow* window)
{
	if(window)
	{
		_gfx_window_destroy((GFX_Internal_Window*)window);
		free(window);
	}
}

/******************************************************/
int gfx_window_is_open(const GFXWindow* window)
{
	return ((GFX_Internal_Window*)window)->handle ? 1 : 0;
}

/******************************************************/
GFXScreen gfx_window_get_screen(const GFXWindow* window)
{
	/* Check if zombie window */
	GFX_Internal_Window* internal = (GFX_Internal_Window*)window;
	if(!internal->handle) return NULL;

	return (GFXScreen)_gfx_platform_window_get_screen(internal->handle);
}

/******************************************************/
GFXContext gfx_window_get_context(const GFXWindow* window)
{
	GFX_Internal_Window* internal = (GFX_Internal_Window*)window;
	GFXContext context = { 0,0 };

	/* Check for zombie window */
	if(internal->handle)
	{
		_gfx_window_make_current(internal);
		_gfx_platform_context_get(&context.major, &context.minor);
		_gfx_window_make_current(_gfx_main_window);
	}
	return context;
}

/******************************************************/
char* gfx_window_get_name(const GFXWindow* window)
{
	/* Check if zombie window */
	GFX_Internal_Window* internal = (GFX_Internal_Window*)window;
	if(!internal->handle) return NULL;

	return _gfx_platform_window_get_name(internal->handle);
}

/******************************************************/
void gfx_window_get_size(const GFXWindow* window, unsigned int* width, unsigned int* height)
{
	/* Check if zombie window */
	GFX_Internal_Window* internal = (GFX_Internal_Window*)window;
	if(internal->handle) _gfx_platform_window_get_size(internal->handle, width, height);
}

/******************************************************/
void gfx_window_get_position(const GFXWindow* window, int* x, int* y)
{
	/* Check if zombie window */
	GFX_Internal_Window* internal = (GFX_Internal_Window*)window;
	if(internal->handle) _gfx_platform_window_get_position(internal->handle, x, y);
}

/******************************************************/
void gfx_window_set_name(const GFXWindow* window, const char* name)
{
	/* Check if zombie window */
	GFX_Internal_Window* internal = (GFX_Internal_Window*)window;
	if(internal->handle) _gfx_platform_window_set_name(internal->handle, name);
}

/******************************************************/
void gfx_window_set_size(const GFXWindow* window, unsigned int width, unsigned int height)
{
	/* Check if zombie window */
	GFX_Internal_Window* internal = (GFX_Internal_Window*)window;
	if(internal->handle) _gfx_platform_window_set_size(internal->handle, width, height);
}

/******************************************************/
void gfx_window_set_position(const GFXWindow* window, int x, int y)
{
	/* Check if zombie window */
	GFX_Internal_Window* internal = (GFX_Internal_Window*)window;
	if(internal->handle) _gfx_platform_window_set_position(internal->handle, x, y);
}

/******************************************************/
void gfx_window_show(const GFXWindow* window)
{
	/* Check if zombie window */
	GFX_Internal_Window* internal = (GFX_Internal_Window*)window;
	if(internal->handle) _gfx_platform_window_show(internal->handle);
}

/******************************************************/
void gfx_window_hide(const GFXWindow* window)
{
	/* Check if zombie window */
	GFX_Internal_Window* internal = (GFX_Internal_Window*)window;
	if(internal->handle) _gfx_platform_window_hide(internal->handle);
}

/******************************************************/
void gfx_window_set_swap_interval(const GFXWindow* window, int num)
{
	/* Check if zombie window */
	GFX_Internal_Window* internal = (GFX_Internal_Window*)window;
	if(internal->handle)
	{
		/* Again make sure the main window is current afterwards */
		_gfx_platform_context_set_swap_interval(internal->handle, num);
		_gfx_window_make_current(_gfx_main_window);
	}
}

/******************************************************/
void gfx_window_swap_buffers(const GFXWindow* window)
{
	/* Check if zombie window */
	GFX_Internal_Window* internal = (GFX_Internal_Window*)window;
	if(internal->handle)
	{
		_gfx_platform_context_swap_buffers(internal->handle);
		if(internal != _gfx_main_window)
			_gfx_platform_context_make_current(_gfx_main_window->handle);
	}
}
