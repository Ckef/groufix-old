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

#ifndef GFX_CORE_WINDOW_H
#define GFX_CORE_WINDOW_H

#include "groufix/core/keys.h"
#include "groufix/core/memory.h"

#include <stddef.h>


/********************************************************
 * Top level monitor
 *******************************************************/

/** Display mode */
typedef struct GFXDisplayMode
{
	unsigned int    width;
	unsigned int    height;
	GFXBitDepth     depth;
	unsigned short  refresh; /* Refresh rate in Hz, only used in fullscreen */

} GFXDisplayMode;


/** A top level monitor */
typedef void* GFXMonitor;


/**
 * Returns the number of visible monitors.
 *
 */
GFX_API unsigned int gfx_get_num_monitors(void);

/**
 * Returns a monitor.
 *
 * @param num The number of the monitors (num < gfx_get_num_monitors()).
 *
 */
GFX_API GFXMonitor gfx_get_monitor(

		unsigned int num);

/**
 * Returns the default monitor.
 *
 */
GFX_API GFXMonitor gfx_get_default_monitor(void);

/**
 * Gets the resolution of a monitor in pixels.
 *
 * @param monitor Monitor to retrieve resolution of, NULL for default monitor.
 *
 */
GFX_API void gfx_monitor_get_size(

		GFXMonitor     monitor,
		unsigned int*  width,
		unsigned int*  height);

/**
 * Returns the number of display modes associated with a monitor.
 *
 * @param monitor Monitor to retrieve number of modes of, NULL for default monitor.
 *
 */
GFX_API unsigned int gfx_monitor_get_num_modes(

		GFXMonitor monitor);

/**
 * Returns a display mode.
 *
 * @param monitor Monitor to retrieve the mode of, NULL for default monitor.
 * @param num     The number of the mode (num < gfx_monitor_get_num_modes()).
 * @param mode    Returns the mode, not written to on failure.
 * @return Zero if the mode could not be retrieved.
 *
 */
GFX_API int gfx_monitor_get_mode(

		GFXMonitor       monitor,
		unsigned int     num,
		GFXDisplayMode*  mode);


/********************************************************
 * Window Callbacks & Metadata
 *******************************************************/

/** Window flags */
typedef enum GFXWindowFlags
{
	GFX_WINDOW_FULLSCREEN     = 0x001,
	GFX_WINDOW_BORDERLESS     = 0x002,
	GFX_WINDOW_RESIZABLE      = 0x004, /* Removed if fullscreen */
	GFX_WINDOW_HIDDEN         = 0x008,
	GFX_WINDOW_DOUBLE_BUFFER  = 0x010

} GFXWindowFlags;


/** Window callbacks */
struct GFXWindow;

typedef void (*GFXWindowCloseFunc)  (struct GFXWindow*);
typedef void (*GFXWindowMoveFunc)   (struct GFXWindow*, int, int);
typedef void (*GFXWindowResizeFunc) (struct GFXWindow*, unsigned int, unsigned int);
typedef void (*GFXWindowFocusFunc)  (struct GFXWindow*);
typedef void (*GFXWindowBlurFunc)   (struct GFXWindow*);
typedef void (*GFXKeyPressFunc)     (struct GFXWindow*, GFXKey, GFXKeyState);
typedef void (*GFXKeyReleaseFunc)   (struct GFXWindow*, GFXKey, GFXKeyState);
typedef void (*GFXMouseMoveFunc)    (struct GFXWindow*, int, int, GFXKeyState);
typedef void (*GFXMouseEnterFunc)   (struct GFXWindow*, int, int, GFXKeyState);
typedef void (*GFXMouseLeaveFunc)   (struct GFXWindow*, int, int, GFXKeyState);
typedef void (*GFXMousePressFunc)   (struct GFXWindow*, GFXMouseKey, int, int, GFXKeyState);
typedef void (*GFXMouseReleaseFunc) (struct GFXWindow*, GFXMouseKey, int, int, GFXKeyState);
typedef void (*GFXMouseWheelFunc)   (struct GFXWindow*, int, int, int, int, GFXKeyState);


/********************************************************
 * Top level windowing
 *******************************************************/

