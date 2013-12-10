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

#ifndef GFX_PLATFORM_H
#define GFX_PLATFORM_H

#include "groufix/utils.h"
#include "groufix/window.h"

/* Get build target */
#if defined(_WIN32) || defined(__WIN32__)
	#define GFX_WIN32
#elif defined(__APPLE__) || defined(__MACH__)
	#define GFX_OSX
#elif defined(__unix) || defined(__unix__) || defined(__linux__)
	#define GFX_UNIX

/* Maybe GLES? */
#elif !defined(GFX_GLES)
	#error "Platform not supported"
#endif

/* Windows */
#ifdef GFX_WIN32

	/* Windows XP */
	#ifndef WINVER
	#define WINVER 0x0501
	#endif

	/* Nothing extra */
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef VC_EXTRALEAN
	#define VC_EXTRALEAN
	#endif
	
	/* To avoid redifinitions */
	#include <windows.h>

#endif

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Platform definitions
 *******************************************************/

/** Proc Address */
typedef void (*GFXProcAddress)(void);


/** A Screen */
typedef void* GFX_Platform_Screen;


/** A Window */
typedef void* GFX_Platform_Window;


/** Window initialization attributes */
typedef struct GFX_Platform_Attributes
{
	GFX_Platform_Screen  screen;
	const char*          name;

	unsigned int         width;
	unsigned int         height;
	int                  x;
	int                  y;

	GFXColorDepth        depth;
	GFXWindowFlags       flags;

} GFX_Platform_Attributes;


/********************************************************
 * Event triggers (must be called manually by platform)
 *******************************************************/

/**
 * Called when a window is requested to close.
 *
 * Used to intercept a user requested window termination.
 *
 */
void _gfx_event_window_close(GFX_Platform_Window handle);

/**
 * Called when a window has been moved.
 *
 * @param x The new x coordinate.
 * @param y The new y coordinate.
 *
 */
void _gfx_event_window_move(GFX_Platform_Window handle, int x, int y);

/**
 * Called when a window has been resized.
 *
 * @param width  The new width.
 * @param height The new height.
 *
 */
void _gfx_event_window_resize(GFX_Platform_Window handle, unsigned int width, unsigned int height);

/**
 * Handles a key press event.
 *
 * @param key   What key is pressed.
 * @param state State of some special keys.
 *
 */
void _gfx_event_key_press(GFX_Platform_Window handle, GFXKey key, GFXKeyState state);

/**
 * Handles a key release event.
 *
 * @param key   What key is released.
 * @param state State of some special keys.
 *
 */
void _gfx_event_key_release(GFX_Platform_Window handle, GFXKey key, GFXKeyState state);

/**
 * Called when the cursor moves in a window.
 *
 * @param x     X coordinate of the cursor relative to the window.
 * @param y     Y coordinate of the cursor relative to the window.
 * @param state State of some special keys.
 *
 */
void _gfx_event_mouse_move(GFX_Platform_Window handle, int x, int y, GFXKeyState state);

/**
 * Called when the cursor enters a window.
 *
 * @param x     X coordinate of the cursor relative to the window.
 * @param y     Y coordinate of the cursor relative to the window.
 * @param state State of some special keys.
 *
 */
void _gfx_event_mouse_enter(GFX_Platform_Window handle, int x, int y, GFXKeyState state);

/**
 * Called when the cursor leaves a window.
 *
 * @param x     X coordinate of the cursor relative to the window.
 * @param y     Y coordinate of the cursor relative to the window.
 * @param state State of some special keys.
 *
 */
void _gfx_event_mouse_leave(GFX_Platform_Window handle, int x, int y, GFXKeyState state);

/**
 * Handles a mouse key press event.
 *
 * @param key   What key is pressed.
 * @param x     X coordinate of the cursor relative to the window.
 * @param y     Y coordinate of the cursor relative to the window.
 * @param state State of some special keys.
 *
 */
void _gfx_event_mouse_press(GFX_Platform_Window handle, GFXMouseKey key, int x, int y, GFXKeyState state);

/**
 * Handles a mouse key release event.
 *
 * @param key   What key is pressed.
 * @param x     X coordinate of the cursor relative to the window.
 * @param y     Y coordinate of the cursor relative to the window.
 * @param state State of some special keys.
 *
 */
void _gfx_event_mouse_release(GFX_Platform_Window handle, GFXMouseKey key, int x, int y, GFXKeyState state);

/**
 * Handles a mouse wheel event.
 *
 * @param xoffset Mouse wheel tilt (negative = left, positive = right).
 * @param yoffset Mouse wheel rotate (negative = down, positive = up).
 * @param x       X coordinate of the cursor relative to the window.
 * @param y       Y coordinate of the cursor relative to the window.
 * @param state   State of some special keys.
 *
 */
void _gfx_event_mouse_wheel(GFX_Platform_Window handle, int xoffset, int yoffset, int x, int y, GFXKeyState state);


/********************************************************
 * Initialization
 *******************************************************/

/**
 * Initializes the platform.
 *
 * @return If successfully initialized, a non-zero value is returned.
 *
 */
int _gfx_platform_init(void);

/**
 * Returns a non-zero value if the platform is initialized correctly.
 *
 */
int _gfx_platform_is_initialized(void);

