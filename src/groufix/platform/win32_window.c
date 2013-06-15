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
#include <windowsx.h>
#include <stdlib.h>
#include <string.h>

/* Windows apparently does not define this everywhere... */
#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020e
#endif

/******************************************************/
static GFXKey _gfx_win32_get_extended_key(GFXKey key, LPARAM lParam)
{
	/* Get extended key (probaby right key) */
	if(lParam & 0x1000000) switch(key)
	{
		case GFX_KEY_RETURN       : return GFX_KEY_KP_RETURN;
		case GFX_KEY_SHIFT_LEFT   : return GFX_KEY_SHIFT_RIGHT;
		case GFX_KEY_CONTROL_LEFT : return GFX_KEY_CONTROL_RIGHT;
		case GFX_KEY_ALT_LEFT     : return GFX_KEY_ALT_RIGHT;
	}
	return key;
}

/******************************************************/
static GFXKeyState _gfx_win32_get_key_state(void)
{
	GFXKeyState state = GFX_KEY_STATE_NONE;

	if(0xff00 & GetKeyState(VK_SHIFT))
		state |= GFX_KEY_STATE_SHIFT;
	if(0xff00 & GetKeyState(VK_CONTROL))
		state |= GFX_KEY_STATE_CONTROL;
	if(0xff00 & GetKeyState(VK_MENU))
		state |= GFX_KEY_STATE_ALT;
	if(0xff00 & (GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)))
		state |= GFX_KEY_STATE_SUPER;
	if(0x00ff & GetKeyState(VK_CAPITAL))
		state |= GFX_KEY_STATE_CAPS_LOCK;
	if(0x00ff & GetKeyState(VK_NUMLOCK))
		state |= GFX_KEY_STATE_NUM_LOCK;
	if(0x00ff & GetKeyState(VK_SCROLL))
		state |= GFX_KEY_STATE_SCROLL_LOCK;

	return state;
}

/******************************************************/
static LRESULT CALLBACK _gfx_win32_window_proc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam)
{
	/* Get window, check if the window is still managed */
	void* window = NULL;

	unsigned int i;
	for(i = 0; i < _gfx_win32->numWindows; ++i)
		if(_gfx_win32->windows[i] == handle) window = handle;

	/* Validate window */
	if(window) switch(msg)
	{
		/* Close button */
		case WM_CLOSE :
		{
			_gfx_event_window_close(window);
			return 0;
		}

		/* Key press */
		case WM_KEYDOWN :
		case WM_SYSKEYDOWN :
		{
			GFXKey key;
			if(wParam > GFX_WIN32_MAX_KEYCODE) key = GFX_KEY_UNKNOWN;
			else key = _gfx_win32_get_extended_key(_gfx_win32->keys[wParam], lParam);

			_gfx_event_key_press(window, key, _gfx_win32_get_key_state());

			return 0;
		}

		/* Key release */
		case WM_KEYUP :
		case WM_SYSKEYUP :
		{
			GFXKey key;
			if(wParam > GFX_WIN32_MAX_KEYCODE) key = GFX_KEY_UNKNOWN;
			else key = _gfx_win32_get_extended_key(_gfx_win32->keys[wParam], lParam);

			_gfx_event_key_release(window, key, _gfx_win32_get_key_state());

			return 0;
		}

		/* Mouse move */
		case WM_MOUSEMOVE :
		{
			_gfx_event_mouse_move(window,
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam),
				_gfx_win32_get_key_state()
			);
			return 0;
		}

		/* Left mouse button */
		case WM_LBUTTONDOWN :
		{
			_gfx_event_mouse_press(window,
				GFX_MOUSE_KEY_LEFT,
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam),
				_gfx_win32_get_key_state()
			);
			return 0;
		}
		case WM_LBUTTONUP :
		{
			_gfx_event_mouse_release(window,
				GFX_MOUSE_KEY_LEFT,
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam),
				_gfx_win32_get_key_state()
			);
			return 0;
		}

		/* Right mouse button */
		case WM_RBUTTONDOWN :
		{
			_gfx_event_mouse_press(window,
				GFX_MOUSE_KEY_RIGHT,
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam),
				_gfx_win32_get_key_state()
			);
			return 0;
		}
		case WM_RBUTTONUP :
		{
			_gfx_event_mouse_release(window,
				GFX_MOUSE_KEY_RIGHT,
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam),
				_gfx_win32_get_key_state()
			);
			return 0;
		}

		/* Left mouse button */
		case WM_MBUTTONDOWN :
		{
			_gfx_event_mouse_press(window,
				GFX_MOUSE_KEY_MIDDLE,
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam),
				_gfx_win32_get_key_state()
			);
			return 0;
		}
		case WM_MBUTTONUP :
		{
			_gfx_event_mouse_release(window,
				GFX_MOUSE_KEY_MIDDLE,
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam),
				_gfx_win32_get_key_state()
			);
			return 0;
		}

		/* Vertical mouse wheel */
		case WM_MOUSEWHEEL :
		{
			_gfx_event_mouse_wheel(window,
				0, GET_WHEEL_DELTA_WPARAM(wParam),
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam),
				_gfx_win32_get_key_state()
			);

			return 0;
		}

		/* Horizontal mouse wheel */
		case WM_MOUSEHWHEEL :
		{
			_gfx_event_mouse_wheel(window,
				GET_WHEEL_DELTA_WPARAM(wParam), 0,
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam),
				_gfx_win32_get_key_state()
			);

			return 0;
		}
	}

	return DefWindowProc(handle, msg, wParam, lParam);
}

