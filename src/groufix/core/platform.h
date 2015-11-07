/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#ifndef GFX_CORE_PLATFORM_H
#define GFX_CORE_PLATFORM_H

#include "groufix/core/window.h"


/* Get build target */
#if defined(_WIN32) || defined(__WIN32__)
	#define GFX_WIN32
#elif defined(__APPLE__) || defined(__MACH__)
	#define GFX_OSX
#elif defined(__unix) || defined(__unix__) || defined(__linux__) || defined(__gnu_linux__)
	#define GFX_UNIX

#else
	#error "Platform not supported"
#endif


/* Whether the platform is thread affine or not */
#if defined(GFX_WIN32)
	#define GFX_PLATFORM_THREAD_AFFINE 1
#elif defined(GFX_OSX)
	#define GFX_PLATFORM_THREAD_AFFINE 1
#elif defined(GFX_UNIX)
	#define GFX_PLATFORM_THREAD_AFFINE 1
#endif


/* Required platform headers */
#if defined(GFX_WIN32)

	#define UNICODE
	#define _UNICODE

	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN

	#include <windows.h>

#endif


/********************************************************
 * Platform window manager definitions
 *******************************************************/

/** Process Address */
typedef void (*GFX_ProcAddress) (void);


/** A Monitor */
typedef void* GFX_PlatformMonitor;


/** A Window */
typedef void* GFX_PlatformWindow;


/** A Context */
typedef void* GFX_PlatformContext;


/** Window initialization attributes */
typedef struct GFX_PlatformAttributes
{
	GFX_PlatformMonitor   monitor;
	unsigned int          mode;  /* Guaranteed to be a valid index if fullscreen */
	const GFXColorDepth*  depth; /* Guaranteed to be non-NULL if not fullscreen */
	const char*           name;

	GFXWindowFlags        flags;
	int                   x;
	int                   y;
	unsigned int          w;     /* To be ignored if fullscreen */
	unsigned int          h;     /* To be ignored if fullscreen */

} GFX_PlatformAttributes;


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
 * Terminates the platform.
 *
 */
void _gfx_platform_terminate(void);


/********************************************************
 * System timer
 *******************************************************/

/**
 * Initializes the timer.
 *
 */
void _gfx_platform_init_timer(void);

/**
 * Returns time since some unspecified starting point.
 *
 * The resolution is implementation dependent.
 *
 */
uint64_t _gfx_platform_get_time(void);

/**
 * Returns the resolution of the system timer.
 *
 * This acts as a factor to multiply the time by to get time in seconds.
 *
 */
double _gfx_platform_get_time_resolution(void);


/********************************************************
 * Monitor retrieval
 *******************************************************/

/**
 * Returns the number of visible monitors.
 *
 */
unsigned int _gfx_platform_get_num_monitors(void);

/**
 * Returns a monitor.
 *
 * @param num The number of the monitors (num < num_monitors).
 * @return A handle to the monitor, NULL if not found.
 *
 */
GFX_PlatformMonitor _gfx_platform_get_monitor(

		unsigned int num);

/**
 * Returns the default monitor.
 *
 */
GFX_PlatformMonitor _gfx_platform_get_default_monitor(void);

/**
 * Gets the resolution of a monitor in pixels.
 *
 */
void _gfx_platform_monitor_get_size(

		GFX_PlatformMonitor  handle,
		unsigned int*        width,
		unsigned int*        height);

/**
 * Returns the number of display modes associated with a monitor.
 *
 */
unsigned int _gfx_platform_monitor_get_num_modes(

		GFX_PlatformMonitor handle);

/**
 * Returns a display mode.
 *
 * @param num  The number of the mode (num < num_modes).
 * @param mode Returns the mode, not written to on failure.
 * @return Zero if the mode could not be retrieved.
 *
 */
int _gfx_platform_monitor_get_mode(

		GFX_PlatformMonitor  handle,
		unsigned int         num,
		GFXDisplayMode*      mode);


/********************************************************
 * Window creation
 *******************************************************/

/**
 * Creates a new window.
 *
 * @brief attributes The attributes to initialize the window with (cannot be NULL).
 * @return A handle to the window (NULL on failure).
 *
 * Note: GFX_WINDOW_FULLSCREEN and GFX_WINDOW_RESIZABLE can never both be set.
 *
 */
GFX_PlatformWindow _gfx_platform_window_create(

		const GFX_PlatformAttributes* attributes);

