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

#ifndef GFX_PLATFORM_WIN32_H
#define GFX_PLATFORM_WIN32_H

/* Windows XP */
#define WINVER 0x0501

/* Unicode */
#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

/* Includes */
#include <windows.h>
#include <windowsx.h>
#include <wchar.h>

#include "groufix/containers/vector.h"
#include "groufix/events.h"


/* Windows apparently does not define this everywhere... */
#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020e
#endif

/* Maximum key code lookup */
#define GFX_WIN32_MAX_KEYCODE 0xff
#define GFX_WIN32_NUM_KEYCODES 0x100

/* Groufix window class */
#define GFX_WIN32_WND_CLASS L"GROUFIX"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * \brief Win32 Window
 *******************************************************/
typedef struct GFX_Win32_Window
{
	HWND      handle;  /* Given to the outside world */
	HMONITOR  monitor;
	HGLRC     context;

} GFX_Win32_Window;


/********************************************************
 * \brief Win32 Instance
 *******************************************************/
typedef struct GFX_Win32_Instance
{
	/* Monitors */
	Vector*   monitors;        /* Stores HMONITOR */

	/* Windows */
	char      classRegistered; /* Whether or not the window class is registered */
	Vector*   windows;         /* Stores GFX_Win32_Window */

	/* Key table */
	GFXKey    keys[GFX_WIN32_NUM_KEYCODES];

} GFX_Win32_Instance;


/**
 * \brief Instance pointer
 */
extern GFX_Win32_Instance* _gfx_win32;


/**
 * \brief Returns an Win32 window from its handle.
 *
 */
VectorIterator _gfx_win32_get_window_from_handle(HWND handle);

/**
 * \brief Converts a UTF-8 string to a wide string.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
wchar_t* utf8_to_wchar(const char* str);

/**
 * \brief Converts a wide string to a UTF-8 string.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
char* wchar_to_utf8(const wchar_t* str);


/********************************************************
 * \brief Win32 Native Access
 *******************************************************/

/**
 * \brief Returns a HMONITOR.
 *
 */
HMONITOR gfx_win32_get_screen(GFX_Platform_Screen screen);

/**
 * \brief Returns a HWND.
 *
 */
HWND gfx_win32_get_window(GFX_Platform_Window window);

/**
 * \brief Returns a HGLRC.
 *
 */
HGLRC gfx_win32_get_context(GFX_Platform_Window window);


#ifdef __cplusplus
}
#endif

#endif // GFX_PLATFORM_WIN32_H
