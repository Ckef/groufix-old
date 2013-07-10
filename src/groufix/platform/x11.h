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

#ifndef GFX_PLATFORM_X11_H
#define GFX_PLATFORM_X11_H

#include "groufix/platform.h"
#include "groufix/containers/vector.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <GL/glxext.h>

/* More of them buttons! */
#define Button6  6
#define Button7  7

/* Maximum key code lookup */
#define GFX_X11_MAX_KEYCODE   0x0ff
#define GFX_X11_NUM_KEYCODES  0x100

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * \brief Vital X11 Extensions
 *******************************************************/
typedef struct GFX_X11_Extensions
{
	PFNGLXCREATECONTEXTATTRIBSARBPROC  CreateContextAttribsARB;

} GFX_X11_Extensions;


/********************************************************
 * \brief X11 Window
 *******************************************************/
typedef struct GFX_X11_Window
{
	Window       handle;  /* Given to the outside world */
	GLXFBConfig  config;
	GLXContext   context;

} GFX_X11_Window;


/********************************************************
 * \brief X11 Connection
 *******************************************************/
typedef struct GFX_X11_Connection
{
	/* X Display and Windows */
	Display*  display;
	Vector*   windows;        /* Stores GFX_X11_Window */
	Atom      wmDeleteWindow; /* WM_DELETE_WINDOW */

	/* Key table */
	GFXKey    keys[GFX_X11_NUM_KEYCODES];

	/* Extensions */
	GFX_X11_Extensions extensions;

} GFX_X11_Connection;


/**
 * \brief Connection pointer
 */
extern GFX_X11_Connection* _gfx_x11;


/**
 * \brief Returns whether an extension is supported for a given screen.
 *
 */
int _gfx_x11_is_extension_supported(int screenNumber, const char* ext);

/**
 * \brief Returns an X11 window from its handle.
 *
 */
VectorIterator _gfx_x11_get_window_from_handle(Window handle);


#ifdef __cplusplus
}
#endif

#endif // GFX_PLATFORM_X11_H
