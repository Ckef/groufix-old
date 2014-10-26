/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#define GL_GLEXT_PROTOTYPES
#include "groufix/core/renderer.h"
#include "groufix/core/platform/x11.h"

/******************************************************/
GFX_PlatformContext _gfx_platform_context_init(

		GFX_PlatformWindow   handle,
		int                  major,
		int                  minor,
		GFX_PlatformContext  share)
{
	/* Get the window */
	GFX_X11_Window* window = _gfx_x11_get_window_from_handle(
		GFX_VOID_TO_UINT(handle));

	if(!window) return NULL;

	/* Create buffer attribute array */
	int bufferAttr[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, major,
		GLX_CONTEXT_MINOR_VERSION_ARB, minor,
		GLX_CONTEXT_FLAGS_ARB,         GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None
	};

	/* Create the context */
	window->context = _gfx_x11->extensions.CreateContextAttribsARB(
		_gfx_x11->display,
		window->config,
		share,
		True,
		bufferAttr
	);

	/* Make it current */
	if(window->context) glXMakeContextCurrent(
		_gfx_x11->display,
		window->handle,
		window->handle,
		window->context
	);

	return window->context;
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
		glXMakeContextCurrent(_gfx_x11->display, None, None, NULL);
		glXDestroyContext(_gfx_x11->display, window->context);

		window->context = NULL;
	}
}

/******************************************************/
int _gfx_platform_context_set_swap_interval(

		GFX_PlatformWindow  handle,
		int                 num)
{
	/* Correct if adaptive vsync is not supported */
	if(!_gfx_x11->extensions.EXT_swap_control_tear && num < 0)
		num = -num;

	if(_gfx_x11->extensions.SwapIntervalEXT)
		_gfx_x11->extensions.SwapIntervalEXT(
			_gfx_x11->display,
			(Window)GFX_VOID_TO_UINT(handle), num
		);

	return num;
}

/******************************************************/
void _gfx_platform_context_swap_buffers(

		GFX_PlatformWindow handle)
{
	if(_gfx_x11) glXSwapBuffers(
		_gfx_x11->display,
		(Window)GFX_VOID_TO_UINT(handle)
	);
}

/******************************************************/
void _gfx_platform_context_get(

		int*  major,
		int*  minor)
{
	GLint ma, mi;
	glGetIntegerv(GL_MAJOR_VERSION, &ma);
	glGetIntegerv(GL_MINOR_VERSION, &mi);

	*major = ma;
	*minor = mi;
}

/******************************************************/
void _gfx_platform_context_make_current(

		GFX_PlatformContext handle)
{
	if(!handle) glXMakeContextCurrent(
		_gfx_x11->display,
		None,
		None,
		NULL
	);

	else
	{
		GFX_X11_Window* window =
			_gfx_x11_get_window_from_context(handle);

		if(window) glXMakeContextCurrent(
			_gfx_x11->display,
			window->handle,
			window->handle,
			window->context
		);
	}
}

/******************************************************/
GFX_ProcAddress _gfx_platform_get_proc_address(

		const char* proc)
{
	return (GFX_ProcAddress)glXGetProcAddressARB((const GLubyte*)proc);
}
