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
	unsigned int index = _gfx_win32->numMonitors++;
	_gfx_win32->monitors = (HMONITOR*)realloc(
		_gfx_win32->monitors,
		sizeof(HMONITOR) * _gfx_win32->numMonitors);

	_gfx_win32->monitors[index] = handle;

	return 1;
}

/******************************************************/
static GFXKey _gfx_win32_get_key(int symbol)
{
	/* Unicode numbers */
	if(symbol >= 0x30 && symbol <= 0x39) return (GFXKey)(symbol - 0x30 + GFX_KEY_0);

	/* Unicode capitals */
	if(symbol >= 0x41 && symbol <= 0x5a) return (GFXKey)(symbol - 0x41 + GFX_KEY_A);

	/* Function keys */
	if(symbol >= VK_F1 && symbol <= VK_F24) return (GFXKey)(symbol - VK_F1 + GFX_KEY_F1);

	/* Non-unicode */
	switch(symbol)
	{
		case VK_BACK      : return GFX_KEY_BACKSPACE;
		case VK_TAB       : return GFX_KEY_TAB;
		case VK_CLEAR     : return GFX_KEY_CLEAR;
		case VK_RETURN    : return GFX_KEY_RETURN;
		case VK_PAUSE     : return GFX_KEY_PAUSE;
		case VK_SCROLL    : return GFX_KEY_SCROLL_LOCK;
		case VK_ESCAPE    : return GFX_KEY_ESCAPE;
		case VK_DELETE    : return GFX_KEY_DELETE;

		case VK_HOME      : return GFX_KEY_HOME;
		case VK_LEFT      : return GFX_KEY_LEFT;
		case VK_UP        : return GFX_KEY_UP;
		case VK_RIGHT     : return GFX_KEY_RIGHT;
		case VK_DOWN      : return GFX_KEY_DOWN;
		case VK_PRIOR     : return GFX_KEY_PAGE_UP;
		case VK_NEXT      : return GFX_KEY_PAGE_DOWN;
		case VK_END       : return GFX_KEY_END;

		case VK_SELECT    : return GFX_KEY_SELECT;
		case VK_PRINT     : return GFX_KEY_PRINT;
		case VK_EXECUTE   : return GFX_KEY_EXECUTE;
		case VK_INSERT    : return GFX_KEY_INSERT;
		case VK_LMENU     : return GFX_KEY_MENU;
		case VK_RMENU     : return GFX_KEY_MENU;
		case VK_CANCEL    : return GFX_KEY_CANCEL;
		case VK_HELP      : return GFX_KEY_HELP;
		case VK_NUMLOCK   : return GFX_KEY_NUM_LOCK;
		case VK_SPACE     : return GFX_KEY_SPACE;

		case VK_NUMPAD0   : return GFX_KEY_KP_0;
		case VK_NUMPAD1   : return GFX_KEY_KP_1;
		case VK_NUMPAD2   : return GFX_KEY_KP_2;
		case VK_NUMPAD3   : return GFX_KEY_KP_3;
		case VK_NUMPAD4   : return GFX_KEY_KP_4;
		case VK_NUMPAD5   : return GFX_KEY_KP_5;
		case VK_NUMPAD6   : return GFX_KEY_KP_6;
		case VK_NUMPAD7   : return GFX_KEY_KP_7;
		case VK_NUMPAD8   : return GFX_KEY_KP_8;
		case VK_NUMPAD9   : return GFX_KEY_KP_9;
		case VK_MULTIPLY  : return GFX_KEY_KP_MULTIPLY;
		case VK_ADD       : return GFX_KEY_KP_ADD;
		case VK_SEPARATOR : return GFX_KEY_KP_SEPARATOR;
		case VK_SUBTRACT  : return GFX_KEY_KP_SUBTRACT;
		case VK_DECIMAL   : return GFX_KEY_KP_DECIMAL;
		case VK_DIVIDE    : return GFX_KEY_KP_DIVIDE;

		case VK_SHIFT     : return GFX_KEY_SHIFT_LEFT;
		case VK_LSHIFT    : return GFX_KEY_SHIFT_LEFT;
		case VK_RSHIFT    : return GFX_KEY_SHIFT_RIGHT;
		case VK_CONTROL   : return GFX_KEY_CONTROL_LEFT;
		case VK_LCONTROL  : return GFX_KEY_CONTROL_LEFT;
		case VK_RCONTROL  : return GFX_KEY_CONTROL_RIGHT;
		case VK_MENU      : return GFX_KEY_ALT_LEFT;
		case VK_LWIN      : return GFX_KEY_SUPER_LEFT;
		case VK_RWIN      : return GFX_KEY_SUPER_RIGHT;
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
