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
int _gfx_platform_create_context(GFX_Platform_Window handle)
{
	HDC dc = GetDC(handle);

	/* Get the window */
	GFX_Win32_Window* window = _gfx_win32_get_window_from_handle(handle);
	if(!window) return 0;

	/* Create the context */
	window->context = wglCreateContext(dc);
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
void _gfx_platform_context_make_current(GFX_Platform_Window handle)
{
	HGLRC cont = gfx_win32_get_context(handle);
	if(cont) wglMakeCurrent(GetDC(handle), cont);
}

/******************************************************/
void _gfx_platform_context_swap_buffers(GFX_Platform_Window handle)
{
	SwapBuffers(GetDC(handle));
}