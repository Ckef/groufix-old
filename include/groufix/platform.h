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

#include "GL/glcorearb.h"

/* Get platform */
#if defined(_WIN32) || defined(__WIN32__)
	#define GFX_WIN32
#elif defined(__unix) || defined(__unix__)
	#define GFX_UNIX
#endif

/* Platform headers */
#if defined(GFX_WIN32)
	#include <windows.h>
#elif defined(GFX_UNIX)
	#include <X11/Xlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Creates a new window, allocating the memory.
 *
 * \return A handle to the window.
 *
 */
void* _gfx_platform_create_window(void);

/**
 * \brief Destroys a window, freeing the memory.
 *
 */
void _gfx_platform_destroy_window(void* handle);

/**
 * \brief Creates a context for a window handle.
 *
 */
void _gfx_platform_create_context(void* handle);

/**
 * \brief Destroys a context of a window handle.
 *
 */
void _gfx_platform_destroy_context(void* handle);

#ifdef __cplusplus
}
#endif

#endif // GFX_PLATFORM_H
