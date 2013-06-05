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
		case WM_CLOSE :
		{
			_gfx_platform_destroy_window(handle);
			return 0;
		}
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
static void _gfx_win32_add_window(void* handle, void* monitor)
{
	if(_gfx_win32)
	{
		unsigned int index = _gfx_win32->numWindows++;
		size_t siz = sizeof(void*) * _gfx_win32->numWindows;

		/* Add window pointer */
		_gfx_win32->windows = (void**)realloc(_gfx_win32->windows, siz);
		_gfx_win32->windows[index] = handle;

		/* Add window monitor */
		_gfx_win32->windowMonitors = (void**)realloc(_gfx_win32->windowMonitors, siz);
		_gfx_win32->windowMonitors[index] = monitor;
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
			free(_gfx_win32->windowMonitors);
			_gfx_win32->windowClass = NULL;
			_gfx_win32->windows = NULL;
			_gfx_win32->windowMonitors = NULL;
		}
		else
		{
			/* Move elements and resize array */
			size_t mov = sizeof(void*) * (_gfx_win32->numWindows - i);
			size_t siz = sizeof(void*) * _gfx_win32->numWindows;

			/* Move window pointers */
			void** dest = _gfx_win32->windows + i;
			memmove(dest, dest + 1, mov);
			_gfx_win32->windows = (void**)realloc(_gfx_win32->windows, siz);

			/* Move window monitors */
			dest = _gfx_win32->windowMonitors + i;
			memmove(dest, dest + 1, mov);
			_gfx_win32->windowMonitors = (void**)realloc(_gfx_win32->windowMonitors, siz);
		}
		break;
	}
}

/******************************************************/
static void* _gfx_win32_window_get_monitor(void* handle)
{
	unsigned int i;
	if(_gfx_win32) for(i = 0; i < _gfx_win32->numWindows; ++i)
		if(_gfx_win32->windows[i] == handle) return _gfx_win32->windowMonitors[i];

	return NULL;
}

/******************************************************/
unsigned int _gfx_platform_get_num_windows(void)
{
	if(!_gfx_win32) return 0;
	return _gfx_win32->numWindows;
}

/******************************************************/
void* _gfx_platform_get_window(unsigned int num)
{
	if(!_gfx_win32) return NULL;

	/* Validate the number first */
	if(num >= _gfx_win32->numWindows) return NULL;
	return _gfx_win32->windows[num];
}

/******************************************************/
void* _gfx_platform_create_window(const GFX_Platform_Attributes* attributes)
{
	/* Make sure to register the window class */
	if(!_gfx_win32_register_window_class()) return NULL;

	/* Get monitor information */
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(attributes->screen, &info);

	/* Make sure to convert to wide character */
	wchar_t* name = utf8_to_wchar(attributes->name);
	HWND window = CreateWindowEx(
		0,
		GFX_WIN32_WND_CLASS,
		name,
		WS_OVERLAPPEDWINDOW,
		attributes->x + info.rcMonitor.left,
		attributes->y + info.rcMonitor.top,
		attributes->width,
		attributes->height,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL
	);

	free(name);
	if(!window) return NULL;

	/* Add window to array */
	_gfx_win32_add_window(window, attributes->screen);

	return (void*)window;
}

/******************************************************/
void _gfx_platform_destroy_window(void* handle)
{
	/* First destroy its context */
	_gfx_platform_window_destroy_context(handle);

	/* Destroy and remove the handle */
	DestroyWindow(handle);
	_gfx_win32_remove_window(handle);
}

/******************************************************/
void _gfx_platform_window_set_size(void* handle, unsigned int width, unsigned int height)
{
	SetWindowPos(handle, NULL, 0, 0, width, height, SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOZORDER);
}

/******************************************************/
void _gfx_platform_window_set_position(void* handle, int x, int y)
{
	/* Get window's monitor position */
	HMONITOR monitor = (HMONITOR)_gfx_win32_window_get_monitor(handle);
	if(monitor)
	{
		MONITORINFO info;
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(monitor, &info);
		x += info.rcMonitor.left;
		y += info.rcMonitor.top;
	}
	SetWindowPos(handle, NULL, x, y, 0, 0, SWP_NOCOPYBITS | SWP_NOSIZE | SWP_NOZORDER);
}

/******************************************************/
void _gfx_platform_window_show(void* handle)
{
	ShowWindow(handle, SW_SHOW);
}

/******************************************************/
void _gfx_platform_window_hide(void* handle)
{
	ShowWindow(handle, SW_HIDE);
}
