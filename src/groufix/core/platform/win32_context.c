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

#define GL_GLEXT_PROTOTYPES
#include "groufix/core/internal.h"
#include "groufix/core/platform/win32.h"

/******************************************************/
int _gfx_platform_context_create(

		GFX_PlatformWindow  handle,
		int                 major,
		int                 minor,
		GFX_PlatformWindow  share)
{
	/* Get the windows */
	GFX_Win32_Window* window = _gfx_win32_get_window_from_handle(handle);
	if(!window) return 0;

	GFX_Win32_Window* shareWind = NULL;
	if(share) shareWind = _gfx_win32_get_window_from_handle(share);

	/* Create buffer attribute array */
	int bufferAttr[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,
		WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	/* Create the context */
	HGLRC shareCont = NULL;
	if(shareWind) shareCont = shareWind->context;

	_gfx_win32->current = GetDC(handle);
	window->context = _gfx_win32->extensions.CreateContextAttribsARB(
		_gfx_win32->current,
		shareCont,
		bufferAttr
	);

	/* Make it current */
	if(window->context)
	{
		wglMakeCurrent(_gfx_win32->current, window->context);
		return 1;
	}
	return 0;
}

/******************************************************/
void _gfx_platform_context_free(

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
void _gfx_platform_context_make_current(

		GFX_PlatformWindow handle)
{
	GFX_Win32_Window* window =
		_gfx_win32_get_window_from_handle(handle);

	if(window)
	{
		_gfx_win32->current = GetDC(handle);
		wglMakeCurrent(_gfx_win32->current, window->context);
	}
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
void _gfx_platform_context_swap_buffers(void)
{
	if(_gfx_win32) SwapBuffers(_gfx_win32->current);
}

/******************************************************/
int _gfx_platform_is_extension_supported(

		GFX_PlatformWindow  handle,
		const char*         ext)
{
	if(!_gfx_win32) return 0;

	/* Get extensions */
	const char* extensions =
		_gfx_win32->extensions.GetExtensionsStringARB(GetDC(handle));

	if(!extensions) return 0;
	return _gfx_extensions_is_in_string(extensions, ext);
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
