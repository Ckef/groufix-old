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

#include "groufix/core/platform/x11.h"

#if defined(GFX_RENDERER_GL)

/******************************************************/
static GLXContext _gfx_x11_create_context(

		int          major,
		int          minor,
		GLXFBConfig  config,
		GLXContext   share,
		int          debug)
{
	/* Temporarily disable errors */
	/* This so we can attempt to create contexts with various versions */
	_gfx_x11.errors = 0;

	/* Create buffer attribute array */
	int flags =
		GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB |
		(debug ? GLX_CONTEXT_DEBUG_BIT_ARB : 0);

	int bufferAttr[] =
	{
		GLX_CONTEXT_MAJOR_VERSION_ARB, major,
		GLX_CONTEXT_MINOR_VERSION_ARB, minor,
		GLX_CONTEXT_FLAGS_ARB,         flags,
		GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None
	};

	/* Create the context */
	GLXContext context = _gfx_x11.extensions.CreateContextAttribsARB(
		_gfx_x11.display,
		config,
		share,
		True,
		bufferAttr
	);

	_gfx_x11.errors = 1;
	return context;
}

#endif

/******************************************************/
GFX_PlatformContext _gfx_platform_context_create(

		GFX_PlatformWindow*  handle,
		int                  major,
		int                  minor,
		GFX_PlatformContext  share,
		int                  debug)
{
	GFX_PlatformContext context = NULL;
	*handle = NULL;

#if defined(GFX_RENDERER_GL)

	/* Get config from default screen */
	int buffElements;
	int attr = None;

	GLXFBConfig* config = glXChooseFBConfig(
		_gfx_x11.display,
		XDefaultScreen(_gfx_x11.display),
		&attr,
		&buffElements
	);

	if(!config) return NULL;

	/* Create context */
	context = _gfx_x11_create_context(
		major,
		minor,
		*config,
		share,
		debug
	);

	XFree(config);

#endif

	return context;
}

/******************************************************/
void _gfx_platform_context_free(

		GFX_PlatformContext context)
{
#if defined(GFX_RENDERER_GL)

	glXMakeContextCurrent(_gfx_x11.display, None, None, NULL);
	glXDestroyContext(_gfx_x11.display, context);

#endif
}

/******************************************************/
GFX_PlatformContext _gfx_platform_context_init(

		GFX_PlatformWindow   handle,
		int                  major,
		int                  minor,
		GFX_PlatformContext  share,
		int                  debug)
{
	/* Get the window */
	GFX_X11_Window* window = _gfx_x11_get_window_from_handle(
		GFX_VOID_TO_UINT(handle));

	if(!window) return NULL;

	GFX_PlatformContext context = NULL;

#if defined(GFX_RENDERER_GL)

	/* Create context */
	window->context = _gfx_x11_create_context(
		major,
		minor,
		window->config,
		share,
		debug
	);

	context = window->context;

#endif

	return context;
}

/******************************************************/
void _gfx_platform_context_clear(

		GFX_PlatformWindow handle)
{
	/* Get the window and destroy its context */
	GFX_X11_Window* window = _gfx_x11_get_window_from_handle(
		GFX_VOID_TO_UINT(handle));

	if(window)
	{
#if defined(GFX_RENDERER_GL)

		_gfx_platform_context_free(window->context);
		window->context = NULL;

#endif
	}
}

/******************************************************/
int _gfx_platform_context_set_swap_interval(

		GFX_PlatformWindow  handle,
		int                 num)
{
#if defined(GFX_RENDERER_GL)

	if(!_gfx_x11.extensions.SwapIntervalEXT)
		return 0;

	/* Correct if adaptive vsync is not supported */
	if(!_gfx_x11.extensions.EXT_swap_control_tear && num < 0)
		num = -num;

	_gfx_x11.extensions.SwapIntervalEXT(
		_gfx_x11.display,
		(Window)GFX_VOID_TO_UINT(handle),
		num
	);

#endif

	return num;
}

/******************************************************/
void _gfx_platform_context_swap_buffers(

		GFX_PlatformWindow handle)
{
#if defined(GFX_RENDERER_GL)

	glXSwapBuffers(_gfx_x11.display, (Window)GFX_VOID_TO_UINT(handle));

#endif
}

/******************************************************/
void _gfx_platform_context_make_current(

		GFX_PlatformWindow   handle,
		GFX_PlatformContext  context)
{
#if defined(GFX_RENDERER_GL)

	if(!context) glXMakeCurrent(
		_gfx_x11.display,
		None,
		NULL);

	else if(handle) glXMakeCurrent(
		_gfx_x11.display,
		(Window)GFX_VOID_TO_UINT(handle),
		context);

	else glXMakeCurrent(
		_gfx_x11.display,
		XDefaultRootWindow(_gfx_x11.display),
		context);

#endif
}

/******************************************************/
GFX_ProcAddress _gfx_platform_get_proc_address(

		const char* proc)
{
	GFX_ProcAddress addr = NULL;

#if defined(GFX_RENDERER_GL)

	addr = (GFX_ProcAddress)glXGetProcAddressARB((const GLubyte*)proc);

#endif

	return addr;
}
