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
#include "groufix/screen.h"

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

/* Forward declerate */
struct GFXWindow;


/********************************************************
 * Window Attributes
 *******************************************************/

/* Window event callbacks */
typedef void (*GFXWindowCloseFun)  (struct GFXWindow*);
typedef void (*GFXKeyPressFun)     (struct GFXWindow*, GFXKey, GFXKeyState);
typedef void (*GFXKeyReleaseFun)   (struct GFXWindow*, GFXKey, GFXKeyState);
typedef void (*GFXMouseMoveFun)    (struct GFXWindow*, int, int, GFXKeyState);
typedef void (*GFXMousePressFun)   (struct GFXWindow*, GFXMouseKey, int, int, GFXKeyState);
typedef void (*GFXMouseReleaseFun) (struct GFXWindow*, GFXMouseKey, int, int, GFXKeyState);
typedef void (*GFXMouseWheelFun)   (struct GFXWindow*, int, int, int, int, GFXKeyState);


/** \brief Color depth */
typedef struct GFXColorDepth
{
	unsigned short redBits;
	unsigned short greenBits;
	unsigned short blueBits;

} GFXColorDepth;


/********************************************************
 * Top level windowing
 *******************************************************/

/** \brief A top level window */
typedef struct GFXWindow
{
	/* Callbacks */
	struct
	{
		GFXWindowCloseFun   windowClose;
		GFXKeyPressFun      keyPress;
		GFXKeyReleaseFun    keyRelease;
		GFXMouseMoveFun     mouseMove;
		GFXMousePressFun    mousePress;
		GFXMouseReleaseFun  mouseRelease;
		GFXMouseWheelFun    mouseWheel;

	} callbacks;

} GFXWindow;


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
 * \brief Swaps the internal buffers of a window.
 *
 */
void gfx_window_swap_buffers(const GFXWindow* window);


#ifdef __cplusplus
}
#endif

#endif // GFX_WINDOW_H
