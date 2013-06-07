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

/******************************************************/
GFX_Win32_Instance* _gfx_win32 = NULL;

/******************************************************/
wchar_t* utf8_to_wchar(const char* str)
{
	/* First get the required length in characters */
	int length = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	if(!length) return NULL;

	wchar_t* out = (wchar_t*)malloc(sizeof(wchar_t) * length);
	if(!MultiByteToWideChar(CP_UTF8, 0, str, -1, out, length))
	{
		free(out);
		return NULL;
	}
	return out;
}

/******************************************************/
char* wchar_to_utf8(const wchar_t* str)
{
	/* First get the required length in bytes */
	int length = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	if(!length) return NULL;

	char* out = (char*)malloc(length);
	if(!WideCharToMultiByte(CP_UTF8, 0, str, -1, out, length, NULL, NULL))
	{
		free(out);
		return NULL;
	}
	return out;
}

/******************************************************/
static BOOL CALLBACK _gfx_win32_monitor_proc(HMONITOR handle, HDC hdc, LPRECT rect, LPARAM data)
{
	/* Simply store the monitor handle */
	++_gfx_win32->numMonitors;
	_gfx_win32->monitors = (void**)realloc(_gfx_win32->monitors, sizeof(void*) * _gfx_win32->numMonitors);
	_gfx_win32->monitors[_gfx_win32->numMonitors - 1] = (void*)handle;

	return 1;
}

/******************************************************/
static GFXKey _gfx_win32_get_key(int symbol)
{
	switch(symbol)
	{
		case VK_BACK : return GFX_KEY_BACKSPACE;
	}

	return GFX_KEY_UNKNOWN;
}

/******************************************************/
static void _gfx_win32_create_key_table(void)
{
	size_t i;
	for(i = 0; i <= GFX_WIN32_MAX_KEYCODE; ++i)
		_gfx_win32->keys[i] = _gfx_win32_get_key(i);
}

/******************************************************/
int _gfx_platform_init(void)
{
	if(!_gfx_win32)
	{
		/* Allocate */
		_gfx_win32 = (GFX_Win32_Instance*)calloc(1, sizeof(GFX_Win32_Instance));

		/* Enumerate all monitors */
		if(!EnumDisplayMonitors(NULL, NULL, _gfx_win32_monitor_proc, 0))
		{
			_gfx_platform_terminate();
			return 0;
		}

		/* Construct a keycode lookup */
		_gfx_win32_create_key_table();
	}
	return 1;
}

/******************************************************/
int _gfx_platform_is_initialized(void)
{
	return (size_t)_gfx_win32;
}

/******************************************************/
void _gfx_platform_terminate(void)
{
	if(_gfx_win32)
	{
		/* Destroy all windows */
		while(_gfx_win32->numWindows) _gfx_platform_destroy_window(_gfx_win32->windows[0]);

		/* Unregister window class */
		UnregisterClass(GFX_WIN32_WND_CLASS, GetModuleHandle(NULL));

		/* Deallocate instance */
		free(_gfx_win32->monitors);
		free(_gfx_win32);
		_gfx_win32 = NULL;
	}
}
