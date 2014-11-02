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

#ifndef GFX_CORE_PLATFORM_WIN32_H
#define GFX_CORE_PLATFORM_WIN32_H


/* Validate platform */
#include "groufix/core/platform.h"

#if !defined(GFX_GL)
	#error "Must compile Win32 target using desktop GL"
#elif !defined(GFX_WIN32)
	#error "Cannot compile Win32 target on this platform"
#endif

/* Includes */
#include "groufix/containers/vector.h"

#include <wchar.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <GL/wglext.h>


/* Window classes */
#define GFX_WIN32_WINDOW_CLASS        L"GROUFIX"
#define GFX_WIN32_WINDOW_CLASS_DUMMY  L"GROUFIXDUMMY"


/* Maximum key code lookup */
#define GFX_WIN32_MAX_KEYCODE   0x0ff
#define GFX_WIN32_NUM_KEYCODES  0x100

/* Yeah these are missing */
#ifndef DISPLAY_DEVICE_ACTIVE
	#define DISPLAY_DEVICE_ACTIVE 0x001
#endif

#ifndef EDS_ROTATEMODE
	#define EDS_ROTATEMODE 0x004
#endif

#ifndef WM_MOUSEHWHEEL
	#define WM_MOUSEHWHEEL 0x020e
#endif


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
 * Win32 Window & Screen (a.k.a Display Device)
 *******************************************************/

/** Win32 Window Flags */
typedef enum GFX_Win32_Flags
{
	GFX_WIN32_MOUSEINSIDE  = 0x01,
	GFX_WIN32_FULLSCREEN   = 0x02

} GFX_Win32_Flags;


/** Win32 Screen */
typedef struct GFX_Win32_Screen
{
	TCHAR  name[32];
	int    width;  /* Actual width, not virtual */
	int    height; /* Actual height, not virtual */

} GFX_Win32_Screen;


/** Win32 Window */
typedef struct GFX_Win32_Window
{
	HWND               handle; /* Given to the outside world */
	GFX_Win32_Screen*  screen;
	HGLRC              context;
	GFX_Win32_Flags    flags;

} GFX_Win32_Window;


/********************************************************
 * Win32 Instance
 *******************************************************/

/** Win32 Instance & data */
typedef struct GFX_Win32_Instance
{
	/* Monitors and Windows */
	GFXVector  screens;         /* Stores GFX_Win32_Screen */
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
 * Converts a UTF-8 string to a UTF-16 string.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
WCHAR* _gfx_win32_utf8_to_utf16(

		const char* str);

/**
 * Converts a UTF-16 string to a UTF-8 string.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
char* _gfx_win32_utf16_to_utf8(

		const WCHAR* str);

/**
 * Returns a Win32 window from its handle.
 *
 */
GFX_Win32_Window* _gfx_win32_get_window_from_handle(

		HWND handle);

/**
 * Returns a Win32 window from its context.
 *
 */
GFX_Win32_Window* _gfx_win32_get_window_from_context(

		HGLRC context);

/**
 * Creates a dummy window for offscreen contexts.
 *
 * @return The dummy window, NULL on failure.
 *
 * Note: the window can be freed by _gfx_platform_window_free.
 *
 */
GFX_Win32_Window* _gfx_win32_window_dummy_create(void);

/**
 * Sets the pixel format for a window.
 *
 * @param backBuffer Non-zero to enable double buffering.
 *
 */
void _gfx_win32_set_pixel_format(

		HWND                  handle,
		const GFXColorDepth*  depth,
		int                   backBuffer);


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_PLATFORM_WIN32_H
