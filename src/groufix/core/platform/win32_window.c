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

#include "groufix.h"
#include "groufix/core/platform/win32.h"

#include <stdlib.h>

/******************************************************/
static int _gfx_win32_set_fullscreen(GFX_Win32_Screen* screen, unsigned int width, unsigned int height, const GFXColorDepth* depth)
{
	/* Minimum of 32 bits */
	DEVMODE mode;
	ZeroMemory(&mode, sizeof(DEVMODE));

	mode.dmSize       = sizeof(DEVMODE);
	mode.dmBitsPerPel = depth->redBits + depth->greenBits + depth->blueBits;
	mode.dmBitsPerPel = mode.dmBitsPerPel < 32 ? 32 : mode.dmBitsPerPel;
	mode.dmPelsWidth  = width;
	mode.dmPelsHeight = height;
	mode.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	/* Actual call */
	return ChangeDisplaySettingsEx(screen->name, &mode, NULL, CDS_FULLSCREEN, NULL) == DISP_CHANGE_SUCCESSFUL;
}

/******************************************************/
static void _gfx_win32_get_screen_position(GFX_Win32_Screen* screen, int* x, int* y)
{
	DEVMODE mode;
	ZeroMemory(&mode, sizeof(DEVMODE));
	mode.dmSize = sizeof(DEVMODE);

	EnumDisplaySettingsEx(screen->name, ENUM_CURRENT_SETTINGS, &mode, EDS_ROTATEMODE);
	*x = mode.dmPosition.x;
	*y = mode.dmPosition.y;
}

/******************************************************/
void _gfx_win32_set_pixel_format(HWND handle, const GFXColorDepth* depth)
{
	PIXELFORMATDESCRIPTOR format;
	ZeroMemory(&format, sizeof(PIXELFORMATDESCRIPTOR));

	format.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
	format.nVersion   = 1;
	format.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	format.iPixelType = PFD_TYPE_RGBA;
	format.cColorBits = depth->redBits + depth->greenBits + depth->blueBits;
	format.iLayerType = PFD_MAIN_PLANE;

	HDC context = GetDC(handle);

	/* Get format compatible with the window */
	int index = ChoosePixelFormat(context, &format);
	SetPixelFormat(context, index, &format);
}

/******************************************************/
static void _gfx_win32_track_mouse(HWND handle)
{
	TRACKMOUSEEVENT track;
	ZeroMemory(&track, sizeof(TRACKMOUSEEVENT));

	track.cbSize    = sizeof(TRACKMOUSEEVENT);
	track.dwFlags   = TME_LEAVE;
	track.hwndTrack = handle;

	TrackMouseEvent(&track);
}

