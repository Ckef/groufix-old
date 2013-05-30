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

#ifndef GFX_PLATFORM_H
#define GFX_PLATFORM_H

/* Get platform */
#if defined(_WIN32) || defined(__WIN32__)
	#define GFX_WIN32
#elif defined(__unix) || defined(__unix__) || defined(__linux__)
	#define GFX_X11
#else
	#error "Platform not supported"
#endif

/* Platform file */
#if defined(GFX_WIN32)
	#include "groufix/platform/win32.h"
#elif defined(GFX_X11)
	#include "groufix/platform/x11.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Initialization
 *******************************************************/

/**
 * \brief Initializes the platform.
 *
 * \return If successfully initialized, a non-zero value is returned.
 *
 */
int _gfx_platform_init(void);

/**
 * \brief Returns a non-zero value if the platform is initialized correctly.
 *
 */
int _gfx_platform_is_initialized(void);

/**
 * \brief Terminates the platform.
 *
 */
void _gfx_platform_terminate(void);


/********************************************************
 * \brief Screens
 *******************************************************/

/**
 * \brief Returns a screen.
 *
 * \param num The number of the screens (0 <= num < num_screens).
 * \return A handle to the screen, NULL if not found.
 *
 */
void* _gfx_platform_get_screen(int num);

/**
 * \brief Returns the default screen.
 *
 */
void* _gfx_platform_get_default_screen();

/**
 * \brief Returns the number of visible screens.
 *
 */
int _gfx_platform_get_num_screens(void);

/**
 * \brief Returns the width of a screen in pixels.
 *
 */
int _gfx_platform_screen_get_width(void* handle);

/**
 * \brief Returns the height of a screen in pixels.
 *
 */
int _gfx_platform_screen_get_height(void* handle);


/********************************************************
 * Window & Context
 *******************************************************/

/**
 * \brief Creates a new window, allocating the memory.
 *
 * \brief screen A handle to the screen to create the window on.
 * \return A handle to the window.
 *
 */
void* _gfx_platform_create_window(void* screen);

/**
 * \brief Destroys a window, freeing the memory.
 *
 */
void _gfx_platform_destroy_window(void* handle);

/**
 * \brief Creates a context for a window handle.
 *
 * \brief handle A handle to the window to create the context for.
 *
 */
void _gfx_platform_create_context(void* handle);

/**
 * \brief Destroys a context of a window handle.
 *
 * \brief handle A handle to the window to destroy the context for.
 *
 */
void _gfx_platform_destroy_context(void* handle);


#ifdef __cplusplus
}
#endif

#endif // GFX_PLATFORM_H
