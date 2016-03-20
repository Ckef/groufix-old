/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/internal.h"

#include <stdlib.h>

/******************************************************/
/** Current context data key */
static GFX_PlatformKey _gfx_current_context;


/** Created contexts */
static GFXVector _gfx_contexts;


/** Number of on-screen contexts */
static unsigned int _gfx_public_contexts = 0;


/** Dummy context for the main thread */
static GFX_Context* _gfx_dummy_context = NULL;


/** Requested context request */
static GFXContext _gfx_version =
{
	.major = GFX_CONTEXT_MAJOR_MIN,
	.minor = GFX_CONTEXT_MINOR_MIN
};


/******************************************************/
static GFX_PlatformContext _gfx_context_create_platform(

		GFX_PlatformWindow*  window,
		int*                 major,
		int*                 minor)
{
	int debug = _gfx_errors_get_mode() == GFX_ERROR_MODE_DEBUG;

	/* Get current context to share with (as all sharable contexts will share) */
	GFX_PlatformContext share = NULL;

	GFX_Context* shareCont = _gfx_platform_key_get(_gfx_current_context);
	if(shareCont) share = shareCont->context;

	/* Get maximum context version */
	GFXContext max =
	{
		.major = GFX_CONTEXT_MAJOR_MAX,
		.minor = GFX_CONTEXT_MINOR_MAX
	};

	/* Iterate from max to min until a valid context was created */
	while(
		max.major > _gfx_version.major ||
		(max.major == _gfx_version.major &&
		max.minor >= _gfx_version.minor))
	{
		/* Try to create it */
		GFX_PlatformWindow wind;
		GFX_PlatformContext cont;

		if(*window) cont = _gfx_platform_context_init(
			*window,
			max.major,
			max.minor,
			share,
			debug);

		else cont = _gfx_platform_context_create(
			&wind,
			max.major,
			max.minor,
			share,
			debug);

		if(cont)
		{
			*window = *window ? *window : wind;
			*major = max.major;
			*minor = max.minor;

			return cont;
		}

		/* Previous version */
		if(!max.minor)
		{
			--max.major;
			max.minor = GFX_CONTEXT_ALL_MINORS_MAX;
		}
		else --max.minor;
	}

	/* Nope */
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"The requested minimal context version %i.%i could not be created.",
		_gfx_version.major,
		_gfx_version.minor
	);

	return NULL;
}

/******************************************************/
static GFX_Context* _gfx_context_create_internal(

		GFX_PlatformAttributes* attr)
{
	if(attr)
	{
		/* Adjust attributes */
		attr->flags = attr->flags & GFX_WINDOW_FULLSCREEN ?
			attr->flags & ~GFX_WINDOW_RESIZABLE :
			attr->flags;

		attr->monitor = attr->monitor ?
			attr->monitor :
			_gfx_platform_get_default_monitor();

		/* Validate fullscreen monitor or depth */
		if(attr->flags & GFX_WINDOW_FULLSCREEN)
		{
			unsigned int modes =
				_gfx_platform_monitor_get_num_modes(attr->monitor);

			if(attr->mode >= modes)
				return NULL;
		}
		else
		{
			if(!attr->depth)
				return NULL;
		}
	}

	/* Setup generic context */
	GFX_Context* context = calloc(1, sizeof(GFX_Context));
	if(!context)
	{
		/* Out of memory error */
		gfx_errors_output(
			"[GFX Out Of Memory]: Context could not be allocated."
		);
		return NULL;
	}

	if(attr)
	{
		/* Create platform window */
		context->swap =
			attr->flags & GFX_WINDOW_DOUBLE_BUFFER;
		context->handle =
			_gfx_platform_window_create(attr);

		if(!context->handle)
		{
			/* Unknown error */
			gfx_errors_push(
				GFX_ERROR_UNKNOWN,
				"Platform window could not be created."
			);

			free(context);
			return NULL;
		}
	}
	else
	{
		context->offscreen = 1;
	}

	/* Create platform context */
	context->context = _gfx_context_create_platform(
		&context->handle,
		&context->version.major,
		&context->version.minor
	);

	if(context->context)
	{
		/* Load renderer and initialize state */
		_gfx_context_make_current(context);

		_gfx_renderer_load();
		_gfx_states_set_default(&context->state);
		_gfx_states_force_set(&context->state, GFX_CONT_INT_AS_ARG(context));

		if(_gfx_errors_get_mode() == GFX_ERROR_MODE_DEBUG)
			_gfx_renderer_init_errors();

		/* Try to prepare the context for post processing */
		if(_gfx_pipe_process_prepare())
		{
			/* And finally initialize the render object container */
			if(_gfx_render_objects_init(&context->objects))
				return context;
		}

		/* Nevermind */
		_gfx_pipe_process_unprepare(1);
		_gfx_renderer_unload();
	}

	/* Destroy window or context */
	if(!context->offscreen)
		_gfx_platform_window_free(context->handle);
	else
		_gfx_platform_context_free(context->context);

	free(context);

	return NULL;
}

