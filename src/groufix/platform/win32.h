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

/* Validate platform */
#define GFX_INTERNAL_GL_LEGACY
#include "groufix/platform.h"

#ifdef GFX_GLES
	#error "Cannot compile Win32 platform using GL ES"
#endif

/* Includes */
#include "groufix/containers/vector.h"
#include "groufix/utils.h"

#include <wchar.h>
#include <windowsx.h>
#include <GL/wglext.h>

/* Windows apparently does not define this everywhere... */
#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020e
#endif

/* Maximum key code lookup */
#define GFX_WIN32_MAX_KEYCODE   0x0ff
#define GFX_WIN32_NUM_KEYCODES  0x100

/* Groufix window class */
#define GFX_WIN32_WND_CLASS L"GROUFIX"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * \brief Vital Win32 Extensions
 *******************************************************/
typedef struct GFX_Win32_Extensions
{
	PFNWGLCREATECONTEXTATTRIBSARBPROC  CreateContextAttribsARB;
	PFNWGLGETEXTENSIONSSTRINGARBPROC   GetExtensionsStringARB;

} GFX_Win32_Extensions;


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
	/* Monitors and Windows */
	Vector*   monitors;        /* Stores HMONITOR */
	char      classRegistered; /* Whether or not the window class is registered */
	Vector*   windows;         /* Stores GFX_Win32_Window */

	/* Key table */
	GFXKey    keys[GFX_WIN32_NUM_KEYCODES];

	/* Extensions */
	GFX_Win32_Extensions extensions;

} GFX_Win32_Instance;


/**
 * \brief Instance pointer
 */
extern GFX_Win32_Instance* _gfx_win32;


/**
 * \brief Sets the pixel format for a window.
 *
 */
void _gfx_win32_set_pixel_format(HWND handle, const GFXColorDepth* depth);

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


#ifdef __cplusplus
}
#endif

#endif // GFX_PLATFORM_WIN32_H
