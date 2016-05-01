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
#if defined(__APPLE__) || defined(__MACH__)
	#define GFX_OSX
	#error "Platform not yet supported"
#elif defined(__unix) || defined(__unix__)
	#define GFX_UNIX
#elif defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)
	#define GFX_WIN32

#else
	#error "Platform not supported"
#endif


/* Whether the platform is thread affine */
#if defined(GFX_OSX)
	#define GFX_PLATFORM_THREAD_AFFINE 1
#elif defined(GFX_UNIX)
	#define GFX_PLATFORM_THREAD_AFFINE 0
#elif defined(GFX_WIN32)
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

/** A Monitor */
typedef void* GFX_PlatformMonitor;


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

/** A Window */
typedef void* GFX_PlatformWindow;


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

/** A Context */
typedef void* GFX_PlatformContext;


/** Process Address */
typedef void (*GFX_ProcAddress) (void);


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
 * This function is required to NOT be thread affine.
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
 * This function exists in case a renderer needs it.
 *
 */
GFX_ProcAddress _gfx_platform_get_proc_address(

		const char* proc);


/********************************************************
 * Event triggers (must be called manually by platform)
 *******************************************************/

/**
 * Request to terminate.
 *
 * Should be set to non-zero in case the OS or another application
 * requested this application to terminate.
 *
 */
extern unsigned char _gfx_event_terminate_request;


/**
 * Called when a window is requested to close.
 *
 * Used to intercept a user requested window termination.
 *
 */
void _gfx_event_window_close(

		GFX_PlatformWindow handle);

/**
 * Called when a window has been moved.
 *
 * @param x The new x coordinate.
 * @param y The new y coordinate.
 *
 */
void _gfx_event_window_move(

		GFX_PlatformWindow  handle,
		int                 x,
		int                 y);

/**
 * Called when a window has been resized.
 *
 * @param width  The new width.
 * @param height The new height.
 *
 */
void _gfx_event_window_resize(

		GFX_PlatformWindow  handle,
		unsigned int        width,
		unsigned int        height);

/**
 * Called when a window receives input focus.
 *
 */
void _gfx_event_window_focus(

		GFX_PlatformWindow handle);

/**
 * Called when a window loses input focus.
 *
 */
void _gfx_event_window_blur(

		GFX_PlatformWindow handle);

/**
 * Handles a key press event.
 *
 * @param key   What key is pressed.
 * @param state State of some special keys.
 *
 */
void _gfx_event_key_press(

		GFX_PlatformWindow  handle,
		GFXKey              key,
		GFXKeyState         state);

/**
 * Handles a key release event.
 *
 * @param key   What key is released.
 * @param state State of some special keys.
 *
 */
void _gfx_event_key_release(

		GFX_PlatformWindow  handle,
		GFXKey              key,
		GFXKeyState         state);

/**
 * Called when the cursor moves in a window.
 *
 * @param x     X coordinate of the cursor relative to the window.
 * @param y     Y coordinate of the cursor relative to the window.
 * @param state State of some special keys.
 *
 */
void _gfx_event_mouse_move(

		GFX_PlatformWindow  handle,
		int                 x,
		int                 y,
		GFXKeyState         state);

/**
 * Called when the cursor enters a window.
 *
 * @param x     X coordinate of the cursor relative to the window.
 * @param y     Y coordinate of the cursor relative to the window.
 * @param state State of some special keys.
 *
 */
void _gfx_event_mouse_enter(

		GFX_PlatformWindow  handle,
		int                 x,
		int                 y,
		GFXKeyState         state);

/**
 * Called when the cursor leaves a window.
 *
 * @param x     X coordinate of the cursor relative to the window.
 * @param y     Y coordinate of the cursor relative to the window.
 * @param state State of some special keys.
 *
 */
void _gfx_event_mouse_leave(

		GFX_PlatformWindow  handle,
		int                 x,
		int                 y,
		GFXKeyState         state);

/**
 * Handles a mouse key press event.
 *
 * @param key   What key is pressed.
 * @param x     X coordinate of the cursor relative to the window.
 * @param y     Y coordinate of the cursor relative to the window.
 * @param state State of some special keys.
 *
 */
void _gfx_event_mouse_press(

		GFX_PlatformWindow  handle,
		GFXMouseKey         key,
		int                 x,
		int                 y,
		GFXKeyState         state);

/**
 * Handles a mouse key release event.
 *
 * @param key   What key is pressed.
 * @param x     X coordinate of the cursor relative to the window.
 * @param y     Y coordinate of the cursor relative to the window.
 * @param state State of some special keys.
 *
 */
void _gfx_event_mouse_release(

		GFX_PlatformWindow  handle,
		GFXMouseKey         key,
		int                 x,
		int                 y,
		GFXKeyState         state);

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
void _gfx_event_mouse_wheel(

		GFX_PlatformWindow  handle,
		int                 xoffset,
		int                 yoffset,
		int                 x,
		int                 y,
		GFXKeyState         state);


#endif // GFX_CORE_PLATFORM_H
