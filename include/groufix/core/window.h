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

#ifndef GFX_WINDOW_H
#define GFX_WINDOW_H

#include "groufix/core/keys.h"

#include <stddef.h>

/* Default hardware ID width */
#define GFX_HARDWARE_ID_WIDTH_DEFAULT 24

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Top level screen
 *******************************************************/

/** A top level screen */
typedef void* GFXScreen;


/**
 * Returns the number of visible screens.
 *
 */
unsigned int gfx_get_num_screens(void);

/**
 * Returns a screen.
 *
 * @param num The number of the screens (num < num_screens).
 *
 */
GFXScreen gfx_get_screen(unsigned int num);

/**
 * Returns the default screen.
 *
 */
GFXScreen gfx_get_default_screen(void);

/**
 * Gets the resolution of a screen in pixels.
 *
 */
void gfx_screen_get_size(GFXScreen screen, unsigned int* width, unsigned int* height);


/********************************************************
 * Window Callbacks & Metadata
 *******************************************************/

/* Forward declerate */
struct GFXWindow;

typedef void (*GFXWindowCloseFunc)  (struct GFXWindow*);
typedef void (*GFXWindowMoveFunc)   (struct GFXWindow*, int, int);
typedef void (*GFXWindowResizeFunc) (struct GFXWindow*, unsigned int, unsigned int);
typedef void (*GFXKeyPressFunc)     (struct GFXWindow*, GFXKey, GFXKeyState);
typedef void (*GFXKeyReleaseFunc)   (struct GFXWindow*, GFXKey, GFXKeyState);
typedef void (*GFXMouseMoveFunc)    (struct GFXWindow*, int, int, GFXKeyState);
typedef void (*GFXMouseEnterFunc)   (struct GFXWindow*, int, int, GFXKeyState);
typedef void (*GFXMouseLeaveFunc)   (struct GFXWindow*, int, int, GFXKeyState);
typedef void (*GFXMousePressFunc)   (struct GFXWindow*, GFXMouseKey, int, int, GFXKeyState);
typedef void (*GFXMouseReleaseFunc) (struct GFXWindow*, GFXMouseKey, int, int, GFXKeyState);
typedef void (*GFXMouseWheelFunc)   (struct GFXWindow*, int, int, int, int, GFXKeyState);


/** Color depth */
typedef struct GFXColorDepth
{
	unsigned short redBits;
	unsigned short greenBits;
	unsigned short blueBits;

} GFXColorDepth;


/* OpenGL Context */
typedef struct GFXContext
{
	int major;
	int minor;

} GFXContext;


/********************************************************
 * Top level windowing
 *******************************************************/

/** Window flags */
typedef enum GFXWindowFlags
{
	GFX_WINDOW_FULLSCREEN  = 0x01,
	GFX_WINDOW_RESIZABLE   = 0x02

} GFXWindowFlags;