/******************************************************/
static int _gfx_context_insert(

		const GFX_Context* context)
{
	/* Try to insert, destroy on failure */
	/* Insert all off-screen contexts at the end */
	/* Insert all on-screen contexts at the beginning */
	size_t index = context->offscreen ?
		gfx_vector_get_size(&_gfx_contexts) :
		_gfx_public_contexts;

	GFXVectorIterator it = gfx_vector_insert_at(
		&_gfx_contexts,
		&context,
		index
	);

	if(it == _gfx_contexts.end)
		return 0;

	/* Count on-screen contexts */
	_gfx_public_contexts +=
		context->offscreen ? 0 : 1;

	return 1;
}

/******************************************************/
static void _gfx_context_erase(

		GFX_Context* context)
{
	/* Erase from vector */
	GFX_Context** it;
	for(
		it = _gfx_contexts.begin;
		it != _gfx_contexts.end;
		it = gfx_vector_next(&_gfx_contexts, it))
	{
		if(context == *it)
		{
			gfx_vector_erase(&_gfx_contexts, it);

			/* Also count on-screen contexts */
			_gfx_public_contexts -=
				context->offscreen ? 0 : 1;

			break;
		}
	}
}

/******************************************************/
GFX_Context* _gfx_context_get_from_handle(

		GFX_PlatformWindow handle)
{
	GFX_Context** it;
	for(
		it = _gfx_contexts.begin;
		it != _gfx_contexts.end;
		it = gfx_vector_next(&_gfx_contexts, it))
	{
		if((*it)->handle == handle)
			return (*it)->offscreen ? NULL : *it;
	}

	return NULL;
}

/******************************************************/
int _gfx_context_manager_init(

		GFXContext version)
{
	/* Initialize current context key */
	if(!_gfx_platform_key_init(&_gfx_current_context))
		return 0;

	/* Get minimal context */
	if(version.major < GFX_CONTEXT_MAJOR_MIN)
	{
		version.major = GFX_CONTEXT_MAJOR_MIN;
		version.minor = GFX_CONTEXT_MINOR_MIN;
	}
	else if(
		version.minor < GFX_CONTEXT_MINOR_MIN &&
		version.major == GFX_CONTEXT_MAJOR_MIN)
	{
		version.minor = GFX_CONTEXT_MINOR_MIN;
	}

	_gfx_version = version;

	/* Initialize context storage */
	gfx_vector_init(&_gfx_contexts, sizeof(GFX_Context*));

	/* Create dummy context */
	_gfx_dummy_context = _gfx_context_create_internal(NULL);

	if(!_gfx_dummy_context)
	{
		gfx_vector_clear(&_gfx_contexts);
		_gfx_platform_key_clear(_gfx_current_context);

		return 0;
	}

	return 1;
}

/******************************************************/
void _gfx_context_manager_terminate(void)
{
	/* Destroy all contexts */
	while(_gfx_contexts.begin != _gfx_contexts.end) _gfx_context_destroy(
		*(GFX_Context**)gfx_vector_previous(&_gfx_contexts, _gfx_contexts.end));

	/* Free dummy context */
	gfx_window_free((GFXWindow*)_gfx_dummy_context);
	_gfx_dummy_context = NULL;

	gfx_vector_clear(&_gfx_contexts);
	_gfx_platform_key_clear(_gfx_current_context);
}

/******************************************************/
GFX_Context* _gfx_context_create(void)
{
	/* Create the context */
	/* Make sure to keep the correct context current */
	GFX_Context* curr = _gfx_platform_key_get(_gfx_current_context);
	GFX_Context* context = _gfx_context_create_internal(NULL);
	_gfx_context_make_current(curr);

	if(!context) return NULL;

	/* Insert the context */
	if(!_gfx_context_insert(context))
	{
		gfx_window_free((GFXWindow*)context);
		return NULL;
	}

	return context;
}

