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

#ifndef GFX_INTERNAL_H
#define GFX_INTERNAL_H

#include "groufix/utils.h"
#include "groufix/window.h"

#include <GL/glcorearb.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * \brief OpenGL Extensions
 *******************************************************/
typedef struct GFX_Extensions
{
	PFNGLGETINTEGERVPROC GetIntegerv;

} GFX_Extensions;


/********************************************************
 * Platform definitions
 *******************************************************/

/** \brief Proc Address */
typedef void (*GFXProcAddress)(void);


/** \brief A Screen */
typedef void* GFX_Platform_Screen;


/** \brief A Window */
typedef void* GFX_Platform_Window;


/** \brief Window initialization attributes */
typedef struct GFX_Platform_Attributes
{
	GFX_Platform_Screen  screen;
	const char*          name;

	unsigned int         width;
	unsigned int         height;
	int                  x;
	int                  y;

	GFXColorDepth        depth;

} GFX_Platform_Attributes;


/********************************************************
 * Internal data & methods
 *******************************************************/

/** \brief Internal window */
typedef struct GFX_Internal_Window
{
	/* Super class */
	GFXWindow window;

	/* Hidden data */
	GFX_Platform_Window  handle;
	GFX_Extensions       extensions;

} GFX_Internal_Window;


/**
 * \brief Returns the top level window associated with a platform window.
 *
 */
GFX_Internal_Window* _gfx_window_get_from_handle(GFX_Platform_Window handle);

/**
 * \brief Loads all extensions for the current window's context.
 *
 */
void _gfx_load_extensions(GFX_Extensions* ext);

/**
 * \brief Returns whether the extension can be found in the space seperated string.
 *
 * This method is primarily used in the platform implementations.
 *
 */
int _gfx_is_extension_in_string(const char* str, const char* ext);


/********************************************************
 * Event triggers (must be called manually by platform)
 *******************************************************/

/**
 * \brief Called when a window is requested to close.
 *
 * Used to intercept a user requested window termination.
 *
 */
void _gfx_event_window_close(GFX_Platform_Window handle);

/**
 * \brief Handles a key press event.
 *
 * \param key   What key is pressed.
 * \param state State of some special keys.
 *
 */
void _gfx_event_key_press(GFX_Platform_Window handle, GFXKey key, GFXKeyState state);

/**
 * \brief Handles a key release event.
 *
 * \param key   What key is released.
 * \param state State of some special keys.
 *
 */
void _gfx_event_key_release(GFX_Platform_Window handle, GFXKey key, GFXKeyState state);

/**
 * \brief Called when the cursor moves in a window.
 *
 * \param x     X coordinate of the cursor relative to the window.
 * \param y     Y coordinate of the cursor relative to the window.
 * \param state State of some special keys.
 *
 */
void _gfx_event_mouse_move(GFX_Platform_Window handle, int x, int y, GFXKeyState state);

/**
 * \brief Handles a mouse key press event.
 *
 * \param key   What key is pressed.
 * \param x     X coordinate of the cursor relative to the window.
 * \param y     Y coordinate of the cursor relative to the window.
 * \param state State of some special keys.
 *
 */
void _gfx_event_mouse_press(GFX_Platform_Window handle, GFXMouseKey key, int x, int y, GFXKeyState state);

/**
 * \brief Handles a mouse key release event.
 *
 * \param key   What key is pressed.
 * \param x     X coordinate of the cursor relative to the window.
 * \param y     Y coordinate of the cursor relative to the window.
 * \param state State of some special keys.
 *
 */
void _gfx_event_mouse_release(GFX_Platform_Window handle, GFXMouseKey key, int x, int y, GFXKeyState state);

/**
 * \brief Handles a mouse wheel event.
 *
 * \param xoffset Mouse wheel tilt (negative = left, positive = right).
 * \param yoffset Mouse wheel rotate (negative = down, positive = up).
 * \param x       X coordinate of the cursor relative to the window.
 * \param y       Y coordinate of the cursor relative to the window.
 * \param state   State of some special keys.
 *
 */
void _gfx_event_mouse_wheel(GFX_Platform_Window handle, int xoffset, int yoffset, int x, int y, GFXKeyState state);


#ifdef __cplusplus
}
#endif

#endif // GFX_INTERNAL_H
