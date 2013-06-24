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
static void _gfx_win32_set_pixel_format(HWND handle, unsigned short red, unsigned short green, unsigned short blue)
{
	PIXELFORMATDESCRIPTOR format;
	format.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
	format.nVersion     = 1;
	format.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	format.iPixelType   = PFD_TYPE_RGBA;
	format.cColorBits   = red + green + blue;
	format.cRedBits     = red;
	format.cGreenBits   = green;
	format.cBlueBits    = blue;
	format.cAlphaBits   = 0;
	format.cAccumBits   = 0;
	format.cDepthBits   = 0;
	format.cStencilBits = 0;
	format.cAuxBuffers  = 0;
	format.iLayerType   = PFD_MAIN_PLANE;

	HDC context = GetDC(handle);

	/* Get format compatible with the window */
	int index = ChoosePixelFormat(context, &format);
	SetPixelFormat(context, index, &format);
}

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
	/* Get window */
	GFX_Platform_Window window = (GFX_Platform_Window)handle;

	switch(msg)
	{
		/* Close button */
		case WM_CLOSE :
		{
			gfx_event_window_close(window);
			return 0;
		}

		/* Key press */
		case WM_KEYDOWN :
		case WM_SYSKEYDOWN :
		{
			GFXKey key;
			if(wParam > GFX_WIN32_MAX_KEYCODE) key = GFX_KEY_UNKNOWN;
			else key = _gfx_win32_get_extended_key(_gfx_win32->keys[wParam], lParam);

			gfx_event_key_press(window, key, _gfx_win32_get_key_state());

			return 0;
		}

		/* Key release */
		case WM_KEYUP :
		case WM_SYSKEYUP :
		{
			GFXKey key;
			if(wParam > GFX_WIN32_MAX_KEYCODE) key = GFX_KEY_UNKNOWN;
			else key = _gfx_win32_get_extended_key(_gfx_win32->keys[wParam], lParam);

			gfx_event_key_release(window, key, _gfx_win32_get_key_state());

			return 0;
		}

		/* Mouse move */
		case WM_MOUSEMOVE :
		{
			gfx_event_mouse_move(window,
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam),
				_gfx_win32_get_key_state()
			);
			return 0;
		}

		/* Left mouse button */
		case WM_LBUTTONDOWN :
		{
			gfx_event_mouse_press(window,
				GFX_MOUSE_KEY_LEFT,
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam),
				_gfx_win32_get_key_state()
			);
			return 0;
		}
		case WM_LBUTTONUP :
		{
			gfx_event_mouse_release(window,
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
			gfx_event_mouse_press(window,
				GFX_MOUSE_KEY_RIGHT,
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam),
				_gfx_win32_get_key_state()
			);
			return 0;
		}
		case WM_RBUTTONUP :
		{
			gfx_event_mouse_release(window,
				GFX_MOUSE_KEY_RIGHT,
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam),
				_gfx_win32_get_key_state()
			);
			return 0;
		}

		/* Middle mouse button */
		case WM_MBUTTONDOWN :
		{
			gfx_event_mouse_press(window,
				GFX_MOUSE_KEY_MIDDLE,
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam),
				_gfx_win32_get_key_state()
			);
			return 0;
		}
		case WM_MBUTTONUP :
		{
			gfx_event_mouse_release(window,
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
			gfx_event_mouse_wheel(window,
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
			gfx_event_mouse_wheel(window,
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
	if(!_gfx_win32) return 0;
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
GFX_Platform_Window _gfx_platform_create_window(const GFX_Platform_Attributes* attributes)
{
	/* Make sure to register the window class */
	if(!_gfx_win32_register_window_class()) return NULL;

	/* Setup the win32 window */
	GFX_Win32_Window window;
	window.monitor = attributes->screen;
	window.context = NULL;

	/* Get monitor information */
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(attributes->screen, &info);

	/* Make sure to convert to wide character */
	wchar_t* name = utf8_to_wchar(attributes->name);

	window.handle = CreateWindowEx(
		0,
		GFX_WIN32_WND_CLASS,
		name,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
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
	if(!window.handle) return NULL;

	/* Add window to vector */
	if(!vector_insert(_gfx_win32->windows, &window, _gfx_win32->windows->end))
	{
		DestroyWindow(window.handle);
		return NULL;
	}

	/* Set pixel format */
	_gfx_win32_set_pixel_format(
		window.handle,
		attributes->redBits,
		attributes->greenBits,
		attributes->blueBits
	);

	return window.handle;
}

/******************************************************/
void _gfx_platform_destroy_window(GFX_Platform_Window handle)
{
	if(_gfx_win32)
	{
		/* Destroy the context and window */
		_gfx_platform_destroy_context(handle);
		DestroyWindow(handle);

		/* Remove from vector */
		VectorIterator it = _gfx_win32_get_window_from_handle(handle);
		vector_erase(_gfx_win32->windows, it);
	}
}

/******************************************************/
GFX_Platform_Screen _gfx_platform_window_get_screen(GFX_Platform_Window handle)
{
	GFX_Win32_Window* it = _gfx_win32_get_window_from_handle(handle);
	if(it) return it->monitor;

	return NULL;
}

/******************************************************/
char* _gfx_platform_window_get_name(GFX_Platform_Window handle)
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
void _gfx_platform_window_get_size(GFX_Platform_Window handle, unsigned int* width, unsigned int* height)
{
	RECT rect;
	ZeroMemory(&rect, sizeof(RECT));

	GetWindowRect(handle, &rect);
	*width = rect.right - rect.left;
	*height = rect.bottom - rect.top;
}

/******************************************************/
void _gfx_platform_window_get_position(GFX_Platform_Window handle, int* x, int* y)
{
	RECT rect;
	ZeroMemory(&rect, sizeof(RECT));

	GetWindowRect(handle, &rect);
	*x = rect.left;
	*y = rect.top;

	/* Get window's monitor position */
	HMONITOR monitor = _gfx_platform_window_get_screen(handle);
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
void _gfx_platform_window_set_name(GFX_Platform_Window handle, const char* name)
{
	/* Make sure to convert to wide character */
	wchar_t* wchar = utf8_to_wchar(name);
	SetWindowText(handle, wchar);
	free(wchar);
}

/******************************************************/
void _gfx_platform_window_set_size(GFX_Platform_Window handle, unsigned int width, unsigned int height)
{
	SetWindowPos(handle, NULL, 0, 0, width, height, SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOZORDER);
}

/******************************************************/
void _gfx_platform_window_set_position(GFX_Platform_Window handle, int x, int y)
{
	/* Get window's monitor position */
	HMONITOR monitor = _gfx_platform_window_get_screen(handle);
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
void _gfx_platform_window_show(GFX_Platform_Window handle)
{
	ShowWindow(handle, SW_SHOW);
}

/******************************************************/
void _gfx_platform_window_hide(GFX_Platform_Window handle)
{
	ShowWindow(handle, SW_HIDE);
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
