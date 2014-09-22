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

#ifndef GFX_CORE_PLATFORM_H
#define GFX_CORE_PLATFORM_H

#include "groufix/core/window.h"
#include "groufix/utils.h"

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


/* Required platform headers */
#if defined(GFX_WIN32)

	/* Nothing extra */
	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN

	#include <windows.h>

#elif defined(GFX_UNIX)

	#include <pthread.h>

#endif

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Platform process/thread definitions
 *******************************************************/

/** Process Address */
typedef void (*GFX_ProcAddress)(void);


/** Thread Address */
typedef unsigned int (*GFX_ThreadAddress)(void*);


/** A Thread */
#if defined(GFX_WIN32)
typedef HANDLE GFX_PlatformThread;

#elif defined(GFX_UNIX)
typedef pthread_t GFX_PlatformThread;

#else
typedef void* GFX_PlatformThread;

#endif


/** A Mutex */
#if defined(GFX_WIN32)
typedef CRITICAL_SECTION GFX_PlatformMutex;

#elif defined(GFX_UNIX)
typedef pthread_mutex_t GFX_PlatformMutex;

#else
typedef void* GFX_PlatformMutex;

#endif


/** A Condition Variable */
#if defined(GFX_WIN32)
typedef CONDITION_VARIABLE GFX_PlatformCond;

#elif defined(GFX_UNIX)
typedef pthread_cond_t GFX_PlatformCond;

#else
typedef void* GFX_PlatformCond;

#endif


/** Thread local data key */
#if defined(GFX_WIN32)
typedef DWORD GFX_PlatformKey;

#elif defined(GFX_UNIX)
typedef pthread_key_t GFX_PlatformKey;

#else
typedef void* GFX_PlatformKey;

#endif


/********************************************************
 * Platform window manager definitions
 *******************************************************/

/** A Screen */
typedef void* GFX_PlatformScreen;


/** A Window */
typedef void* GFX_PlatformWindow;


/** Window initialization attributes */
typedef struct GFX_PlatformAttributes
{
	GFX_PlatformScreen  screen;
	const char*         name;

	unsigned int        width;
	unsigned int        height;
	int                 x;
	int                 y;

	GFXColorDepth       depth;
	GFXWindowFlags      flags;

} GFX_PlatformAttributes;


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
 * Threading
 *******************************************************/

/**
 * Initializes a new thread.
 *
 * @param thread   Returns the thread handle.
 * @param func     Starting address of the thread, cannot be NULL.
 * @param arg      Argument to give to func.
 * @param joinable If zero, the thread cannot be joined.
 * @return Zero on failure.
 *
 */
int _gfx_platform_thread_init(

		GFX_PlatformThread*  thread,
		GFX_ThreadAddress    func,
		void*                arg,
		int                  joinable);

/**
 * Detaches a thread, making it unjoinable.
 *
 */
void _gfx_platform_thread_detach(

		GFX_PlatformThread thread);

/**
 * Wait for a thread to terminate and frees all resources associated with it.
 *
 * @param ret Value the thread has returned (can be NULL).
 * @return Zero on failure.
 *
 * Note: if a thread was created to be joinable, this call must be made to free resources.
 *
 */
int _gfx_platform_thread_join(

		GFX_PlatformThread  thread,
		unsigned int*       ret);

/**
 * Exits the calling thread.
 *
 * @param ret Return value of the thread.
 *
 */
void _gfx_platform_thread_exit(

		unsigned int ret);

/**
 * Initializes a new thread local data key.
 *
 * @param key Returns the key object.
 * @return Zero on failure.
 *
 */
int _gfx_platform_key_init(

		GFX_PlatformKey* key);

/**
 * Makes sure the data key is freed properly.
 *
 * Note: this does not free any of the associated values!
 *
 */
void _gfx_platform_key_clear(

		GFX_PlatformKey key);

/**
 * Associate a thread specific value with a data key.
 *
 * @return Zero on failure.
 *
 */
int _gfx_platform_key_set(

		GFX_PlatformKey  key,
		void*            value);

/**
 * Retrieve the thread specific value associated with a data key.
 *
 * @return The stored value, NULL if no value is associated.
 *
 */
void* _gfx_platform_key_get(

		GFX_PlatformKey key);

/**
 * Initializes a new mutex.
 *
 * @param mutex Returns the mutex object.
 * @return Zero on failure.
 *
 * Note: NEVER copy the initialized mutex, the same pointer must always be used!
 *
 */
int _gfx_platform_mutex_init(

		GFX_PlatformMutex* mutex);

/**
 * Makes sure a mutex is freed properly.
 *
 * Note: Clearing a locked mutex results in undefined behaviour.
 *
 */
void _gfx_platform_mutex_clear(

		GFX_PlatformMutex* mutex);

/**
 * Blocks until the calling thread is granted ownership of the mutex.
 *
 * @return Zero on failure (no blocking occurred).
 *
 * Note: locking a mutex you already own results in undefined behaviour.
 *
 */
int _gfx_platform_mutex_lock(

		GFX_PlatformMutex* mutex);

/**
 * Try to get ownership of the mutex, but do not block.
 *
 * @return Non-zero if ownership was granted.
 *
 */
