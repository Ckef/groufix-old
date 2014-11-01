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
#include "groufix/core/platform/win32.h"

/******************************************************/
static HGLRC _gfx_win32_create_context(

		int    major,
		int    minor,
		HWND   window,
		HGLRC  share)
{
	/* Create buffer attribute array */
	int bufferAttr[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,
		WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
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

		int                  major,
		int                  minor,
		GFX_PlatformContext  share)
{
	/* Create dummy window */
	GFX_Win32_Window* window = _gfx_win32_window_dummy_create();
	if(!window) return NULL;

	/* Create context */
	window->context = _gfx_win32_create_context(
		major,
		minor,
		window->handle,
		share
	);

	if(!window->context)
		_gfx_platform_window_free(window->handle);

	return window->context;
}

/******************************************************/
void _gfx_platform_context_free(

		GFX_PlatformContext context)
{
	/* Get the window and free it */
	GFX_Win32_Window* window =_gfx_win32_get_window_from_context(context);
	if(window) _gfx_platform_window_free(window->handle);
}

/******************************************************/
GFX_PlatformContext _gfx_platform_context_init(

		GFX_PlatformWindow   handle,
		int                  major,
		int                  minor,
		GFX_PlatformContext  share)
{
	/* Get the window */
	GFX_Win32_Window* window = _gfx_win32_get_window_from_handle(handle);
	if(!window) return NULL;

	/* Create context */
	window->context = _gfx_win32_create_context(
		major,
		minor,
		handle,
		share
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
	/* Correct if adaptive vsync is not supported */
	if(!_gfx_win32->extensions.EXT_swap_control_tear && num < 0)
		num = -num;

	if(_gfx_win32->extensions.SwapIntervalEXT)
	{
		/* Make current to set its interval */
		_gfx_platform_context_make_current(handle);
		_gfx_win32->extensions.SwapIntervalEXT(num);
	}

	return num;
}

/******************************************************/
void _gfx_platform_context_swap_buffers(

		GFX_PlatformWindow handle)
{
	if(_gfx_win32) SwapBuffers(GetDC(handle));
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

		GFX_PlatformContext context)
{
	if(!context)
		wglMakeCurrent(NULL, NULL);

	else
	{
		GFX_Win32_Window* window =
			_gfx_win32_get_window_from_context(context);

		if(window) wglMakeCurrent(
			GetDC(window->handle),
			window->context
		);
	}
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
