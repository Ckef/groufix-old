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

#include "groufix/utils.h"

/* Get build target */
#if defined(_WIN32) || defined(__WIN32__)
	#define GFX_WIN32
#elif defined(__APPLE__) || defined(__MACH__)
	#define GFX_OSX
#elif defined(__unix) || defined(__unix__) || defined(__linux__)
	#define GFX_UNIX
#else
	#error "Platform not supported"
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
 * \brief Single Screen of the display
 *******************************************************/
typedef void* GFX_Platform_Screen;


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
GFX_Platform_Screen _gfx_platform_get_screen(unsigned int num);

/**
 * \brief Returns the default screen.
 *
 */
GFX_Platform_Screen _gfx_platform_get_default_screen(void);

/**
 * \brief Gets the resolution of a screen in pixels.
 *
 */
void _gfx_platform_screen_get_size(GFX_Platform_Screen handle, unsigned int* width, unsigned int* height);


/********************************************************
 * \brief A Window
 *******************************************************/
typedef void* GFX_Platform_Window;


/** \brief Window initialization attributes */
typedef struct GFX_Platform_Attributes
{
	GFX_Platform_Screen  screen;
	char*                name;

	unsigned int         width;
	unsigned int         height;
	int                  x;
	int                  y;

	unsigned short       redBits;
	unsigned short       greenBits;
	unsigned short       blueBits;

} GFX_Platform_Attributes;


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
GFX_Platform_Window _gfx_platform_get_window(unsigned int num);

/**
 * \brief Creates a new window and OpenGL context, allocating the memory.
 *
 * \brief attributes The attributes to initialize the window with (cannot be NULL).
 * \return A handle to the window.
 *
 * The created context will be made current as well.
 *
 */
GFX_Platform_Window _gfx_platform_create_window(const GFX_Platform_Attributes* attributes);

/**
 * \brief Destroys a window, freeing the memory.
 *
 */
void _gfx_platform_destroy_window(GFX_Platform_Window handle);

/**
 * \brief Returns the screen assigned to a window.
 *
 * \return NULL if the handle was not a previously created window.
 *
 */
GFX_Platform_Screen _gfx_platform_window_get_screen(GFX_Platform_Window handle);

/**
 * \brief Returns the name of a window.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
char* _gfx_platform_window_get_name(GFX_Platform_Window handle);

/**
 * \brief Gets the resolution of a window in pixels.
 *
 */
void _gfx_platform_window_get_size(GFX_Platform_Window handle, unsigned int* width, unsigned int* height);

/**
 * \brief Gets the position of the window.
 *
 */
void _gfx_platform_window_get_position(GFX_Platform_Window handle, int* x, int* y);

/**
 * \brief Sets the title of a window.
 *
 */
void _gfx_platform_window_set_name(GFX_Platform_Window handle, const char* name);

/**
 * \brief Sets the resolution of the window in pixels.
 *
 */
void _gfx_platform_window_set_size(GFX_Platform_Window handle, unsigned int width, unsigned int height);

/**
 * \brief Sets the position of the window.
 *
 */
void _gfx_platform_window_set_position(GFX_Platform_Window handle, int x, int y);

/**
 * \brief Makes a window visible.
 *
 */
void _gfx_platform_window_show(GFX_Platform_Window handle);

/**
 * \brief Hides a window, making it invisible.
 *
 */
void _gfx_platform_window_hide(GFX_Platform_Window handle);


/********************************************************
 * \brief Window context handling
 *******************************************************/

/**
 * \brief Makes the current window the active render target.
 *
 */
void _gfx_platform_window_make_current(GFX_Platform_Window handle);

/** 
 * \brief Swaps the internal buffers of a window.
 *
 */
void _gfx_platform_window_swap_buffers(GFX_Platform_Window handle);


/********************************************************
 * \brief Event polling
 *******************************************************/

/**
 * \brief Polls events of all windows.
 *
 */
void _gfx_platform_poll_events(void);


#ifdef __cplusplus
}
#endif

#endif // GFX_PLATFORM_H
