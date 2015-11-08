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

#include "groufix/core/platform/win32.h"

#if defined(GFX_RENDERER_GL)

/******************************************************/
static GFX_Win32_Window* _gfx_win32_get_window_from_context(

		HGLRC context)
{
	GFX_Win32_Window* it;
	for(
		it = _gfx_win32.windows.begin;
		it != _gfx_win32.windows.end;
		it = gfx_vector_next(&_gfx_win32.windows, it))
	{
		if(it->context == context) break;
	}

	return it != _gfx_win32.windows.end ? it : NULL;
}

/******************************************************/
static HGLRC _gfx_win32_create_context(

		int    major,
		int    minor,
		HWND   window,
		HGLRC  share,
		int    debug)
{
	/* Create buffer attribute array */
	int flags =
		WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB |
		(debug ? WGL_CONTEXT_DEBUG_BIT_ARB : 0);

	int bufferAttr[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,
		WGL_CONTEXT_FLAGS_ARB,         flags,
		WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	/* Create the context */
	return _gfx_win32.extensions.CreateContextAttribsARB(
		GetDC(window),
		share,
		bufferAttr
	);
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
	/* Create dummy window */
	GFX_Win32_Window* window = _gfx_win32_window_dummy_create();
	if(!window) return NULL;

	GFX_PlatformContext context = NULL;

#if defined(GFX_RENDERER_GL)
	/* Create context */
	window->context = _gfx_win32_create_context(
		major,
		minor,
		window->handle,
		share,
		debug
	);

	if(!window->context)
		_gfx_platform_window_free(window->handle);

	context = window->context;
#endif

	/* Return dummy window and context */
	*handle = window->handle;
	return context;
}

/******************************************************/
void _gfx_platform_context_free(

		GFX_PlatformContext context)
{
#if defined(GFX_RENDERER_GL)
	/* Get the window and free it */
	GFX_Win32_Window* window = _gfx_win32_get_window_from_context(context);
	if(window) _gfx_platform_window_free(window->handle);
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
	GFX_Win32_Window* window = _gfx_win32_get_window_from_handle(handle);
	if(!window) return NULL;

	GFX_PlatformContext context = NULL;

#if defined(GFX_RENDERER_GL)
	/* Create context */
	window->context = _gfx_win32_create_context(
		major,
		minor,
		handle,
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
	GFX_Win32_Window* window =
		_gfx_win32_get_window_from_handle(handle);

	if(window)
	{
#if defined(GFX_RENDERER_GL)
		wglDeleteContext(window->context);
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
	/* First get window */
	GFX_Win32_Window* window =
		_gfx_win32_get_window_from_handle(handle);

	if(!window || !_gfx_win32.extensions.SwapIntervalEXT)
		return 0;

	/* Correct if adaptive vsync is not supported */
	if(!_gfx_win32.extensions.EXT_swap_control_tear && num < 0)
		num = -num;

	/* Make current to set its interval */
	_gfx_platform_context_make_current(handle, window->context);
	_gfx_win32.extensions.SwapIntervalEXT(num);
#endif

	return num;
}

/******************************************************/
void _gfx_platform_context_swap_buffers(

		GFX_PlatformWindow handle)
{
#if defined(GFX_RENDERER_GL)
	SwapBuffers(GetDC(handle));
#endif
}

/******************************************************/
void _gfx_platform_context_make_current(

		GFX_PlatformWindow   handle,
		GFX_PlatformContext  context)
{
#if defined(GFX_RENDERER_GL)
	if(!context)
		wglMakeCurrent(NULL, NULL);

	else if(handle)
		wglMakeCurrent(GetDC(handle), context);
#endif
}

/******************************************************/
GFX_ProcAddress _gfx_platform_get_proc_address(

		const char* proc)
{
	GFX_ProcAddress addr = NULL;

#if defined(GFX_RENDERER_GL)
	addr =
		(GFX_ProcAddress)wglGetProcAddress(proc);
	if(!addr) addr =
		(GFX_ProcAddress)GetProcAddress(GetModuleHandle(NULL), proc);
#endif

	return addr;
}