/**
 * Terminates the platform.
 *
 * If the platform was not yet initialized, this method should do nothing.
 *
 */
void _gfx_platform_terminate(void);


/********************************************************
 * Screen retrieval
 *******************************************************/

/**
 * Returns the number of visible screens.
 *
 */
unsigned int _gfx_platform_get_num_screens(void);

/**
 * Returns a screen.
 *
 * @param num The number of the screens (num < num_screens).
 * @return A handle to the screen, NULL if not found.
 *
 */
GFX_Platform_Screen _gfx_platform_get_screen(unsigned int num);

/**
 * Returns the default screen.
 *
 */
GFX_Platform_Screen _gfx_platform_get_default_screen(void);

/**
 * Gets the resolution of a screen in pixels.
 *
 */
void _gfx_platform_screen_get_size(GFX_Platform_Screen handle, unsigned int* width, unsigned int* height);


/********************************************************
 * Window creation
 *******************************************************/

/**
 * Creates a new visible window.
 *
 * @brief attributes The attributes to initialize the window with (cannot be NULL).
 * @return A handle to the window (NULL on failure).
 *
 */
GFX_Platform_Window _gfx_platform_window_create(const GFX_Platform_Attributes* attributes);

/**
 * Destroys a window, freeing all its memory.
 *
 * This method is allowed to make all contexts inactive,
 * as this method should also destroy the context if it has one.
 *
 */
void _gfx_platform_window_free(GFX_Platform_Window handle);

/**
 * Returns the screen assigned to a window.
 *
 * @return NULL if the handle was not a previously created window.
 *
 */
GFX_Platform_Screen _gfx_platform_window_get_screen(GFX_Platform_Window handle);

/**
 * Returns the name of a window.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
char* _gfx_platform_window_get_name(GFX_Platform_Window handle);

/**
 * Gets the resolution of a window in pixels.
 *
 */
void _gfx_platform_window_get_size(GFX_Platform_Window handle, unsigned int* width, unsigned int* height);

/**
 * Gets the position of the window.
 *
 */
void _gfx_platform_window_get_position(GFX_Platform_Window handle, int* x, int* y);

/**
 * Sets the name of the window.
 *
 */
void _gfx_platform_window_set_name(GFX_Platform_Window handle, const char* name);

/**
 * Sets the resolution of the window in pixels.
 *
 */
void _gfx_platform_window_set_size(GFX_Platform_Window handle, unsigned int width, unsigned int height);

/**
 * Sets the position of the window.
 *
 */
void _gfx_platform_window_set_position(GFX_Platform_Window handle, int x, int y);

/**
 * Makes a window visible.
 *
 */
void _gfx_platform_window_show(GFX_Platform_Window handle);

/**
 * Hides a window, making it invisible.
 *
 */
void _gfx_platform_window_hide(GFX_Platform_Window handle);

/**
 * Polls events of all windows.
 *
 */
void _gfx_platform_poll_events(void);


/********************************************************
 * OpenGL Context of window
 *******************************************************/

/**
 * Creates the OpenGL context of a window.
 *
 * @param major Major OpenGL (ES) version.
 * @param minor Minor OpenGL (ES) version.
 * @param share Window to share OpenGL resources with (can be NULL to not share).
 * @return Whether or not the context could be created.
 *
 */
int _gfx_platform_context_create(GFX_Platform_Window handle, int major, int minor, GFX_Platform_Window share);

/**
 * Destroys the context of a window.
 *
 * This method is allowed to make all contexts inactive.
 *
 */
void _gfx_platform_context_free(GFX_Platform_Window handle);

/**
 * Makes the current window the active render target.
 *
 * @return Whether it could make the context current or not.
 *
 */
void _gfx_platform_context_make_current(GFX_Platform_Window handle);

/**
 * Returns the context version of the current window.
 *
 * @param major Return parameter for the major version, 0 on failure.
 * @param minor Return parameter for the minor version, 0 on failure.
 *
 */
void _gfx_platform_context_get(int* major, int* minor);

/**
 * Sets the minimum number of video frame periods per buffer swap.
 *
 * A value of 1 effectively enables vsync, use 0 to disable.
 * A value of -1 enables adaptive vsync, if not supported, the function behaves as if -num was passed.
 *
 * Note: a value > 1 or < -1 is allowed, depending on the implementation.
 * This method may make the window's context current.
 *
 */
void _gfx_platform_context_set_swap_interval(GFX_Platform_Window handle, int num);

/** 
 * Swaps the internal buffers of a window.
 *
 * This method is allowed to make the given context current.
 *
 */
void _gfx_platform_context_swap_buffers(GFX_Platform_Window handle);

/**
 * Returns whether an extension is supported for a given window.
 *
 */
int _gfx_platform_is_extension_supported(GFX_Platform_Window handle, const char* ext);

/**
 * Returns the address to a process of the current context.
 *
 * @return NULL if the process does not exist.
 *
 */
GFXProcAddress _gfx_platform_get_proc_address(const char* proc);


/********************************************************
 * System timer
 *******************************************************/

/**
 * Returns time in seconds since some unspecified starting point.
 *
 * The resolution is implementation dependent.
 *
 */
double _gfx_platform_get_time(void);


#ifdef __cplusplus
}
#endif

#endif // GFX_PLATFORM_H
