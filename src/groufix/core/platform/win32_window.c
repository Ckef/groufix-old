/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/platform/win32.h"
#include "groufix/core/internal.h"
#include "groufix.h"

#include <stdlib.h>

/******************************************************/
static inline void _gfx_win32_enter_fullscreen(

		GFX_Win32_Monitor*  monitor,
		DEVMODE*            mode)
{
	ChangeDisplaySettingsEx(monitor->name, mode, NULL, CDS_FULLSCREEN, NULL);
}

/******************************************************/
static inline void _gfx_win32_leave_fullscreen(

		GFX_Win32_Monitor* monitor)
{
	ChangeDisplaySettingsEx(monitor->name, NULL, NULL, CDS_FULLSCREEN, NULL);
}

/******************************************************/
static void _gfx_win32_track_mouse(

		HWND handle)
{
	TRACKMOUSEEVENT track;
	ZeroMemory(&track, sizeof(TRACKMOUSEEVENT));

	track.cbSize    = sizeof(TRACKMOUSEEVENT);
	track.dwFlags   = TME_LEAVE;
	track.hwndTrack = handle;

	TrackMouseEvent(&track);
}

/******************************************************/
static GFXKey _gfx_win32_get_extended_key(

		GFXKey  key,
		LPARAM  lParam)
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
static LRESULT CALLBACK _gfx_win32_window_proc(

		HWND    handle,
		UINT    msg,
		WPARAM  wParam,
		LPARAM  lParam)
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

			GFX_Win32_Monitor* monitor =
				_gfx_platform_window_get_monitor(window);

			if(monitor)
			{
				xS = monitor->x;
				yS = monitor->y;
			}

			int x = (int)(short)LOWORD(lParam);
			int y = (int)(short)HIWORD(lParam);

			_gfx_event_window_move(window, x - xS, y - yS);

			return 0;
		}

		/* Resize */
		case WM_SIZE :
		{
			_gfx_event_window_resize(
				window,
				LOWORD(lParam),
				HIWORD(lParam)
			);
			return 0;
		}

		/* Focus */
		case WM_SETFOCUS :
		{
			/* Enter fullscreen */
			GFX_Win32_Window* internal =
				_gfx_win32_get_window_from_handle(handle);

			if(!internal) return 0;

			if(internal->flags & GFX_WIN32_FULLSCREEN)
				_gfx_win32_enter_fullscreen(
					internal->monitor,
					internal->mode
				);

			_gfx_event_window_focus(window);

			return 0;
		}

		/* Blur */
		case WM_KILLFOCUS :
		{
			/* Leave fullscreen */
			GFX_Win32_Window* internal =
				_gfx_win32_get_window_from_handle(handle);

			if(!internal) return 0;

			if(internal->flags & GFX_WIN32_FULLSCREEN)
			{
				_gfx_win32_leave_fullscreen(internal->monitor);

				if(!(internal->flags & GFX_WIN32_HIDDEN))
					ShowWindow(handle, SW_MINIMIZE);
			}

			_gfx_event_window_blur(window);

			return 0;
		}

		/* Key press */
		case WM_KEYDOWN :
		case WM_SYSKEYDOWN :
		{
			GFXKey key;
			if(wParam > GFX_WIN32_MAX_KEYCODE)
				key = GFX_KEY_UNKNOWN;
			else key = _gfx_win32_get_extended_key(
				_gfx_win32->keys[wParam],
				lParam);

			_gfx_event_key_press(window, key, _gfx_win32_get_key_state());

			return 0;
		}

		/* Key release */
		case WM_KEYUP :
		case WM_SYSKEYUP :
		{
			GFXKey key;
			if(wParam > GFX_WIN32_MAX_KEYCODE)
				key = GFX_KEY_UNKNOWN;
			else key = _gfx_win32_get_extended_key(
				_gfx_win32->keys[wParam],
				lParam);

			_gfx_event_key_release(window, key, _gfx_win32_get_key_state());

			return 0;
		}

		/* Mouse move */
		case WM_MOUSEMOVE :
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			GFXKeyState state = _gfx_win32_get_key_state();

			/* Check mouse enter event */
			GFX_Win32_Window* internal =
				_gfx_win32_get_window_from_handle(handle);

			if(!internal) return 0;

			if(!(internal->flags & GFX_WIN32_MOUSEINSIDE))
			{
				internal->flags |= GFX_WIN32_MOUSEINSIDE;
				_gfx_win32_track_mouse(handle);

				_gfx_event_mouse_enter(window, x, y, state);
			}

			else _gfx_event_mouse_move(window, x, y, state);

			return 0;
		}

		/* Mouse leave */
		case WM_MOUSELEAVE :
		{
			GFX_Win32_Window* internal =
				_gfx_win32_get_window_from_handle(handle);

			if(!internal) return 0;
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

			_gfx_event_mouse_leave(
				window,
				pnt.x,
				pnt.y,
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
int _gfx_win32_register_classes(void)
{
	HMODULE handle = GetModuleHandle(NULL);

	/* Register dummy class */
	WNDCLASS dc;
	ZeroMemory(&dc, sizeof(WNDCLASS));

	dc.lpfnWndProc   = DefWindowProc;
	dc.hInstance     = handle;
	dc.lpszClassName = GFX_WIN32_WINDOW_CLASS_DUMMY;

	if(!RegisterClass(&dc))
		return 0;

	/* Register regular class */
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.lpfnWndProc   = _gfx_win32_window_proc;
	wc.hInstance     = handle;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = GFX_WIN32_WINDOW_CLASS;

	if(!RegisterClassEx(&wc))
	{
		UnregisterClass(GFX_WIN32_WINDOW_CLASS_DUMMY, handle);
		return 0;
	}

	return 1;
}

/******************************************************/
void _gfx_win32_set_pixel_format(

		HWND                  handle,
		const GFXColorDepth*  depth,
		int                   backBuffer)
{
	PIXELFORMATDESCRIPTOR format;
	ZeroMemory(&format, sizeof(PIXELFORMATDESCRIPTOR));

	format.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
	format.nVersion   = 1;
	format.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	format.iPixelType = PFD_TYPE_RGBA;
	format.cColorBits = depth->redBits + depth->greenBits + depth->blueBits;
	format.iLayerType = PFD_MAIN_PLANE;

	format.dwFlags |= backBuffer ? PFD_DOUBLEBUFFER : 0;
	HDC context = GetDC(handle);

	/* Get format compatible with the window */
	int index = ChoosePixelFormat(context, &format);
	SetPixelFormat(context, index, &format);
}

/******************************************************/
GFX_Win32_Window* _gfx_win32_window_dummy_create(void)
{
	if(!_gfx_win32) return NULL;

	/* Create a dummy window */
	GFX_Win32_Window window;
	window.monitor = NULL;
	window.context = NULL;
	window.flags   = GFX_WIN32_HIDDEN;

	window.handle = CreateWindow(
		GFX_WIN32_WINDOW_CLASS_DUMMY,
		L"",
		0,
		0, 0,
		0, 0,
		NULL, NULL,
		GetModuleHandle(NULL),
		NULL
	);

	if(window.handle)
	{
		/* Add window to vector */
		GFXVectorIterator it = gfx_vector_insert(
			&_gfx_win32->windows,
			&window,
			_gfx_win32->windows.end
		);

		if(it != _gfx_win32->windows.end)
		{
			/* Set pixel format */
			GFXColorDepth depth;
			depth.redBits   = 0;
			depth.greenBits = 0;
			depth.blueBits  = 0;

			_gfx_win32_set_pixel_format(window.handle, &depth, 0);

			return it;
		}

		/* Nevermind */
		DestroyWindow(window.handle);
	}

	return NULL;
}

/******************************************************/
GFX_PlatformWindow _gfx_platform_window_create(

		const GFX_PlatformAttributes* attributes)
{
	if(!_gfx_win32) return NULL;

	/* Setup the win32 window */
	GFX_Win32_Window window;
	window.monitor = attributes->monitor;
	window.context = NULL;
	window.flags   = 0;

	window.flags |=
		attributes->flags & GFX_WINDOW_RESIZABLE ?
		GFX_WIN32_RESIZABLE : 0;
	window.flags |=
		attributes->flags & GFX_WINDOW_HIDDEN ?
		GFX_WIN32_HIDDEN : 0;

	/* Display mode, style and window rectangle */
	DWORD styleEx =
		WS_EX_APPWINDOW;
	DWORD style =
		(!(attributes->flags & GFX_WINDOW_HIDDEN) ?
		WS_VISIBLE : 0);

	GFXColorDepth depth;
	RECT rect;

	rect.left = window.monitor->x;
	rect.top = window.monitor->y;

	if(attributes->flags & GFX_WINDOW_FULLSCREEN)
	{
		/* Display mode */
		window.flags |= GFX_WIN32_FULLSCREEN;

		window.mode = gfx_vector_at(
			&_gfx_win32->modes,
			window.monitor->modes + attributes->mode);

		_gfx_split_depth(
			window.mode->dmBitsPerPel,
			&depth.redBits,
			&depth.greenBits,
			&depth.blueBits);

		/* Style and rectangle */
		rect.right = window.mode->dmPelsWidth;
		rect.bottom = window.mode->dmPelsHeight;

		styleEx |= WS_EX_TOPMOST;
		style |= WS_POPUP;
	}
	else
	{
		/* Color depth and rectangle */
		depth       = *attributes->depth;
		rect.right  = attributes->w;
		rect.bottom = attributes->h;

		rect.left += attributes->x;
		rect.top += attributes->y;

		/* Style */
		style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

		if(!(attributes->flags & GFX_WINDOW_BORDERLESS))
		{
			/* With a border */
			styleEx |=
				WS_EX_WINDOWEDGE;

			style |=
				WS_CAPTION |
				WS_MINIMIZEBOX |
				WS_OVERLAPPED |
				WS_SYSMENU;

			/* With size options */
			if(attributes->flags & GFX_WINDOW_RESIZABLE) style |=
				WS_MAXIMIZEBOX |
				WS_SIZEBOX;
		}
		else
		{
			/* Borderless */
			styleEx |= WS_EX_TOPMOST;
			style |= WS_POPUP;
		}
	}

	rect.right += rect.left;
	rect.bottom += rect.top;

	/* Make sure the client area is the specified size */
	AdjustWindowRectEx(&rect, style, FALSE, styleEx);

	/* Convert name to UTF-16 */
	WCHAR* name = _gfx_win32_utf8_to_utf16(attributes->name);

	/* Create the actual window */
	window.handle = CreateWindowEx(
		styleEx,
		GFX_WIN32_WINDOW_CLASS,
		name,
		style,
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
		/* Add window to vector */
		GFXVectorIterator it = gfx_vector_insert(
			&_gfx_win32->windows,
			&window,
			_gfx_win32->windows.end
		);

		if(it != _gfx_win32->windows.end)
		{
			/* Set pixel format */
			_gfx_win32_set_pixel_format(
				window.handle,
				&depth,
				attributes->flags & GFX_WINDOW_DOUBLE_BUFFER
			);

			/* Start tracking the mouse */
			_gfx_win32_track_mouse(window.handle);

			/* Enter fullscreen */
			if(
				(attributes->flags & GFX_WINDOW_FULLSCREEN) &&
				!(attributes->flags & GFX_WINDOW_HIDDEN))
			{
				_gfx_win32_enter_fullscreen(window.monitor, window.mode);
			}

			return window.handle;
		}

		DestroyWindow(window.handle);
	}

	return NULL;
}

/******************************************************/
void _gfx_platform_window_free(

		GFX_PlatformWindow handle)
{
	if(_gfx_win32)
	{
		/* Make sure to undo fullscreen */
		GFX_Win32_Window* it = _gfx_win32_get_window_from_handle(handle);
		if(it->flags & GFX_WIN32_FULLSCREEN)
			_gfx_win32_leave_fullscreen(it->monitor);

		/* Destroy the context and window */
		_gfx_platform_context_clear(handle);
		DestroyWindow(handle);

		/* Remove from vector */
		gfx_vector_erase(&_gfx_win32->windows, it);
	}
}

/******************************************************/
GFX_PlatformMonitor _gfx_platform_window_get_monitor(

		GFX_PlatformWindow handle)
{
	GFX_Win32_Window* it = _gfx_win32_get_window_from_handle(handle);
	if(it) return it->monitor;

	return NULL;
}

/******************************************************/
char* _gfx_platform_window_get_name(

		GFX_PlatformWindow handle)
{
	/* Check if it has a name */
	int len = GetWindowTextLength(handle);
	if(!(len++)) return NULL;

	WCHAR* buff = malloc(sizeof(WCHAR) * len);

	if(!GetWindowText(handle, buff, len))
	{
		free(buff);
		return NULL;
	}

	/* Convert to UTF-8 */
	char* str = _gfx_win32_utf16_to_utf8(buff);
	free(buff);

	return str;
}

/******************************************************/
void _gfx_platform_window_get_size(

		GFX_PlatformWindow  handle,
		unsigned int*       width,
		unsigned int*       height)
{
	RECT rect;
	ZeroMemory(&rect, sizeof(RECT));

	GetClientRect(handle, &rect);
	*width = rect.right;
	*height = rect.bottom;
}

/******************************************************/
void _gfx_platform_window_get_position(

		GFX_PlatformWindow  handle,
		int*                x,
		int*                y)
{
	/* Get window's monitor position */
	GFX_Win32_Monitor* monitor =
		_gfx_platform_window_get_monitor(handle);

	if(monitor)
	{
		*x = monitor->x;
		*y = monitor->y;
	}

	POINT point;
	point.x = 0;
	point.y = 0;
	MapWindowPoints(handle, GetParent(handle), &point, 1);

	*x = point.x - *x;
	*y = point.y - *y;
}

/******************************************************/
void _gfx_platform_window_set_name(

		GFX_PlatformWindow  handle,
		const char*         name)
{
	/* Convert to UTF-16 */
	WCHAR* str = _gfx_win32_utf8_to_utf16(name);
	if(str)
	{
		SetWindowText(handle, str);
		free(str);
	}
}

/******************************************************/
void _gfx_platform_window_set_size(

		GFX_PlatformWindow  handle,
		unsigned int        width,
		unsigned int        height)
{
	GFX_Win32_Window* it = _gfx_win32_get_window_from_handle(handle);
	if(it && (it->flags & GFX_WIN32_RESIZABLE))
	{
		RECT rect;
		rect.left   = 0;
		rect.right  = width;
		rect.top    = 0;
		rect.bottom = height;

		/* Make sure the client area is the specified size */
		AdjustWindowRectEx(
			&rect,
			GetWindowLong(handle, GWL_STYLE),
			FALSE,
			GetWindowLong(handle, GWL_EXSTYLE));

		SetWindowPos(
			handle,
			NULL,
			0, 0,
			rect.right - rect.left,
			rect.bottom - rect.top,
			SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOZORDER);
	}
}

/******************************************************/
void _gfx_platform_window_set_position(

		GFX_PlatformWindow  handle,
		int                 x,
		int                 y)
{
	/* Check if fullscreen */
	GFX_Win32_Window* it = _gfx_win32_get_window_from_handle(handle);
	if(it && !(it->flags & GFX_WIN32_FULLSCREEN))
	{
		/* Get window's monitor position */
		int xM = 0;
		int yM = 0;

		if(it->monitor)
		{
			xM = it->monitor->x;
			yM = it->monitor->y;
		}

		RECT rect;
		rect.left   = x + xM;
		rect.right  = rect.left;
		rect.top    = y + yM;
		rect.bottom = rect.top;

		/* Make sure the client area is the specified size */
		AdjustWindowRectEx(
			&rect,
			GetWindowLong(handle, GWL_STYLE),
			FALSE,
			GetWindowLong(handle, GWL_EXSTYLE));

		SetWindowPos(
			handle,
			NULL,
			rect.left,
			rect.top,
			0, 0,
			SWP_NOCOPYBITS | SWP_NOSIZE | SWP_NOZORDER);
	}
}

/******************************************************/
void _gfx_platform_window_show(

		GFX_PlatformWindow handle)
{
	GFX_Win32_Window* it =
		_gfx_win32_get_window_from_handle(handle);
	if(it) it->flags &=
		~GFX_WIN32_HIDDEN;

	ShowWindow(handle, SW_SHOW);
}

/******************************************************/
void _gfx_platform_window_hide(

		GFX_PlatformWindow handle)
{
	GFX_Win32_Window* it =
		_gfx_win32_get_window_from_handle(handle);
	if(it) it->flags |=
		GFX_WIN32_HIDDEN;

	ShowWindow(handle, SW_HIDE);
}

/******************************************************/
int _gfx_platform_poll_events(void)
{
	if(!_gfx_win32) return 0;

	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if(msg.message != WM_QUIT)
		{
			/* Dispatch any regular message */
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			/* Terminate properly on WM_QUIT */
			gfx_terminate();
			return 0;
		}
	}

	return 1;
}
