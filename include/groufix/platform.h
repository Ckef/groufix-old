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

#include "groufix/events.h"

/* Get platform */
#if defined(_WIN32) || defined(__WIN32__)
	#define GFX_WIN32
#elif defined(__APPLE__) || defined(__MACH__)
	#define GFX_X11
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
 * \brief Window initialization attributes
 *******************************************************/
typedef struct GFX_Platform_Attributes
{
	void*         screen;
	char*         name;

	unsigned int  width;
	unsigned int  height;
	int           x;
	int           y;

} GFX_Platform_Attributes;


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
 * \brief Returns the number of visible screens.
 *
 */
unsigned int _gfx_platform_get_num_screens(void);

/**
 * \brief Returns a screen.
 *
 * \param num The number of the screens (num < num_screens).
 * \return A handle to the screen, NULL if not found.
 *
 */
void* _gfx_platform_get_screen(unsigned int num);

/**
 * \brief Returns the default screen.
 *
 */
void* _gfx_platform_get_default_screen(void);

/**
 * \brief Gets the resolution of a screen in pixels.
 *
 */
void _gfx_platform_screen_get_size(void* handle, unsigned int* width, unsigned int* height);


/********************************************************
 * Windows
 *******************************************************/

/**
 * \brief Polls events of all windows.
 *
 */
void _gfx_platform_poll_events(void);

/**
 * \brief Returns the number of windows.
 *
 */
unsigned int _gfx_platform_get_num_windows(void);

/**
 * \brief Returns a previously created window.
 *
 * \param num The number of the window (num < num_windows).
 * \return A handle to the window, NULL if not found.
 *
 * The number of a window is not constant over the duration of the program.
 * This method is only meant for iterating over existing windows.
 *
 */
void* _gfx_platform_get_window(unsigned int num);

/**
 * \brief Creates a new window, allocating the memory.
 *
 * \brief attributes The attributes to initialize the window with (cannot be NULL).
 * \return A handle to the window.
 *
 */
void* _gfx_platform_create_window(const GFX_Platform_Attributes* attributes);

/**
 * \brief Destroys a window, freeing the memory.
 *
 */
void _gfx_platform_destroy_window(void* handle);

/**
 * \brief Returns the screen assigned to a window.
 *
 * \return NULL if the handle was not a previously created window.
 *
 */
void* _gfx_platform_window_get_screen(void* handle);

/**
 * \brief Returns the name of a window.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
char* _gfx_platform_window_get_name(void* handle);

/**
 * \brief Gets the resolution of a window in pixels.
 *
 */
void _gfx_platform_window_get_size(void* handle, unsigned int* width, unsigned int* height);

/**
 * \brief Gets the position of the window.
 *
 */
void _gfx_platform_window_get_position(void* handle, int* x, int* y);

/**
 * \brief Sets the title of a window.
 *
 */
void _gfx_platform_window_set_name(void* handle, const char* name);

/**
 * \brief Sets the resolution of the window in pixels.
 *
 */
void _gfx_platform_window_set_size(void* handle, unsigned int width, unsigned int height);

/**
 * \brief Sets the position of the window.
 *
 */
void _gfx_platform_window_set_position(void* handle, int x, int y);

/**
 * \brief Makes a window visible.
 *
 */
void _gfx_platform_window_show(void* handle);

/**
 * \brief Hides a window, making it invisible.
 *
 */
void _gfx_platform_window_hide(void* handle);


/********************************************************
 * Contexts
 *******************************************************/

/**
 * \brief Creates a context for a window.
 *
 * \return non-zero if the context was successfully created.
 *
 */
int _gfx_platform_window_create_context(void* handle);

/**
 * \brief Destroys a context of a window.
 *
 */
void _gfx_platform_window_destroy_context(void* handle);


#ifdef __cplusplus
}
#endif

#endif // GFX_PLATFORM_H
