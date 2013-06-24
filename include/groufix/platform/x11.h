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

/* Includes */
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

#include "groufix/containers/vector.h"
#include "groufix/events.h"
#include "groufix/utils.h"

/* More of them buttons! */
#define Button6  6
#define Button7  7

/* Maximum key code lookup */
#define GFX_X11_MAX_KEYCODE 0xff
#define GFX_X11_NUM_KEYCODES 0x100

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * \brief X11 Window
 *******************************************************/
typedef struct GFX_X11_Window
{
	Window       handle;  /* Given to the outside world */
	XVisualInfo  info;
	GLXContext   context;

} GFX_X11_Window;


/********************************************************
 * \brief X11 Connection
 *******************************************************/
typedef struct GFX_X11_Connection
{
	/* X Display */
	Display*  display;

	/* Windows */
	Vector*   windows;        /* Stores GFX_X11_Window */
	Atom      wmDeleteWindow; /* WM_DELETE_WINDOW */

	/* Key table */
	GFXKey    keys[GFX_X11_NUM_KEYCODES];

} GFX_X11_Connection;


/**
 * \brief Connection pointer
 */
extern GFX_X11_Connection* _gfx_x11;


/**
 * \brief Returns an X11 window from its handle.
 *
 */
VectorIterator _gfx_x11_get_window_from_handle(Window handle);

/**
 * \brief Xlib error handler.
 *
 */
int _gfx_x11_error_handler(Display* display, XErrorEvent* evt);


/********************************************************
 * \brief X11 Native Access
 *******************************************************/

/**
 * \brief Returns the X11 display.
 *
 */
Display* gfx_x11_get_display(void);

/**
 * \brief Returns an X11 screen.
 *
 */
Screen* gfx_x11_get_screen(GFX_Platform_Screen screen);

/**
 * \brief Returns an X11 window.
 *
 */
Window gfx_x11_get_window(GFX_Platform_Window window);

/**
 * \brief Returns an GLX context.
 *
 */
GLXContext gfx_x11_get_context(GFX_Platform_Window window);


#ifdef __cplusplus
}
#endif

#endif // GFX_PLATFORM_X11_H
