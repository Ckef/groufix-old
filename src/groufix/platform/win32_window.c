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

#include "groufix/platform.h"

#include <windows.h>
#include <stdlib.h>
#include <string.h>

/******************************************************/
static LRESULT CALLBACK _gfx_win32_window_proc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	}

	return DefWindowProc(handle, msg, wParam, lParam);
}

/******************************************************/
static int _gfx_win32_register_window_class(void)
{
	if(!_gfx_win32) return 0;

	/* Check if it is already registered */
	if(!_gfx_win32->windowClass)
	{
		_gfx_win32->windowClass = malloc(sizeof(WNDCLASSEX));
		WNDCLASSEX* wc = (WNDCLASSEX*)_gfx_win32->windowClass;

		wc->cbSize        = sizeof(WNDCLASSEX);
		wc->style         = 0;
		wc->lpfnWndProc   = _gfx_win32_window_proc;
		wc->cbClsExtra    = 0;
		wc->cbWndExtra    = 0;
		wc->hInstance     = GetModuleHandle(NULL);
		wc->hIcon         = NULL;
		wc->hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc->hbrBackground = NULL;
		wc->lpszMenuName  = NULL;
		wc->lpszClassName = GFX_WIN32_WND_CLASS;
		wc->hIconSm       = NULL;

		if(!RegisterClassEx(wc))
		{
			free(_gfx_win32->windowClass);
			_gfx_win32->windowClass = NULL;

			return 0;
		}
	}
	return 1;
}

/******************************************************/
static void _gfx_win32_add_window(void* handle)
{
	if(_gfx_win32)
	{
		++_gfx_win32->numWindows;
		_gfx_win32->windows = (void**)realloc(_gfx_win32->windows, sizeof(void*) * _gfx_win32->numWindows);
		_gfx_win32->windows[_gfx_win32->numWindows - 1] = (void*)handle;
	}
}

/******************************************************/
static void _gfx_win32_remove_window(void* handle)
{
	/* Remove the handle from the array */
	unsigned int i;
	if(_gfx_win32) for(i = 0; i < _gfx_win32->numWindows; ++i)
		if(_gfx_win32->windows[i] == handle)
	{
		--_gfx_win32->numWindows;
		if(!_gfx_win32->numWindows)
		{
			/* Also unregister the window class */
			UnregisterClass(GFX_WIN32_WND_CLASS, GetModuleHandle(NULL));
			free(_gfx_win32->windowClass);
			free(_gfx_win32->windows);
			_gfx_win32->windowClass = NULL;
			_gfx_win32->windows = NULL;
		}
		else
		{
			/* Move elements and resize array */
			void** dest = _gfx_win32->windows + i;
			memmove(dest, dest + 1, sizeof(void*) * (_gfx_win32->numWindows - i));
			_gfx_win32->windows = (void**)realloc(_gfx_win32->windows, sizeof(void*) * _gfx_win32->numWindows);
		}
		break;
	}
}

/******************************************************/
void* _gfx_platform_create_window(const GFX_Platform_Attributes* attributes)
{
	/* Make sure to register the window class */
	if(!_gfx_win32_register_window_class()) return NULL;

	/* Get screen position */
	MONITORINFO info;
	ZeroMemory(&info, sizeof(MONITORINFO));
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo((HMONITOR)attributes->screen, &info);

	HWND window = CreateWindowEx(
		0,
		GFX_WIN32_WND_CLASS,
		attributes->name,
		0,
		attributes->x + info.rcMonitor.left,
		attributes->y + info.rcMonitor.top,
		attributes->width,
		attributes->height,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL
	);

	if(!window) return NULL;
	ShowWindow(window, SW_SHOWDEFAULT);

	/* Add window to array */
	_gfx_win32_add_window(window);

	return (void*)window;
}

/******************************************************/
void _gfx_platform_destroy_window(void* handle)
{
	/* First destroy its context */
	_gfx_platform_destroy_context(handle);

	DestroyWindow((HWND)handle);
	_gfx_win32_remove_window(handle);
}

/******************************************************/
int _gfx_platform_create_context(void* handle)
{
	return 0;
}

//******************************************************/
void _gfx_platform_destroy_context(void* handle)
{
}
