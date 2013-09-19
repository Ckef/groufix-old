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

#ifndef GFX_WINDOW_H
#define GFX_WINDOW_H

#include "groufix/utils.h"

/* Minimal supported context */
#define GFX_CONTEXT_MAJOR_MIN 3
#define GFX_CONTEXT_MINOR_MIN 2

/* Maximal supported context */
#define GFX_CONTEXT_MAJOR_MAX 4
#define GFX_CONTEXT_MINOR_MAX 3

/* Greatest minor version possible */
#define GFX_CONTEXT_ALL_MINORS_MAX 3;

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Window Callbacks
 *******************************************************/

/* Forward declerate */
struct GFXWindow;

typedef void (*GFXWindowCloseFunc)  (struct GFXWindow*);
typedef void (*GFXWindowMoveFunc)   (struct GFXWindow*, int, int);
typedef void (*GFXWindowResizeFunc) (struct GFXWindow*, unsigned int, unsigned int);
typedef void (*GFXKeyPressFunc)     (struct GFXWindow*, GFXKey, GFXKeyState);
typedef void (*GFXKeyReleaseFunc)   (struct GFXWindow*, GFXKey, GFXKeyState);
typedef void (*GFXMouseMoveFunc)    (struct GFXWindow*, int, int, GFXKeyState);
typedef void (*GFXMouseEnterFunc)   (struct GFXWindow*, int, int, GFXKeyState);
typedef void (*GFXMouseLeaveFunc)   (struct GFXWindow*, int, int, GFXKeyState);
typedef void (*GFXMousePressFunc)   (struct GFXWindow*, GFXMouseKey, int, int, GFXKeyState);
typedef void (*GFXMouseReleaseFunc) (struct GFXWindow*, GFXMouseKey, int, int, GFXKeyState);
typedef void (*GFXMouseWheelFunc)   (struct GFXWindow*, int, int, int, int, GFXKeyState);


/********************************************************
 * Top level screen
 *******************************************************/
typedef void* GFXScreen;


/**
 * Returns the number of visible screens.
 *
 */
unsigned int gfx_get_num_screens(void);

/**
 * Returns a screen.
 *
 * @param num The number of the screens (num < num_screens).
 *
 */
GFXScreen gfx_get_screen(unsigned int num);

/**
 * Returns the default screen.
 *
 */
GFXScreen gfx_get_default_screen(void);

/**
 * Gets the resolution of a screen in pixels.
 *
 */
void gfx_screen_get_size(GFXScreen screen, unsigned int* width, unsigned int* height);


/********************************************************
 * Top level windowing
 *******************************************************/

/** A top level window */
typedef struct GFXWindow
{
	/* Callbacks */
	struct
	{
		GFXWindowCloseFunc   windowClose;
		GFXWindowMoveFunc    windowMove;
		GFXWindowResizeFunc  windowResize;
		GFXKeyPressFunc      keyPress;
		GFXKeyReleaseFunc    keyRelease;
		GFXMouseMoveFunc     mouseMove;
		GFXMouseEnterFunc    mouseEnter;
		GFXMouseLeaveFunc    mouseLeave;
		GFXMousePressFunc    mousePress;
		GFXMouseReleaseFunc  mouseRelease;
		GFXMouseWheelFunc    mouseWheel;

	} callbacks;

} GFXWindow;


/** Color depth */
typedef struct GFXColorDepth
{
	unsigned short redBits;
	unsigned short greenBits;
	unsigned short blueBits;

} GFXColorDepth;


/* OpenGL Context */
typedef struct GFXContext
{
	int major;
	int minor;

} GFXContext;


/**
 * Requests a minimal OpenGL Context for new windows.
 *
 */
void gfx_request_context(GFXContext context);

/**
 * Returns the number of windows.
 *
 */
unsigned int gfx_get_num_windows(void);

/**
 * Returns a window.
 *
 * @param num The number of the window (num < num_windows).
 * @return The window, NULL if not found.
 *
 * The number of a screen can change, this is meant purely for iteration.
 *
 */
GFXWindow* gfx_get_window(unsigned int num);

/**
 * Creates a new window.
 *
 * @param screen Screen to use, NULL for default screen.
 * @param depth  Color depth of the window.
 * @return NULL on failure.
 * 
 */
GFXWindow* gfx_window_create(GFXScreen screen, GFXColorDepth depth, const char* name, unsigned int width, unsigned int height, int x, int y);

/**
 * Recreates a window.
 *
 * @param screen New screen to use, NULL for default screen.
 * @param depth  New color depth for the window.
 * @return zero on failure (old window is still functional).
 *
 * This method is to avoid destructing a window, thereby freeing hardware memory.
 *
 */
int gfx_window_recreate(const GFXWindow* window, GFXScreen screen, GFXColorDepth depth);

/**
 * Destroys and frees the window.
 *
 * Once all windows are destroyed, all hardware related memory is freed.
 * This is done automatically at termination.
 *
 */
void gfx_window_free(GFXWindow* window);

/**
 * Returns the screen associated with a window.
 *
 */
GFXScreen gfx_window_get_screen(const GFXWindow* window);

/**
 * Returns the context of the window.
 *
 */
GFXContext gfx_window_get_context(const GFXWindow* window);

/**
 * Gets the name of the window.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
char* gfx_window_get_name(const GFXWindow* window);

/**
 * Gets the size of the window.
 *
 */
void gfx_window_get_size(const GFXWindow* window, unsigned int* width, unsigned int* height);

/**
 * Gets the position of the window.
 *
 */
void gfx_window_get_position(const GFXWindow* window, int* x, int* y);

/**
 * Sets the name of the window.
 *
 */
void gfx_window_set_name(const GFXWindow* window, const char* name);

/**
 * Sets the size of the window.
 *
 */
void gfx_window_set_size(const GFXWindow* window, unsigned int width, unsigned int height);

/**
 * Sets the position of the window.
 *
 */
void gfx_window_set_position(const GFXWindow* window, int x, int y);

/**
 * Makes a window visible.
 *
 */
void gfx_window_show(const GFXWindow* window);

/**
 * Makes a window invisible.
 *
 */
void gfx_window_hide(const GFXWindow* window);

/**
 * Sets the minimum number of video frame periods per buffer swap.
 *
 * A value of 1 effectively enables vsync, use 0 to disable.
 * A value of -1 enables adaptive vsync.
 *
 */
void gfx_window_set_swap_interval(const GFXWindow* window, int num);

/** 
 * Swaps the internal buffers of a window.
 *
 */
void gfx_window_swap_buffers(const GFXWindow* window);

/**
 * Swaps the internal buffers of all windows.
 *
 */
void gfx_window_swap_all_buffers(void);


#ifdef __cplusplus
}
#endif

#endif // GFX_WINDOW_H