/******************************************************/
void _gfx_context_destroy(

		GFX_Context* context)
{
	if(!context->context) return;

	/* Find a new context for the main thread */
	_gfx_context_erase(context);
	GFX_Context* curr = _gfx_platform_key_get(_gfx_current_context);

	if(curr == context)
	{
		if(_gfx_public_contexts)
			curr = *(GFX_Context**)_gfx_contexts.begin;

		else if(context != _gfx_dummy_context)
		{
			if(!_gfx_dummy_context)
				_gfx_dummy_context = _gfx_context_create_internal(NULL);

			curr = _gfx_dummy_context;
		}

		else curr = NULL;
	}

	/* Get if this is the last context */
	int last = !curr && _gfx_contexts.begin == _gfx_contexts.end;

	/* First unprepare */
	/* Let the processes free their resources */
	_gfx_context_make_current(context);
	_gfx_pipe_process_unprepare(last);

	/* Prepare for transfer and unload */
	if(curr) _gfx_render_objects_prepare(&context->objects, 1);
	_gfx_renderer_unload();

	/* Braaaaaaains! */
	if(context->offscreen)
		_gfx_platform_context_free(context->context);
	else
		_gfx_platform_window_free(context->handle);

	context->handle = NULL;
	context->context = NULL;

	/* Make corrext current again */
	/* Also transfer all objects */
	_gfx_context_make_current(curr);

	if(curr) _gfx_render_objects_transfer(&context->objects, &curr->objects, 1);
	_gfx_render_objects_clear(&context->objects);
}

/******************************************************/
void _gfx_context_make_current(

		GFX_Context* context)
{
	GFX_Context* current =
		_gfx_platform_key_get(_gfx_current_context);

	if(current != context)
	{
		if(!context) _gfx_platform_context_make_current(
			NULL,
			NULL);

		else _gfx_platform_context_make_current(
			context->handle,
			context->context);

		_gfx_platform_key_set(_gfx_current_context, context);
	}
}

/******************************************************/
GFX_Context* _gfx_context_get_current(void)
{
	return _gfx_platform_key_get(_gfx_current_context);
}

/******************************************************/
unsigned int gfx_get_num_windows(void)
{
	return _gfx_public_contexts;
}

/******************************************************/
GFXWindow* gfx_get_window(

		unsigned int num)
{
	if(num >= _gfx_public_contexts) return NULL;
	return *(GFXWindow**)gfx_vector_at(&_gfx_contexts, num);
}

/******************************************************/
GFXWindow* gfx_window_create(

		GFXMonitor            monitor,
		unsigned int          mode,
		const GFXColorDepth*  depth,
		const char*           name,
		int                   x,
		int                   y,
		unsigned int          w,
		unsigned int          h,
		GFXWindowFlags        flags)
{
	/* Create the window */
	GFX_PlatformAttributes attr =
	{
		.monitor = (GFX_PlatformMonitor)monitor,
		.mode    = mode,
		.depth   = depth,
		.name    = name,
		.flags   = flags,
		.x       = x,
		.y       = y,
		.w       = w,
		.h       = h
	};

	GFX_Context* curr = _gfx_platform_key_get(_gfx_current_context);
	GFX_Context* context = _gfx_context_create_internal(&attr);

	if(context)
	{
		/* Insert the context */
		if(_gfx_context_insert(context))
		{
			/* Destroy dummy context */
			if(_gfx_dummy_context)
			{
				gfx_window_free((GFXWindow*)_gfx_dummy_context);
				_gfx_dummy_context = NULL;
			}

			return (GFXWindow*)context;
		}

		/* Failure */
		gfx_window_free((GFXWindow*)context);
	}

	/* Make correct context current again */
	_gfx_context_make_current(curr);

	return NULL;
}

