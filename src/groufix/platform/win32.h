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

#ifndef GFX_PLATFORM_WIN32_H
#define GFX_PLATFORM_WIN32_H

/* Validate platform */
#include "groufix/platform.h"

#if defined(GFX_GLES)
	#error "Cannot compile Win32 target using GL ES"
#elif !defined(GFX_WIN32)
	#error "Cannot compile Win32 target on this platform"
#endif

/* Includes */
#include "groufix/containers/vector.h"

#include <wchar.h>
#include <windowsx.h>
#include <GL/gl.h>
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
 * Vital Win32 Extensions
 *******************************************************/

/** Win32 Extensions */
typedef struct GFX_Win32_Extensions
{
	PFNWGLCREATECONTEXTATTRIBSARBPROC  CreateContextAttribsARB;
	PFNWGLGETEXTENSIONSSTRINGARBPROC   GetExtensionsStringARB;
	PFNWGLSWAPINTERVALEXTPROC          SwapIntervalEXT;
	unsigned char                      EXT_swap_control_tear;

} GFX_Win32_Extensions;


/********************************************************
 * Win32 Window
 *******************************************************/

/** Win32 Window Flags */
typedef enum GFX_Win32_Flags
{
	GFX_WIN32_MOUSEINSIDE  = 0x01,
	GFX_WIN32_FULLSCREEN   = 0x02

} GFX_Win32_Flags;


/** Win32 Window */
typedef struct GFX_Win32_Window
{
	HWND             handle; /* Given to the outside world */
	HMONITOR         monitor;
	HGLRC            context;
	GFX_Win32_Flags  flags;

} GFX_Win32_Window;


/********************************************************
 * Win32 Instance
 *******************************************************/

/** Win32 Instance & data */
typedef struct GFX_Win32_Instance
{
	/* Monitors and Windows */
	GFXVector  monitors;        /* Stores HMONITOR */
	GFXVector  windows;         /* Stores GFX_Win32_Window */
	char       classRegistered; /* Whether or not the window class is registered */

	/* Key table */
	GFXKey     keys[GFX_WIN32_NUM_KEYCODES];

	/* Extensions */
	GFX_Win32_Extensions extensions;

} GFX_Win32_Instance;


/**
 * Instance pointer
 */
extern GFX_Win32_Instance* _gfx_win32;


/**
 * Sets the pixel format for a window.
 *
 */
void _gfx_win32_set_pixel_format(HWND handle, const GFXColorDepth* depth);

/**
 * Returns an Win32 window from its handle.
 *
 */
GFXVectorIterator _gfx_win32_get_window_from_handle(HWND handle);

/**
 * Converts a UTF-8 string to a wide string.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
wchar_t* utf8_to_wchar(const char* str);

/**
 * Converts a wide string to a UTF-8 string.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
char* wchar_to_utf8(const wchar_t* str);

/**
 * Returns whether the OpenGL extension can be found in the space seperated string.
 *
 * This method is primarily used in the platform implementations.
 *
 */
int _gfx_extensions_is_in_string(const char* str, const char* ext);


#ifdef __cplusplus
}
#endif

#endif // GFX_PLATFORM_WIN32_H
