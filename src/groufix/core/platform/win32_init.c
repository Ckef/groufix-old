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

#include "groufix/core/errors.h"
#include "groufix/core/platform/win32.h"

#include <stdlib.h>

/******************************************************/
/* Instance */
GFX_Win32_Instance* _gfx_win32 = NULL;

/* Window class name */
LPCTSTR _gfx_win32_window_class = "GROUFIX";

/******************************************************/
GFXVectorIterator _gfx_win32_get_window_from_handle(

		HWND handle)
{
	if(!_gfx_win32) return NULL;

	GFXVectorIterator it;
	for(
		it = _gfx_win32->windows.begin;
		it != _gfx_win32->windows.end;
		it = gfx_vector_next(&_gfx_win32->windows, it))
	{
		if(((GFX_Win32_Window*)it)->handle == handle) break;
	}

	return it != _gfx_win32->windows.end ? it : NULL;
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
	wc.lpszClassName = _gfx_win32_window_class;

	if(!RegisterClass(&wc)) return 0;
	HWND window = CreateWindow(
		_gfx_win32_window_class,
		"", 0, 0, 0, 0, 0, NULL, NULL,
		GetModuleHandle(NULL),
		NULL
	);

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
		(PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");

	if(_gfx_win32->extensions.GetExtensionsStringARB)
	{
		/* Check all vital extensions */
		if(
			!_gfx_platform_is_extension_supported(NULL, "WGL_ARB_create_context") ||
			!_gfx_platform_is_extension_supported(NULL, "WGL_ARB_create_context_profile"))
		{
			gfx_errors_push(GFX_ERROR_INCOMPATIBLE_CONTEXT, "Vital WGL extensions are missing.");
			success = 0;
		}

		/* Load all functions */
		_gfx_win32->extensions.CreateContextAttribsARB =
			(PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
		_gfx_win32->extensions.SwapIntervalEXT =
			(PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		_gfx_win32->extensions.EXT_swap_control_tear =
			_gfx_platform_is_extension_supported(NULL, "WGL_EXT_swap_control_tear") ? 1 : 0;

		/* Check non-vital extensions */
		if(!_gfx_platform_is_extension_supported(NULL, "WGL_EXT_swap_control"))
			_gfx_win32->extensions.SwapIntervalEXT = NULL;
	}
	else success = 0;

	/* Destroy dummy context and window */
	wglDeleteContext(context);
	DestroyWindow(window);
	UnregisterClass(_gfx_win32_window_class, GetModuleHandle(NULL));

	return success;
}

/******************************************************/
static GFXKey _gfx_win32_get_key(

		int symbol)
{
	/* Unicode numbers */
	if(symbol >= 0x30 && symbol <= 0x39)
		return (GFXKey)(symbol - 0x30 + GFX_KEY_0);

	/* Keypad numbers */
	if(symbol >= VK_NUMPAD0 && symbol <= VK_NUMPAD9)
		return (GFXKey)(symbol - VK_NUMPAD0 + GFX_KEY_KP_0);

	/* Unicode capitals */
	if(symbol >= 0x41 && symbol <= 0x5a)
		return (GFXKey)(symbol - 0x41 + GFX_KEY_A);

	/* Function keys */
	if(symbol >= VK_F1 && symbol <= VK_F24)
		return (GFXKey)(symbol - VK_F1 + GFX_KEY_F1);

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
static inline void _gfx_win32_create_key_table(void)
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
		_gfx_win32 = calloc(1, sizeof(GFX_Win32_Instance));
		if(!_gfx_win32) return 0;

		/* Load extensions and setup memory */
		if(!_gfx_win32_load_extensions())
		{
			free(_gfx_win32);
			return 0;
		}
		gfx_vector_init(&_gfx_win32->screens, sizeof(GFX_Win32_Screen));
		gfx_vector_init(&_gfx_win32->windows, sizeof(GFX_Win32_Window));

		/* Enumerate all display adapters */
		DISPLAY_DEVICE adapter;
		ZeroMemory(&adapter, sizeof(DISPLAY_DEVICE));
		adapter.cb = sizeof(DISPLAY_DEVICE);

		DWORD i = 0;
		while(EnumDisplayDevices(NULL, i++, &adapter, 0))
		{
			/* Validate adapter */
			if(!(adapter.StateFlags & DISPLAY_DEVICE_ACTIVE) ||
				adapter.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)
			{
				continue;
			}

			/* Get display device (screen) */
			DISPLAY_DEVICE display;
			ZeroMemory(&display, sizeof(DISPLAY_DEVICE));
			display.cb = sizeof(DISPLAY_DEVICE);

			EnumDisplayDevices(adapter.DeviceName, 0, &display, 0);

			/* Create new screen */
			GFX_Win32_Screen screen;
			memcpy(screen.name, adapter.DeviceName, sizeof(screen.name));

			HDC dc =
				CreateDC("DISPLAY", display.DeviceString, NULL, NULL);
			screen.width =
				GetDeviceCaps(dc, HORZRES);
			screen.height =
				GetDeviceCaps(dc, VERTRES);

			DeleteDC(dc);

			/* Insert at beginning if primary */
			GFXVectorIterator scrPos =
				(adapter.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) ?
				_gfx_win32->screens.begin : _gfx_win32->screens.end;

			gfx_vector_insert(&_gfx_win32->screens, &screen, scrPos);
		}

		/* Need at least one screen */
		if(_gfx_win32->screens.begin == _gfx_win32->screens.end)
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
		unsigned int i = gfx_vector_get_size(&_gfx_win32->windows);
		while(i--)
		{
			GFXVectorIterator it = gfx_vector_previous(
				&_gfx_win32->windows,
				_gfx_win32->windows.end
			);

			_gfx_platform_window_free(((GFX_Win32_Window*)it)->handle);
		}
		gfx_vector_clear(&_gfx_win32->screens);
		gfx_vector_clear(&_gfx_win32->windows);

		/* Unregister window class */
		UnregisterClass(_gfx_win32_window_class, GetModuleHandle(NULL));

		/* Deallocate instance */
		free(_gfx_win32);
		_gfx_win32 = NULL;
	}
}