/******************************************************/
GFXWindow* gfx_window_recreate(

		GFXWindow*            window,
		GFXMonitor            monitor,
		unsigned int          mode,
		const GFXColorDepth*  depth,
		GFXWindowFlags        flags)
{
	/* Check if zombie context */
	GFX_Context* context = (GFX_Context*)window;
	if(!context->context || context->offscreen)
		return NULL;

	/* Get window properties */
	int x;
	int y;
	unsigned int w;
	unsigned int h;

	char* name = _gfx_platform_window_get_name(
		context->handle);

	_gfx_platform_window_get_position(
		context->handle,
		&x,
		&y);

	_gfx_platform_window_get_size(
		context->handle,
		&w,
		&h);

	/* Hide original window first */
	/* This to undo any window manager effects it may have */
	_gfx_platform_window_hide(context->handle);

	/* Create our new window */
	GFXWindow* new = gfx_window_create(
		monitor,
		mode,
		depth,
		name,
		x,
		y,
		w,
		h,
		flags
	);

	free(name);

	if(!new) return NULL;

	/* Copy event callbacks */
	*new = *window;

	/* Retarget the context at processes */
	_gfx_context_make_current(context);
	_gfx_pipe_process_retarget((GFX_Context*)new);

	/* Now free the old window */
	/* Don't make something new current as the freeing should take care of that */
	gfx_window_free(window);

	return new;
}

/******************************************************/
void gfx_window_free(

		GFXWindow* window)
{
	if(window)
	{
		_gfx_context_destroy((GFX_Context*)window);
		free(window);
	}
}

/******************************************************/
int gfx_window_is_open(

		const GFXWindow* window)
{
	if(!window) return 0;
	return ((const GFX_Context*)window)->handle ? 1 : 0;
}

/******************************************************/
GFXMonitor gfx_window_get_monitor(

		const GFXWindow* window)
{
	const GFX_Context* context = (GFX_Context*)window;
	if(!context->context || context->offscreen)
		return NULL;

	return (GFXMonitor)_gfx_platform_window_get_monitor(context->handle);
}

/******************************************************/
char* gfx_window_get_name(

		const GFXWindow* window)
{
	const GFX_Context* context = (GFX_Context*)window;
	if(!context->context || context->offscreen)
		return NULL;

	return _gfx_platform_window_get_name(context->handle);
}

/******************************************************/
void gfx_window_get_size(

		const GFXWindow*  window,
		unsigned int*     width,
		unsigned int*     height)
{
	const GFX_Context* context = (GFX_Context*)window;

	if(context->context && !context->offscreen)
	{
		_gfx_platform_window_get_size(
			context->handle,
			width,
			height
		);
	}
	else
	{
		*width = 0;
		*height = 0;
	}
}

/******************************************************/
void gfx_window_get_position(

		const GFXWindow*  window,
		int*              x,
		int*              y)
{
	const GFX_Context* context = (GFX_Context*)window;

	if(context->context && !context->offscreen)
	{
		_gfx_platform_window_get_position(
			context->handle,
			x,
			y
		);
	}
	else
	{
		*x = 0;
		*y = 0;
	}
}

/******************************************************/
void gfx_window_set_name(

		const GFXWindow*  window,
		const char*       name)
{
	const GFX_Context* context = (GFX_Context*)window;
	if(context->context && !context->offscreen)
		_gfx_platform_window_set_name(
			context->handle,
			name
		);
}

/******************************************************/
void gfx_window_set_size(

		const GFXWindow*  window,
		unsigned int      width,
		unsigned int      height)
{
	const GFX_Context* context = (GFX_Context*)window;
	if(context->context && !context->offscreen)
		_gfx_platform_window_set_size(
			context->handle,
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
	const GFX_Context* context = (GFX_Context*)window;
	if(context->context && !context->offscreen)
		_gfx_platform_window_set_position(
			context->handle,
			x,
			y
		);
}

/******************************************************/
void gfx_window_show(

		const GFXWindow* window)
{
	const GFX_Context* context = (GFX_Context*)window;
	if(context->context && !context->offscreen)
		_gfx_platform_window_show(context->handle);
}

/******************************************************/
void gfx_window_hide(

		const GFXWindow* window)
{
	const GFX_Context* context = (GFX_Context*)window;
	if(context->context && !context->offscreen)
		_gfx_platform_window_hide(context->handle);
}

/******************************************************/
int gfx_window_set_swap_interval(

		const GFXWindow*  window,
		int               num)
{
	const GFX_Context* context = (GFX_Context*)window;
	if(context->context && !context->offscreen)
		num = _gfx_platform_context_set_swap_interval(context->handle, num);

	return num;
}