/** A top level window */
typedef struct GFXWindow
{
	/* Bound data, can be modified */
	void* data;

	/* Callbacks, can be modified */
	struct
	{
		GFXWindowCloseFunc   windowClose;
		GFXWindowMoveFunc    windowMove;
		GFXWindowResizeFunc  windowResize;
		GFXWindowFocusFunc   windowFocus;
		GFXWindowBlurFunc    windowBlur;
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


/**
 * Returns the number of open windows.
 *
 */
GFX_API unsigned int gfx_get_num_windows(void);

/**
 * Returns an open window.
 *
 * @param num The number of the window (num < gfx_get_num_windows()).
 * @return The window, NULL if not found.
 *
 * The number of a window can change, this is meant purely for iteration.
 *
 */
GFX_API GFXWindow* gfx_get_window(

		unsigned int num);

/**
 * Creates a new window.
 *
 * @param monitor Monitor to use, NULL for default monitor.
 * @param mode    Index of the display mode to use (only used if fullscreen).
 * @param depth   Color bit depth to use (ignored if fullscreen, last component is always ignored).
 * @param x       X position of the window.
 * @param y       Y position of the window.
 * @param w       Width of the window (ignored if fullscreen).
 * @param h       Height of the window (ignored if fullscreen).
 * @param flags   Flags to apply to this window.
 * @return NULL on failure.
 *
 */
GFX_API GFXWindow* gfx_window_create(

		GFXMonitor          monitor,
		unsigned int        mode,
		const GFXBitDepth*  depth,
		const char*         name,
		int                 x,
		int                 y,
		unsigned int        w,
		unsigned int        h,
		GFXWindowFlags      flags);

/**
 * Recreates a window using a new monitor, mode and flags.
 *
 * @return The new window on success, NULL on failure (old window is still functional).
 *
 * Note: if NULL is returned, the original window is still functional, but hidden.
 *
 */
GFX_API GFXWindow* gfx_window_recreate(

		GFXWindow*          window,
		GFXMonitor          monitor,
		unsigned int        mode,
		const GFXBitDepth*  depth,
		GFXWindowFlags      flags);

/**
 * Destroys and frees the window.
 *
 * Once all windows are destroyed, all hardware related memory is freed.
 * This is done automatically at termination.
 *
 */
GFX_API void gfx_window_free(

		GFXWindow* window);

/**
 * Returns whether a window is still alive.
 *
 * A window can be destroyed by other means than freeing it,
 * this call queries whether the window is still physically open.
 *
 */
GFX_API int gfx_window_is_open(

		const GFXWindow* window);

/**
 * Returns the monitor associated with a window.
 *
 */
GFX_API GFXMonitor gfx_window_get_monitor(

		const GFXWindow* window);

/**
 * Gets the name of the window.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
GFX_API char* gfx_window_get_name(

		const GFXWindow* window);

/**
 * Gets the size of the window.
 *
 */
GFX_API void gfx_window_get_size(

		const GFXWindow*  window,
		unsigned int*     width,
		unsigned int*     height);

/**
 * Gets the position of the window.
 *
 */
GFX_API void gfx_window_get_position(

		const GFXWindow*  window,
		int*              x,
		int*              y);

/**
 * Sets the name of the window.
 *
 */
GFX_API void gfx_window_set_name(

		GFXWindow*   window,
		const char*  name);

/**
 * Sets the size of the window.
 *
 */
GFX_API void gfx_window_set_size(

		GFXWindow*    window,
		unsigned int  width,
		unsigned int  height);

/**
 * Sets the position of the window.
 *
 */
GFX_API void gfx_window_set_position(

		GFXWindow*  window,
		int         x,
		int         y);

/**
 * Makes a window visible.
 *
 * Note: this might misbehave if directly called after gfx_window_hide or
 * when the window was just created.
 *
 */
GFX_API void gfx_window_show(

		GFXWindow* window);

/**
 * Makes a window invisible.
 *
 * Note: this might misbehave if directly called after gfx_window_show or
 * when the window was just created.
 *
 */
GFX_API void gfx_window_hide(

		GFXWindow* window);

/**
 * Sets the minimum number of video frame periods per buffer swap.
 *
 * @return The actual value used.
 *
 * A negative value enables adaptive vsync, if not supported, the function behaves as if -num was passed.
 * The absolute value is always used for the minimum frame periods.
 *
 */
GFX_API int gfx_window_set_swap_interval(

		GFXWindow*  window,
		int         num);


#endif // GFX_CORE_WINDOW_H