/** A top level window */
typedef struct GFXWindow
{
	/* Callbacks */
	struct
	{
		GFXWindowCloseFunc   windowClose;
		GFXWindowMoveFunc    windowMove;
		GFXWindowResizeFunc  windowResize;
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
 * Requests a minimal OpenGL Context for new windows.
 *
 */
void gfx_request_context(GFXContext context);

/**
 * Returns the number of windows.
 *
 */
unsigned int gfx_get_num_windows(void);

/**
 * Returns a window.
 *
 * @param num The number of the window (num < num_windows).
 * @return The window, NULL if not found.
 *
 * The number of a screen can change, this is meant purely for iteration.
 *
 */
GFXWindow* gfx_get_window(unsigned int num);

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
GFXWindow* gfx_window_create(GFXScreen screen, GFXColorDepth depth, const char* name, int x, int y, unsigned int w, unsigned int h, GFXWindowFlags flags);

/**
 * Recreates a window.
 *
 * @param screen New screen to use, NULL for default screen.
 * @param flags  Flags to apply to this window, full screen has precedence over all other flags.
 * @return The new window on success, NULL on failure (old window is still functional).
 *
 * This method is to avoid destroying a window, thereby freeing hardware memory.
 *
 */
GFXWindow* gfx_window_recreate(GFXWindow* window, GFXScreen screen, GFXColorDepth depth, GFXWindowFlags flags);

/**
 * Destroys and frees the window.
 *
 * Once all windows are destroyed, all hardware related memory is freed.
 * This is done automatically at termination.
 *
 */
void gfx_window_free(GFXWindow* window);

/**
 * Returns whether a window is still alive.
 *
 * A window can be destroyed by other means than freeing it,
 * this call queries whether the window is still physically open.
 *
 */
int gfx_window_is_open(const GFXWindow* window);

/**
 * Returns the screen associated with a window.
 *
 */
GFXScreen gfx_window_get_screen(const GFXWindow* window);

/**
 * Returns the context of the window.
 *
 */
GFXContext gfx_window_get_context(const GFXWindow* window);

/**
 * Gets the name of the window.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
char* gfx_window_get_name(const GFXWindow* window);

/**
 * Gets the size of the window.
 *
 */
void gfx_window_get_size(const GFXWindow* window, unsigned int* width, unsigned int* height);

/**
 * Gets the position of the window.
 *
 */
void gfx_window_get_position(const GFXWindow* window, int* x, int* y);

/**
 * Sets the name of the window.
 *
 */
void gfx_window_set_name(const GFXWindow* window, const char* name);

/**
 * Sets the size of the window.
 *
 */
void gfx_window_set_size(const GFXWindow* window, unsigned int width, unsigned int height);

/**
 * Sets the position of the window.
 *
 */
void gfx_window_set_position(const GFXWindow* window, int x, int y);

/**
 * Makes a window visible.
 *
 */
void gfx_window_show(const GFXWindow* window);

/**
 * Makes a window invisible.
 *
 */
void gfx_window_hide(const GFXWindow* window);

/**
 * Sets the minimum number of video frame periods per buffer swap.
 *
 * A value of 1 effectively enables vsync, use 0 to disable.
 * A value of -1 enables adaptive vsync.
 *
 */
void gfx_window_set_swap_interval(const GFXWindow* window, int num);


/********************************************************
 * Top level context & extension handling
 *******************************************************/

/** Hardware Extensions */
typedef enum GFXExtension
{
	GFX_EXT_BUFFER_TEXTURE,
	GFX_EXT_GEOMETRY_SHADER,
	GFX_EXT_INSTANCED_ATTRIBUTES,
	GFX_EXT_LAYERED_CUBEMAP,
	GFX_EXT_MULTISAMPLE_TEXTURE,
	GFX_EXT_POLYGON_STATE,
	GFX_EXT_PROGRAM_BINARY,
	GFX_EXT_SEAMLESS_CUBEMAP,
	GFX_EXT_TESSELLATION_SHADER,
	GFX_EXT_TEXTURE_1D,

	GFX_EXT_COUNT

} GFXExtension;


/** Hardware Limits */
typedef enum GFXLimit
{
	GFX_LIM_MAX_BUFFER_PROPERTIES,
	GFX_LIM_MAX_BUFFER_TEXTURE_SIZE,
	GFX_LIM_MAX_COLOR_ATTACHMENTS,
	GFX_LIM_MAX_COLOR_TARGETS,
	GFX_LIM_MAX_CUBEMAP_SIZE,
	GFX_LIM_MAX_FEEDBACK_BUFFERS,
	GFX_LIM_MAX_SAMPLER_PROPERTIES,
	GFX_LIM_MAX_SAMPLES,
	GFX_LIM_MAX_TEXTURE_3D_SIZE,
	GFX_LIM_MAX_TEXTURE_LAYERS,
	GFX_LIM_MAX_TEXTURE_SIZE,
	GFX_LIM_MAX_VERTEX_ATTRIBS,

	GFX_LIM_COUNT

} GFXLimit;


/**
 * Returns whether a given extension is supported or not.
 *
 * Note: if no window is created, thus no context exists, this will return 0.
 *
 */
int gfx_hardware_is_extension_supported(GFXExtension extension);

/**
 * Returns a limit given by the hardware.
 *
 * Note: if no window is created, thus no context exists, this will return -1.
 *
 */
int gfx_hardware_get_limit(GFXLimit limit);

/**
 * Sets the maximum bit width of any hardware ID.
 *
 * This determines how many objects with an hardware ID can be alive at the same time.
 * The actual width is one bit extra,
 * as you can always subtract 1 to get an ID of the passed width (0 is not a valid ID).
 *
 * Note: in order for this call to have any effect,
 * it should be called before hardware objects are created.
 *
 */
void gfx_hardware_set_max_id_width(unsigned char width);

/**
 * Returns the maximum bit width of any hardware ID.
 *
 */
unsigned char gfx_hardware_get_max_id_width(void);


#ifdef __cplusplus
}
#endif

#endif // GFX_WINDOW_H
