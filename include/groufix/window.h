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

#include "groufix/keys.h"

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
typedef void (*GFXKeyPressFunc)     (struct GFXWindow*, GFXKey, GFXKeyState);
typedef void (*GFXKeyReleaseFunc)   (struct GFXWindow*, GFXKey, GFXKeyState);
typedef void (*GFXMouseMoveFunc)    (struct GFXWindow*, int, int, GFXKeyState);
typedef void (*GFXMousePressFunc)   (struct GFXWindow*, GFXMouseKey, int, int, GFXKeyState);
typedef void (*GFXMouseReleaseFunc) (struct GFXWindow*, GFXMouseKey, int, int, GFXKeyState);
typedef void (*GFXMouseWheelFunc)   (struct GFXWindow*, int, int, int, int, GFXKeyState);


/********************************************************
 * \brief Top level screen
 *******************************************************/
typedef void* GFXScreen;


/**
 * \brief Returns the number of visible screens.
 *
 */
unsigned int gfx_get_num_screens(void);

/**
 * \brief Returns a screen.
 *
 * \param num The number of the screens (num < num_screens).
 *
 */
GFXScreen gfx_get_screen(unsigned int num);

/**
 * \brief Returns the default screen.
 *
 */
GFXScreen gfx_get_default_screen(void);

/**
 * \brief Gets the resolution of a screen in pixels.
 *
 */
void gfx_screen_get_size(GFXScreen screen, unsigned int* width, unsigned int* height);


/********************************************************
 * Top level windowing
 *******************************************************/

/** \brief A top level window */
typedef struct GFXWindow
{
	/* Callbacks */
	struct
	{
		GFXWindowCloseFunc   windowClose;
		GFXKeyPressFunc      keyPress;
		GFXKeyReleaseFunc    keyRelease;
		GFXMouseMoveFunc     mouseMove;
		GFXMousePressFunc    mousePress;
		GFXMouseReleaseFunc  mouseRelease;
		GFXMouseWheelFunc    mouseWheel;

	} callbacks;

} GFXWindow;


/** \brief Color depth */
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
 * \brief Requests a minimal OpenGL Context for new windows.
 *
 */
void gfx_request_context(GFXContext context);

/**
 * \brief Returns the number of windows.
 *
 */
unsigned int gfx_get_num_windows(void);

/**
 * \brief Returns a window.
 *
 * \param num The number of the window (num < num_windows).
 * \return The window, NULL if not found.
 *
 * The number of a screen can change, this is meant purely for iteration.
 *
 */
GFXWindow* gfx_get_window(unsigned int num);

/**
 * \brief Creates a new window.
 *
 * \param screen Screen to use, NULL for default screen.
 * \param depth  Color depth of the window, must be set.
 * \return NULL on failure.
 *
 * The first window created is considered the main static OpenGL context,
 * it cannot be freed untill all other windows have been freed.
 * 
 */
GFXWindow* gfx_window_create(GFXScreen screen, GFXColorDepth depth, const char* name, unsigned int width, unsigned int height, int x, int y);

/**
 * \brief Destroys and frees the window.
 *
 */
void gfx_window_free(GFXWindow* window);

/**
 * \brief Returns the screen associated with a window.
 *
 */
GFXScreen gfx_window_get_screen(const GFXWindow* window);

/**
 * \brief Returns the context of the window.
 *
 */
GFXContext gfx_window_get_context(const GFXWindow* window);

/**
 * \brief Gets the name of the window.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
char* gfx_window_get_name(const GFXWindow* window);

/**
 * \brief Gets the size of the window.
 *
 */
void gfx_window_get_size(const GFXWindow* window, unsigned int* width, unsigned int* height);

/**
 * \brief Gets the position of the window.
 *
 */
void gfx_window_get_position(const GFXWindow* window, int* x, int* y);

/**
 * \brief Sets the name of the window.
 *
 */
void gfx_window_set_name(const GFXWindow* window, const char* name);

/**
 * \brief Sets the size of the window.
 *
 */
void gfx_window_set_size(const GFXWindow* window, unsigned int width, unsigned int height);

/**
 * \brief Sets the position of the window.
 *
 */
void gfx_window_set_position(const GFXWindow* window, int x, int y);

/**
 * \brief Makes a window visible.
 *
 */
void gfx_window_show(const GFXWindow* window);

/**
 * \brief Makes a window invisible.
 *
 */
void gfx_window_hide(const GFXWindow* window);

/**
 * \brief Sets the minimum number of video frame periods per buffer swap.
 *
 * A value of 1 effectively enables vsync, use 0 to disable.
 * A value of -1 enables adaptive vsync.
 *
 */
void gfx_window_set_swap_interval(const GFXWindow* window, int num);

/** 
 * \brief Swaps the internal buffers of a window.
 *
 */
void gfx_window_swap_buffers(const GFXWindow* window);


#ifdef __cplusplus
}
#endif

#endif // GFX_WINDOW_H
