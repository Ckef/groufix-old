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

#include "groufix/core/platform/win32.h"

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
	return _gfx_win32->extensions.CreateContextAttribsARB(
		GetDC(window),
		share,
		bufferAttr
	);
}

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

	/* Return dummy window and context */
	*handle = window->handle;
	return window->context;
}

/******************************************************/
void _gfx_platform_context_free(

		GFX_PlatformContext context)
{
	/* Get the window and free it */
	GFX_Win32_Window* window = _gfx_win32_get_window_from_context(context);
	if(window) _gfx_platform_window_free(window->handle);
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

	/* Create context */
	window->context = _gfx_win32_create_context(
		major,
		minor,
		handle,
		share,
		debug
	);

	return window->context;
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
		wglDeleteContext(window->context);
		window->context = NULL;
	}
}

/******************************************************/
int _gfx_platform_context_set_swap_interval(

		GFX_PlatformWindow  handle,
		int                 num)
{
	/* First get window */
	GFX_Win32_Window* window =
		_gfx_win32_get_window_from_handle(handle);

	if(!window || !_gfx_win32->extensions.SwapIntervalEXT)
		return 0;

	/* Correct if adaptive vsync is not supported */
	if(!_gfx_win32->extensions.EXT_swap_control_tear && num < 0)
		num = -num;

	/* Make current to set its interval */
	_gfx_platform_context_make_current(handle, window->context);
	_gfx_win32->extensions.SwapIntervalEXT(num);

	return num;
}

/******************************************************/
void _gfx_platform_context_swap_buffers(

		GFX_PlatformWindow handle)
{
	if(_gfx_win32) SwapBuffers(GetDC(handle));
}

/******************************************************/
void _gfx_platform_context_make_current(

		GFX_PlatformWindow   handle,
		GFX_PlatformContext  context)
{
	if(!context)
		wglMakeCurrent(NULL, NULL);

	else if(handle)
		wglMakeCurrent(GetDC(handle), context);
}

/******************************************************/
GFX_ProcAddress _gfx_platform_get_proc_address(

		const char* proc)
{
	GFX_ProcAddress address =
		(GFX_ProcAddress)wglGetProcAddress(proc);

	if(address) return address;
	return (GFX_ProcAddress)GetProcAddress(GetModuleHandle(NULL), proc);
}
