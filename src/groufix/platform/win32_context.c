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

#include <string.h>

/******************************************************/
int _gfx_platform_create_context(GFX_Platform_Window handle, unsigned short major, unsigned short minor)
{
	HDC dc = GetDC(handle);

	/* Get the window */
	GFX_Win32_Window* window = _gfx_win32_get_window_from_handle(handle);
	if(!window) return 0;

	/* Create buffer attribute array */
	int bufferAttr[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,
		WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	/* Create the context */
	window->context = _gfx_win32->extensions.CreateContextAttribsARB(
		dc,
		NULL,
		bufferAttr
	);

	if(!window->context) return 0;
	wglMakeCurrent(dc, window->context);

	return 1;
}

/******************************************************/
void _gfx_platform_destroy_context(GFX_Platform_Window handle)
{
	HDC dc = GetDC(handle);

	/* Get the window and destroy its context */
	GFX_Win32_Window* window = _gfx_win32_get_window_from_handle(handle);
	if(window)
	{
		wglMakeCurrent(dc, NULL);
		wglDeleteContext(window->context);

		window->context = NULL;
	}
	ReleaseDC(handle, dc);
}

/******************************************************/
int _gfx_platform_context_get(GFX_Platform_Window handle, GFX_Platform_Context* type, unsigned short* major, unsigned short* minor)
{
	if(!_gfx_platform_context_make_current(handle)) return 0;

	int ma, mi;
	glGetIntegerv(GL_MAJOR_VERSION, &ma);
	glGetIntegerv(GL_MINOR_VERSION, &mi);

	*type = GFX_CONTEXT_OPENGL;
	*major = ma;
	*minor = mi;

	return 1;
}

/******************************************************/
int _gfx_platform_context_make_current(GFX_Platform_Window handle)
{
	GFX_Win32_Window* window = _gfx_win32_get_window_from_handle(handle);
	if(window) return wglMakeCurrent(GetDC(handle), window->context);

	return 0;
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

	return _gfx_platform_is_extension_in_string(extensions, ext);
}

/******************************************************/
void* _gfx_platform_get_proc_address(const char* proc)
{
	return (void*)wglGetProcAddress(proc);
}