int _gfx_platform_mutex_try_lock(

		GFX_PlatformMutex* mutex);

/**
 * Releases the mutex, making it available to other threads.
 *
 * Note: unlocking a mutex which was not locked results in undefined behaviour.
 *
 */
void _gfx_platform_mutex_unlock(

		GFX_PlatformMutex* mutex);

/**
 * Initializes a new condition variable.
 *
 * @param cond Returns the condition object.
 * @return Zero on failure.
 *
 * Note: NEVER copy the initialized condition, the same pointer must always be used!
 *
 */
int _gfx_platform_cond_init(

		GFX_PlatformCond* cond);

/**
 * Makes sure a condition variable is freed properly.
 *
 * Note: Clearing a condition upon which threads are waiting results in undefined behaviour.
 *
 */
void _gfx_platform_cond_clear(

		GFX_PlatformCond* cond);

/**
 * Releases the mutex and blocks until the condition variable is signalled.
 *
 * @return Zero on failure (no blocking occurred), the mutex shall be locked when this call returns.
 *
 * Note: waiting with an unlocked mutex or with different mutexes is undefined behaviour.
 *
 */
int _gfx_platform_cond_wait(

		GFX_PlatformCond*   cond,
		GFX_PlatformMutex*  mutex);

/**
 * Behaves like _gfx_platform_cond_wait, except it returns when a nsec nanoseconds have passed.
 *
 * @return Negative if a minimum of nsec nanoseconds have passed, otherwise equivalent to cond_wait.
 *
 * Note: the time might not be as accurate on all implementations.
 *
 */
int _gfx_platform_cond_wait_time(

		GFX_PlatformCond*   cond,
		GFX_PlatformMutex*  mutex,
		uint64_t            nsec);

/**
 * Unblocks at least one of the threads waiting for the condition variable.
 *
 * If no threads are waiting, the call simply returns.
 *
 */
void _gfx_platform_cond_signal(

		GFX_PlatformCond* cond);

/**
 * Unblocks all threads waiting for the condition variable.
 *
 * If no threads are waiting, the call simply returns.
 *
 */
void _gfx_platform_cond_broadcast(

		GFX_PlatformCond* cond);


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
GFX_PlatformScreen _gfx_platform_get_screen(

		unsigned int num);

/**
 * Returns the default screen.
 *
 */
GFX_PlatformScreen _gfx_platform_get_default_screen(void);

/**
 * Gets the resolution of a screen in pixels.
 *
 */
void _gfx_platform_screen_get_size(

		GFX_PlatformScreen  handle,
		unsigned int*       width,
		unsigned int*       height);


/********************************************************
 * Window creation
 *******************************************************/

/**
 * Creates a new window.
 *
 * @brief attributes The attributes to initialize the window with (cannot be NULL).
 * @return A handle to the window (NULL on failure).
 *
 * Note: if GFX_WINDOW_FULLSCREEN is set, all other flags may be ignored.
 *
 */
GFX_PlatformWindow _gfx_platform_window_create(

		const GFX_PlatformAttributes* attributes);

/**
 * Destroys a window, freeing all its memory.
 *
 * This method is allowed to make all contexts inactive,
 * as this method should also destroy the context if it has one.
 *
 */
void _gfx_platform_window_free(

		GFX_PlatformWindow handle);

/**
 * Returns the screen assigned to a window.
 *
 * @return NULL if the handle was not a previously created window.
 *
 */
GFX_PlatformScreen _gfx_platform_window_get_screen(

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
 */
void _gfx_platform_poll_events(void);


/********************************************************
 * OpenGL Context of window
 *******************************************************/

/**
 * Creates the OpenGL context of a window and makes it current.
 *
 * @param major Major OpenGL (ES) version.
 * @param minor Minor OpenGL (ES) version.
 * @param share Window to share OpenGL resources with (can be NULL to not share).
 * @return Whether or not the context could be created.
 *
 */
int _gfx_platform_context_create(

		GFX_PlatformWindow  handle,
		int                 major,
		int                 minor,
		GFX_PlatformWindow  share);

/**
 * Destroys the context of a window.
 *
 * This method is allowed to make all contexts inactive.
 *
 */
void _gfx_platform_context_free(

		GFX_PlatformWindow handle);

/**
 * Returns the context version of the current window.
 *
 * @param major Return parameter for the major version, 0 on failure.
 * @param minor Return parameter for the minor version, 0 on failure.
 *
 */
void _gfx_platform_context_get(

		int*  major,
		int*  minor);

/**
 * Makes the current window the active render target.
 *
 * @return Whether it could make the context current or not.
 *
 */
void _gfx_platform_context_make_current(

		GFX_PlatformWindow handle);

/**
 * Returns the address to a process of the current context.
 *
 * @return NULL if the process does not exist.
 *
 */
GFX_ProcAddress _gfx_platform_get_proc_address(

		const char* proc);

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
 * Swaps the internal buffers of the window.
 *
 */
void _gfx_platform_context_swap_buffers(

		GFX_PlatformWindow handle);


/********************************************************
 * Event triggers (must be called manually by platform)
 *******************************************************/

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


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_PLATFORM_H
