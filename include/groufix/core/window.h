/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#ifndef GFX_CORE_WINDOW_H
#define GFX_CORE_WINDOW_H

#include "groufix/core/keys.h"
#include "groufix/utils.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Renderer extensions & limits
 *******************************************************/

/** Renderer Extensions */
typedef enum GFXExtension
{
	GFX_EXT_ANISOTROPIC_FILTER,
	GFX_EXT_BUFFER_TEXTURE,
	GFX_EXT_DIRECT_STATE_ACCESS,
	GFX_EXT_GEOMETRY_SHADER,
	GFX_EXT_IMMUTABLE_TEXTURE,
	GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE,
	GFX_EXT_INSTANCED_ATTRIBUTES,
	GFX_EXT_INSTANCED_BASE_ATTRIBUTES,
	GFX_EXT_LAYERED_CUBEMAP,
	GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE,
	GFX_EXT_MULTI_BIND,
	GFX_EXT_MULTISAMPLE_TEXTURE,
	GFX_EXT_POLYGON_STATE,
	GFX_EXT_PROGRAM_BINARY,
	GFX_EXT_PROGRAM_MAP,
	GFX_EXT_SAMPLER_OBJECTS,
	GFX_EXT_SEAMLESS_CUBEMAP,
	GFX_EXT_SEPARATE_VERTEX_BUFFERS,
	GFX_EXT_TESSELLATION_SHADER,
	GFX_EXT_VERTEX_BASE_INDICES,

	GFX_EXT_COUNT

} GFXExtension;


/** Renderer Limits */
typedef enum GFXLimit
{
	GFX_LIM_MAX_ANISOTROPY,
	GFX_LIM_MAX_BUFFER_PROPERTIES,
	GFX_LIM_MAX_BUFFER_TEXTURE_SIZE,
	GFX_LIM_MAX_COLOR_ATTACHMENTS,
	GFX_LIM_MAX_COLOR_TARGETS,
	GFX_LIM_MAX_CUBEMAP_SIZE,
	GFX_LIM_MAX_FEEDBACK_BUFFERS,
	GFX_LIM_MAX_PATCH_VERTICES,
	GFX_LIM_MAX_SAMPLER_PROPERTIES,
	GFX_LIM_MAX_SAMPLES,
	GFX_LIM_MAX_TEXTURE_3D_SIZE,
	GFX_LIM_MAX_TEXTURE_LAYERS,
	GFX_LIM_MAX_TEXTURE_SIZE,
	GFX_LIM_MAX_VERTEX_ATTRIBS,
	GFX_LIM_MAX_VERTEX_ATTRIB_OFFSET,
	GFX_LIM_MAX_VERTEX_BUFFERS,
	GFX_LIM_MAX_VERTEX_STRIDE,

	GFX_LIM_COUNT

} GFXLimit;


/**
 * Returns whether a given extension is supported or not.
 *
 */
GFX_API int gfx_is_extension_supported(

		GFXExtension extension);

/**
 * Returns a limit given by the hardware.
 *
 */
GFX_API int gfx_get_limit(

		GFXLimit limit);


/********************************************************
 * Top level screen
 *******************************************************/

/** A top level screen */
typedef void* GFXScreen;


/**
 * Returns the number of visible screens.
 *
 */
GFX_API unsigned int gfx_get_num_screens(void);

/**
 * Returns a screen.
 *
 * @param num The number of the screens (num < gfx_get_num_screens()).
 *
 */
GFX_API GFXScreen gfx_get_screen(

		unsigned int num);

/**
 * Returns the default screen.
 *
 */
GFX_API GFXScreen gfx_get_default_screen(void);

/**
 * Gets the resolution of a screen in pixels.
 *
 */
GFX_API void gfx_screen_get_size(

		GFXScreen      screen,
		unsigned int*  width,
		unsigned int*  height);


/********************************************************
 * Window Callbacks & Metadata
 *******************************************************/

/** Window flags */
typedef enum GFXWindowFlags
{
	GFX_WINDOW_FULLSCREEN     = 0x0001,
	GFX_WINDOW_BORDERLESS     = 0x0002,
	GFX_WINDOW_RESIZABLE      = 0x0004,
	GFX_WINDOW_HIDDEN         = 0x0008,
	GFX_WINDOW_DOUBLE_BUFFER  = 0x0010

} GFXWindowFlags;


/** Color depth */
typedef struct GFXColorDepth
{
	unsigned short redBits;
	unsigned short greenBits;
	unsigned short blueBits;

} GFXColorDepth;


/* Forward declerate */
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
	/* Callbacks */
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
 * @param screen Screen to use, NULL for default screen.
 * @param x      X position of the window.
 * @param y      Y position of the window.
 * @param w      Width of the window.
 * @param h      Height of the window.
 * @param flags  Flags to apply to this window, full screen has precedence over all other flags.
 * @return NULL on failure.
 *
 */
GFX_API GFXWindow* gfx_window_create(

		GFXScreen       screen,
		GFXColorDepth   depth,
		const char*     name,
		int             x,
		int             y,
		unsigned int    w,
		unsigned int    h,
		GFXWindowFlags  flags);

/**
 * Recreates a window using a new screen, depth and flags.
 *
 * @param screen New screen to use, NULL for default screen.
 * @param flags  Flags to apply to this window, full screen has precedence over all other flags.
 * @return The new window on success, NULL on failure (old window is still functional).
 *
 * Note: if NULL is returned, the original window is still functional, but hidden.
 *
 */
GFX_API GFXWindow* gfx_window_recreate(

		GFXWindow*      window,
		GFXScreen       screen,
		GFXColorDepth   depth,
		GFXWindowFlags  flags);

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
 * Returns the screen associated with a window.
 *
 */
GFX_API GFXScreen gfx_window_get_screen(

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

		const GFXWindow*  window,
		const char*       name);

/**
 * Sets the size of the window.
 *
 */
GFX_API void gfx_window_set_size(

		const GFXWindow*  window,
		unsigned int      width,
		unsigned int      height);

/**
 * Sets the position of the window.
 *
 */
GFX_API void gfx_window_set_position(

		const GFXWindow*  window,
		int               x,
		int               y);

/**
 * Makes a window visible.
 *
 * Note: this might misbehave if directly called after gfx_window_hide.
 *
 */
GFX_API void gfx_window_show(

		const GFXWindow* window);

/**
 * Makes a window invisible.
 *
 * Note: this might misbehave if directly called after gfx_window_show.
 *
 */
GFX_API void gfx_window_hide(

		const GFXWindow* window);

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

		const GFXWindow*  window,
		int               num);


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_WINDOW_H
