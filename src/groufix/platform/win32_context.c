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

#include "groufix/platform/win32.h"

/******************************************************/
int _gfx_platform_context_create(GFX_Platform_Window handle, int major, int minor, GFX_Platform_Window share)
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

	HDC dc = GetDC(handle);
	window->context = _gfx_win32->extensions.CreateContextAttribsARB(dc, shareCont, bufferAttr);

	return window->context ? 1 : 0;
}

/******************************************************/
void _gfx_platform_context_free(GFX_Platform_Window handle)
{
	HDC dc = GetDC(handle);

	/* Get the window and destroy its context */
	GFX_Win32_Window* window = _gfx_win32_get_window_from_handle(handle);
	if(window)
	{
		wglDeleteContext(window->context);
		window->context = NULL;
	}
	ReleaseDC(handle, dc);
}

/******************************************************/
void _gfx_platform_context_make_current(GFX_Platform_Window handle)
{
	GFX_Win32_Window* window = _gfx_win32_get_window_from_handle(handle);
	if(window) wglMakeCurrent(GetDC(handle), window->context);
}

/******************************************************/
void _gfx_platform_context_set_swap_interval(GFX_Platform_Window handle, int num)
{
	if(_gfx_win32->extensions.SwapIntervalEXT)
	{
		/* Make current to set its interval */
		_gfx_platform_context_make_current(handle);
		_gfx_win32->extensions.SwapIntervalEXT(num);
	}
}

/******************************************************/
void _gfx_platform_context_swap_buffers(GFX_Platform_Window handle)
{
	SwapBuffers(GetDC(handle));
}

/******************************************************/
int _gfx_platform_is_extension_supported(GFX_Platform_Window handle, const char* ext)
{
	if(!_gfx_win32) return 0;

	/* Get extensions */
	const char* extensions = _gfx_win32->extensions.GetExtensionsStringARB(GetDC(handle));
	if(!extensions) return 0;

	return _gfx_extensions_is_in_string(extensions, ext);
}

/******************************************************/
GFXProcAddress _gfx_platform_get_proc_address(const char* proc)
{
	GFXProcAddress address = (GFXProcAddress)wglGetProcAddress(proc);
	if(address) return address;
	
	return (GFXProcAddress)GetProcAddress(GetModuleHandle(NULL), proc);
}
