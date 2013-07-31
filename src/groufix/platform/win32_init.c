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
static int _gfx_win32_window_compare_handle(const VectorIterator it, const void* value)
{
	return ((GFX_Win32_Window*)it)->handle == value;
}

/******************************************************/
VectorIterator _gfx_win32_get_window_from_handle(HWND handle)
{
	if(!_gfx_win32) return NULL;

	VectorIterator found = vector_find(_gfx_win32->windows, handle, _gfx_win32_window_compare_handle);

	return found != _gfx_win32->windows->end ? found : NULL;
}

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
static int _gfx_win32_load_extensions(void)
{
	int success = 1;

	/* Create a dummy window */
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(WNDCLASS));
	wc.lpfnWndProc   = DefWindowProc;
	wc.hInstance     = GetModuleHandle(NULL);
	wc.lpszClassName = GFX_WIN32_WND_CLASS;

	if(!RegisterClass(&wc)) return 0;
	HWND window = CreateWindow(
		GFX_WIN32_WND_CLASS, L"", 0, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);

	/* Setup context properties */
	GFXColorDepth depth;
	depth.redBits   = 0;
	depth.greenBits = 0;
	depth.blueBits  = 0;

	_gfx_win32_set_pixel_format(window, &depth);
	HDC dc = GetDC(window);

	/* Create a dummy render context */
	HGLRC context = wglCreateContext(dc);
	if(!context) success = 0;

	wglMakeCurrent(dc, context);

	/* Get extension string extension.. */
	_gfx_win32->extensions.GetExtensionsStringARB =
		(PFNWGLGETEXTENSIONSSTRINGARBPROC)_gfx_platform_get_proc_address("wglGetExtensionsStringARB");

	if(_gfx_win32->extensions.GetExtensionsStringARB)
	{
		/* Check all vital extensions */
		if(
			!_gfx_platform_is_extension_supported(NULL, "WGL_ARB_create_context") ||
			!_gfx_platform_is_extension_supported(NULL, "WGL_ARB_create_context_profile")
		) success = 0;

		/* Load all functions */
		_gfx_win32->extensions.CreateContextAttribsARB =
			(PFNWGLCREATECONTEXTATTRIBSARBPROC)_gfx_platform_get_proc_address("wglCreateContextAttribsARB");
		_gfx_win32->extensions.SwapIntervalEXT =
			(PFNWGLSWAPINTERVALEXTPROC)_gfx_platform_get_proc_address("wglSwapIntervalEXT");

		/* Check non-vital extensions */
		if(!_gfx_platform_is_extension_supported(NULL, "WGL_EXT_swap_control"))
			_gfx_win32->extensions.SwapIntervalEXT = NULL;
	}
	else success = 0;

	/* Destroy dummy context and window */
	wglDeleteContext(context);
	DestroyWindow(window);
	UnregisterClass(GFX_WIN32_WND_CLASS, GetModuleHandle(NULL));

	return success;
}

/******************************************************/
static GFXKey _gfx_win32_get_key(int symbol)
{
	/* Unicode numbers */
	if(symbol >= 0x30 && symbol <= 0x39) return (GFXKey)(symbol - 0x30 + GFX_KEY_0);

	/* Keypad numbers */
	if(symbol >= VK_NUMPAD0 && symbol <= VK_NUMPAD9) return (GFXKey)(symbol - VK_NUMPAD0 + GFX_KEY_KP_0);

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
static BOOL CALLBACK _gfx_win32_monitor_proc(HMONITOR handle, HDC hdc, LPRECT rect, LPARAM data)
{
	/* Simply store the monitor handle */
	return vector_insert(_gfx_win32->monitors, &handle, _gfx_win32->monitors->end) != _gfx_win32->monitors->end;
}

/******************************************************/
int _gfx_platform_init(void)
{
	if(!_gfx_win32)
	{
		/* Allocate */
		_gfx_win32 = (GFX_Win32_Instance*)calloc(1, sizeof(GFX_Win32_Instance));
		if(!_gfx_win32) return 0;

		/* Setup memory and load extensions */
		_gfx_win32->monitors = vector_create(sizeof(HMONITOR));
		_gfx_win32->windows = vector_create(sizeof(GFX_Win32_Window));
		if(!_gfx_win32->monitors || !_gfx_win32->windows || !_gfx_win32_load_extensions())
		{
			_gfx_platform_terminate();
			return 0;
		}

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
		unsigned int i = vector_get_size(_gfx_win32->windows);
		while(i--)
		{
			VectorIterator it = vector_previous(_gfx_win32->windows, _gfx_win32->windows->end);
			_gfx_platform_destroy_window(((GFX_Win32_Window*)it)->handle);
		}
		vector_free(_gfx_win32->monitors);
		vector_free(_gfx_win32->windows);

		/* Unregister window class */
		UnregisterClass(GFX_WIN32_WND_CLASS, GetModuleHandle(NULL));

		/* Deallocate instance */
		free(_gfx_win32);
		_gfx_win32 = NULL;
	}
}