/**
 * Destroys a window, freeing all its memory.
 *
 * This method is allowed to make all contexts inactive,
 * as this method should also clear the associated context if it has one.
 *
 */
void _gfx_platform_window_free(

		GFX_PlatformWindow handle);

/**
 * Returns the monitor assigned to a window.
 *
 * @return NULL if the handle was not a previously created window.
 *
 */
GFX_PlatformMonitor _gfx_platform_window_get_monitor(

		GFX_PlatformWindow handle);

/**
 * Returns the name of a window.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
char* _gfx_platform_window_get_name(

		GFX_PlatformWindow handle);

/**
 * Gets the resolution of a window in pixels.
 *
 */
void _gfx_platform_window_get_size(

		GFX_PlatformWindow  handle,
		unsigned int*       width,
		unsigned int*       height);

/**
 * Gets the position of the window.
 *
 */
void _gfx_platform_window_get_position(

		GFX_PlatformWindow  handle,
		int*                x,
		int*                y);

/**
 * Sets the name of the window.
 *
 */
void _gfx_platform_window_set_name(

		GFX_PlatformWindow  handle,
		const char*         name);

/**
 * Sets the resolution of the window in pixels.
 *
 */
void _gfx_platform_window_set_size(

		GFX_PlatformWindow  handle,
		unsigned int        width,
		unsigned int        height);

/**
 * Sets the position of the window.
 *
 */
void _gfx_platform_window_set_position(

		GFX_PlatformWindow  handle,
		int                 x,
		int                 y);

/**
 * Makes a window visible.
 *
 */
void _gfx_platform_window_show(

		GFX_PlatformWindow handle);

/**
 * Hides a window, making it invisible.
 *
 */
void _gfx_platform_window_hide(

		GFX_PlatformWindow handle);

/**
 * Polls events of all windows.
 *
 * @return Zero if the platform is not initialized or terminated.
 *
 */
int _gfx_platform_poll_events(void);


/********************************************************
 * Renderer Context of window
 *******************************************************/

/**
 * Creates a new windowless renderer context.
 *
 * @param handle Allows the implementation to return a dummy window.
 * @param major  Major context version.
 * @param minor  Minor context version.
 * @param share  Context to share resources with (can be NULL to not share).
 * @param debug  Non-zero to enable extensive debugging against a performance cost.
 * @return A handle to the context (NULL on failure).
 *
 * Note: the dummy window may only be used in _gfx_platform_context_make_current.
 *
 */
GFX_PlatformContext _gfx_platform_context_create(

		GFX_PlatformWindow*  handle,
		int                  major,
		int                  minor,
		GFX_PlatformContext  share,
		int                  debug);

/**
 * Destroys a windowless context.
 *
 * This method is allowed to make all contexts inactive.
 * It should also destroy the dummy window.
 *
 */
void _gfx_platform_context_free(

		GFX_PlatformContext context);

/**
 * Creates the renderer context of a window.
 *
 * @return A handle to the context (NULL on failure).
 *
 */
GFX_PlatformContext _gfx_platform_context_init(

		GFX_PlatformWindow   handle,
		int                  major,
		int                  minor,
		GFX_PlatformContext  share,
		int                  debug);

/**
 * Destroys the context of a window.
 *
 * This method is allowed to make all contexts inactive.
 *
 */
void _gfx_platform_context_clear(

		GFX_PlatformWindow handle);

/**
 * Sets the minimum number of video frame periods per buffer swap.
 *
 * @return The actual value used.
 *
 * A negative value enables adaptive vsync, if not supported, the function behaves as if -num was passed.
 * The absolute value is always used for the minimum frame periods.
 *
 * This method may make the window's context current.
 *
 */
int _gfx_platform_context_set_swap_interval(

		GFX_PlatformWindow  handle,
		int                 num);

/**
 * Swaps the internal buffers of the window's context.
 *
 */
void _gfx_platform_context_swap_buffers(

		GFX_PlatformWindow handle);

/**
 * Makes a context the active context.
 *
 * @param handle  Associated window, in case of windowless, the returned dummy window.
 * @param context Context to make current, NULL to unmake any context current.
 *
 * This is the only function required to be thread safe.
 *
 */
void _gfx_platform_context_make_current(

		GFX_PlatformWindow   handle,
		GFX_PlatformContext  context);

/**
 * Returns the address to a process of the current context.
 *
 * @return NULL if the process does not exist.
 *
 */
GFX_ProcAddress _gfx_platform_get_proc_address(

		const char* proc);


#endif // GFX_CORE_PLATFORM_H