/******************************************************/
static GFXKey _gfx_win32_get_extended_key(GFXKey key, LPARAM lParam)
{
	/* Get extended key (probably right key) */
	if(lParam & 0x1000000) switch(key)
	{
		case GFX_KEY_RETURN       : return GFX_KEY_KP_RETURN;
		case GFX_KEY_SHIFT_LEFT   : return GFX_KEY_SHIFT_RIGHT;
		case GFX_KEY_CONTROL_LEFT : return GFX_KEY_CONTROL_RIGHT;
		case GFX_KEY_ALT_LEFT     : return GFX_KEY_ALT_RIGHT;
		
		default : return key;
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
	GFX_PlatformWindow window = (GFX_PlatformWindow)handle;

	switch(msg)
	{
		/* Close button */
		case WM_CLOSE :
		{
			_gfx_event_window_close(window);
			return 0;
		}

		/* Move */
		case WM_MOVE :
		{
			int xS = 0;
			int yS = 0;

			GFX_Win32_Screen* screen = _gfx_platform_window_get_screen(window);
			if(screen) _gfx_win32_get_screen_position(screen, &xS, &yS);
			
			int x = (int)(short)LOWORD(lParam);
			int y = (int)(short)HIWORD(lParam);

			_gfx_event_window_move(window, x - xS, y - yS);

			return 0;
		}

		/* Resize */
		case WM_SIZE :
		{
			_gfx_event_window_resize(window, LOWORD(lParam), HIWORD(lParam));
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
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			GFXKeyState state = _gfx_win32_get_key_state();

			_gfx_event_mouse_move(window, x, y, state);

			/* Check mouse enter event */
			GFX_Win32_Window* internal = (GFX_Win32_Window*)_gfx_win32_get_window_from_handle(handle);
			if(!(internal->flags & GFX_WIN32_MOUSEINSIDE))
			{
				internal->flags |= GFX_WIN32_MOUSEINSIDE;
				_gfx_win32_track_mouse(handle);

				_gfx_event_mouse_enter(window, x, y, state);
			}

			return 0;
		}

		/* Mouse leave */
		case WM_MOUSELEAVE :
		{
			GFX_Win32_Window* internal = (GFX_Win32_Window*)_gfx_win32_get_window_from_handle(handle);
			internal->flags &= ~GFX_WIN32_MOUSEINSIDE;

			/* Untrack */
			TRACKMOUSEEVENT track;
			ZeroMemory(&track, sizeof(TRACKMOUSEEVENT));

			track.cbSize    = sizeof(TRACKMOUSEEVENT);
			track.dwFlags   = TME_CANCEL | TME_LEAVE;
			track.hwndTrack = handle;

			TrackMouseEvent(&track);

			/* Get mouse position */
			POINT pnt;
			ZeroMemory(&pnt, sizeof(POINT));
			GetCursorPos(&pnt);

			ScreenToClient(handle, &pnt);

			_gfx_event_mouse_leave(window, pnt.x, pnt.y, _gfx_win32_get_key_state());

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

		/* Middle mouse button */
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
GFX_PlatformWindow _gfx_platform_window_create(const GFX_PlatformAttributes* attributes)
{
	/* Make sure to register the window class */
	if(!_gfx_win32_register_window_class()) return NULL;

	/* Setup the win32 window */
	GFX_Win32_Window window;
	window.screen  = attributes->screen;
	window.context = NULL;
	window.flags   = 0;

	/* Get screen position */
	int xS = 0;
	int yS = 0;
	_gfx_win32_get_screen_position(attributes->screen, &xS, &yS);

	/* Style and window rectangle */
	DWORD styleEx = 0;
	DWORD style = 0;

	RECT rect;
	rect.right = attributes->width;
	rect.bottom = attributes->height;

	if(attributes->flags & GFX_WINDOW_FULLSCREEN)
	{
		/* Change to fullscreen */
		if(!_gfx_win32_set_fullscreen(
			attributes->screen,
			attributes->width,
			attributes->height,
			&attributes->depth)) return NULL;

		window.flags |= GFX_WIN32_FULLSCREEN;

		/* Style and rectangle */
		styleEx = WS_EX_TOPMOST;
		style = WS_POPUP | WS_VISIBLE;

		rect.left = xS;
		rect.top = yS;
	}
	else
	{
		/* Create window style */
		if(!(attributes->flags & GFX_WINDOW_BORDERLESS))
		{
			styleEx =
				WS_EX_WINDOWEDGE;

			style =
				WS_CAPTION |
				WS_MINIMIZEBOX |
				WS_OVERLAPPED |
				WS_SYSMENU;

			if(attributes->flags & GFX_WINDOW_RESIZABLE) style |=
				WS_MAXIMIZEBOX |
				WS_SIZEBOX;
		}
		else
		{
			/* Borderless */
			styleEx = WS_EX_TOPMOST;
			style = WS_POPUP | WS_VISIBLE;
		}

		/* Rectangle */
		rect.left = xS + attributes->x;
		rect.top = yS + attributes->y;
	}

	rect.right += rect.left;
	rect.bottom += rect.top;
	AdjustWindowRectEx(&rect, style, FALSE, styleEx);

	/* Make sure to convert to wide character */
	wchar_t* name = _gfx_win32_utf8_to_wchar(attributes->name);

	/* Create the actual window */
	window.handle = CreateWindowEx(
		styleEx | WS_EX_APPWINDOW,
		GFX_WIN32_WND_CLASS,
		name,
		style | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL
	);
	free(name);

	if(window.handle)
	{
		/* Some fullscreen options */
		if(attributes->flags & GFX_WINDOW_FULLSCREEN)
		{
			SetWindowPos(window.handle, HWND_TOPMOST, 0,0,0,0, SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOSIZE);
			ShowWindow(window.handle, SW_MAXIMIZE);
		}

		/* Add window to vector */
		if(gfx_vector_insert(&_gfx_win32->windows, &window, _gfx_win32->windows.end) != _gfx_win32->windows.end)
		{
			/* Set pixel format */
			_gfx_win32_set_pixel_format(window.handle, &attributes->depth);

			/* Start tracking the mouse */
			_gfx_win32_track_mouse(window.handle);

			return window.handle;
		}
		DestroyWindow(window.handle);
	}

	/* Undo fullscreen */
	if(window.flags & GFX_WIN32_FULLSCREEN)
		ChangeDisplaySettingsEx(window.screen->name, NULL, NULL, CDS_FULLSCREEN, NULL);

	return NULL;
}

/******************************************************/
void _gfx_platform_window_free(GFX_PlatformWindow handle)
{
	if(_gfx_win32)
	{
		/* Make sure to undo fullscreen */
		GFX_Win32_Window* it = _gfx_win32_get_window_from_handle(handle);
		if(it->flags & GFX_WIN32_FULLSCREEN)
			ChangeDisplaySettingsEx(it->screen->name, NULL, NULL, CDS_FULLSCREEN, NULL);

		/* Destroy the context and window */
		_gfx_platform_context_free(handle);
		DestroyWindow(handle);

		/* Remove from vector */
		gfx_vector_erase(&_gfx_win32->windows, it);
	}
}

/******************************************************/
GFX_PlatformScreen _gfx_platform_window_get_screen(GFX_PlatformWindow handle)
{
	GFX_Win32_Window* it = _gfx_win32_get_window_from_handle(handle);
	if(it) return it->screen;

	return NULL;
}

/******************************************************/
char* _gfx_platform_window_get_name(GFX_PlatformWindow handle)
{
	/* Check if it has a name */
	int len = GetWindowTextLength(handle);
	if(!len++) return NULL;

	wchar_t* buff = malloc(sizeof(wchar_t) * len);
	if(!GetWindowText(handle, buff, len)) return NULL;

	/* Make sure to convert to utf-8 */
	char* utf8 = _gfx_win32_wchar_to_utf8(buff);
	free(buff);

	return utf8;
}

/******************************************************/
void _gfx_platform_window_get_size(GFX_PlatformWindow handle, unsigned int* width, unsigned int* height)
{
	RECT rect;
	ZeroMemory(&rect, sizeof(RECT));

	GetWindowRect(handle, &rect);
	*width = rect.right - rect.left;
	*height = rect.bottom - rect.top;
}

/******************************************************/
void _gfx_platform_window_get_position(GFX_PlatformWindow handle, int* x, int* y)
{
	/* Get window's screen position */
	GFX_Win32_Screen* screen = _gfx_platform_window_get_screen(handle);
	if(screen) _gfx_win32_get_screen_position(screen, x, y);

	RECT rect;
	ZeroMemory(&rect, sizeof(RECT));

	GetWindowRect(handle, &rect);
	*x = rect.left - *x;
	*y = rect.top - *y;
}

/******************************************************/
void _gfx_platform_window_set_name(GFX_PlatformWindow handle, const char* name)
{
	/* Make sure to convert to wide character */
	wchar_t* wchar = _gfx_win32_utf8_to_wchar(name);
	SetWindowText(handle, wchar);
	free(wchar);
}

/******************************************************/
void _gfx_platform_window_set_size(GFX_PlatformWindow handle, unsigned int width, unsigned int height)
{
	SetWindowPos(handle, NULL, 0, 0, width, height, SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOZORDER);
}

/******************************************************/
void _gfx_platform_window_set_position(GFX_PlatformWindow handle, int x, int y)
{
	/* Get window's monitor position */
	int xM = 0;
	int yM = 0;

	GFX_Win32_Screen* screen = _gfx_platform_window_get_screen(handle);
	if(screen) _gfx_win32_get_screen_position(screen, &xM, &yM);

	SetWindowPos(handle, NULL, x + xM, y + yM, 0, 0, SWP_NOCOPYBITS | SWP_NOSIZE | SWP_NOZORDER);
}

/******************************************************/
void _gfx_platform_window_show(GFX_PlatformWindow handle)
{
	ShowWindow(handle, SW_SHOW);
}

/******************************************************/
void _gfx_platform_window_hide(GFX_PlatformWindow handle)
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
		else gfx_terminate();
	}
}
