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

#ifndef GFX_PLATFORM_X11_H
#define GFX_PLATFORM_X11_H

/* Validate platform */
#include "groufix/platform.h"

#if defined(GFX_GLES)
	#error "Cannot compile X11 target using GL ES"
#elif !defined(GFX_UNIX)
	#error "Cannot compile X11 target on this platform"
#endif

/* Includes */
#include "groufix/containers/vector.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>

/* More of them buttons! */
#define Button6  6
#define Button7  7

/* Maximum key code lookup */
#define GFX_X11_MAX_KEYCODE   0x0ff
#define GFX_X11_NUM_KEYCODES  0x100

/* Max error length */
#define GFX_X11_ERROR_LENGTH  0x100

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Vital X11 Extensions
 *******************************************************/
typedef struct GFX_X11_Extensions
{
	PFNGLXCREATECONTEXTATTRIBSARBPROC  CreateContextAttribsARB;
	PFNGLXSWAPINTERVALEXTPROC          SwapIntervalEXT;
	unsigned char                      EXT_swap_control_tear;

} GFX_X11_Extensions;


/********************************************************
 * X11 Window
 *******************************************************/
typedef struct GFX_X11_Window
{
	Window        handle;  /* Given to the outside world */
	GLXFBConfig   config;
	GLXContext    context;

	int           x;
	int           y;
	unsigned int  width;
	unsigned int  height;

} GFX_X11_Window;


/********************************************************
 * X11 Connection
 *******************************************************/
typedef struct GFX_X11_Connection
{
	/* X Display and Windows */
	Display*   display;
	GFXVector  windows;           /* Stores GFX_X11_Window */

	/* Atoms */
	Atom       wmDeleteWindow;    /* WM_DELETE_WINDOW */
	Atom       wmState;           /* _NET_WM_STATE */
	Atom       wmStateFullscreen; /* _NET_WM_STATE_FULLSCREEN */

	/* Key table */
	GFXKey     keys[GFX_X11_NUM_KEYCODES];

	/* Extensions */
	GFX_X11_Extensions extensions;

} GFX_X11_Connection;


/**
 * Connection pointer
 */
extern GFX_X11_Connection* _gfx_x11;


/**
 * Returns whether an extension is supported for a given screen.
 *
 */
int _gfx_x11_is_extension_supported(int screenNumber, const char* ext);

/**
 * Returns an X11 window from its handle.
 *
 */
GFXVectorIterator _gfx_x11_get_window_from_handle(Window handle);

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

#endif // GFX_PLATFORM_X11_H