/******************************************************/
static int _gfx_win32_register_window_class(void)
{
	/* Check if it is already registered */
	if(_gfx_win32->classRegistered) return 1;

	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = _gfx_win32_window_proc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetModuleHandle(NULL);
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = GFX_WIN32_WND_CLASS;
	wc.hIconSm       = NULL;

	_gfx_win32->classRegistered = RegisterClassEx(&wc);
	return _gfx_win32->classRegistered;
}

/******************************************************/
static void _gfx_win32_add_window(void* handle, void* monitor)
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

/******************************************************/
static void _gfx_win32_remove_window(void* handle)
{
	/* Remove the handle from the array */
	unsigned int i;
	for(i = 0; i < _gfx_win32->numWindows; ++i)
		if(_gfx_win32->windows[i] == handle)
	{
		--_gfx_win32->numWindows;
		if(!_gfx_win32->numWindows)
		{
			/* Free the array */
			free(_gfx_win32->windows);
			free(_gfx_win32->windowMonitors);
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
void _gfx_platform_poll_events(void)
{
	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		/* Dispatch any regular message */
		if(msg.message != WM_QUIT)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		/* Terminate properly on WM_QUIT */
		else _gfx_platform_terminate();
	}
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
void* _gfx_platform_window_get_screen(void* handle)
{
	unsigned int i;
	if(_gfx_win32) for(i = 0; i < _gfx_win32->numWindows; ++i)
		if(_gfx_win32->windows[i] == handle) return _gfx_win32->windowMonitors[i];

	return NULL;
}

/******************************************************/
char* _gfx_platform_window_get_name(void* handle)
{
	/* Check if it has a name */
	int len = GetWindowTextLength(handle);
	if(!len++) return NULL;

	wchar_t* buff = (wchar_t*)malloc(sizeof(wchar_t) * len);
	if(!GetWindowText(handle, buff, len)) return NULL;

	/* Make sure to convert to utf-8 */
	char* utf8 = wchar_to_utf8(buff);
	free(buff);

	return utf8;
}

/******************************************************/
void _gfx_platform_window_get_size(void* handle, unsigned int* width, unsigned int* height)
{
	RECT rect;
	ZeroMemory(&rect, sizeof(RECT));

	GetWindowRect(handle, &rect);
	*width = rect.right - rect.left;
	*height = rect.bottom - rect.top;
}

/******************************************************/
void _gfx_platform_window_get_position(void* handle, int* x, int* y)
{
	RECT rect;
	ZeroMemory(&rect, sizeof(RECT));

	GetWindowRect(handle, &rect);
	*x = rect.left;
	*y = rect.top;

	/* Get window's monitor position */
	HMONITOR monitor = (HMONITOR)_gfx_platform_window_get_screen(handle);
	if(monitor)
	{
		MONITORINFO info;
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(monitor, &info);
		*x -= info.rcMonitor.left;
		*y -= info.rcMonitor.top;
	}
}

/******************************************************/
void _gfx_platform_window_set_name(void* handle, const char* name)
{
	/* Make sure to convert to wide character */
	wchar_t* wchar = utf8_to_wchar(name);
	SetWindowText(handle, wchar);
	free(wchar);
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
	HMONITOR monitor = (HMONITOR)_gfx_platform_window_get_screen(handle);
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
