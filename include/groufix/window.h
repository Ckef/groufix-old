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

#ifndef GFX_WINDOW_H
#define GFX_WINDOW_H

/* Minimal supported context */
#define GFX_CONTEXT_MAJOR_MIN 3
#define GFX_CONTEXT_MINOR_MIN 2

/* Maximal supported context */
#define GFX_CONTEXT_MAJOR_MAX 4
#define GFX_CONTEXT_MINOR_MAX 3

/* Greatest minor version possible */
#define GFX_CONTEXT_ALL_MINORS_MAX 3;

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Hardware Context & Extension handling
 *******************************************************/

/** Hardware Extensions */
typedef enum GFXExtension
{
	GFX_EXT_GEOMETRY_SHADER,
	GFX_EXT_INSTANCED_ATTRIBUTES,
	GFX_EXT_PROGRAM_BINARY,
	GFX_EXT_TESSELLATION_SHADER,

	GFX_EXT_COUNT

} GFXExtension;


/**
 * Returns whether a given extension is supported or not.
 *
 * Note: if no window is created, thus no context exists, this will return 0.
 *
 */
int gfx_hardware_is_extension_supported(GFXExtension extension);

/**
 * Polls all OpenGL errors at any given time.
 *
 * @param description A description to attach to each error (can be NULL).
 * @return The number of errors encountered.
 * 
 */
unsigned int gfx_hardware_poll_errors(const char* description);


/********************************************************
 * Platform independent key input
 *******************************************************/

/** Key codes */
typedef enum GFXKey
{
	GFX_KEY_UNKNOWN        = 0x0000, /* Must be zero to initialize to unknown */

	GFX_KEY_BACKSPACE      = 0x0008,
	GFX_KEY_TAB            = 0x0009,
	GFX_KEY_CLEAR          = 0x000a,
	GFX_KEY_RETURN         = 0x000b, /* ENTER */
	GFX_KEY_ENTER          = 0x000b,
	GFX_KEY_PAUSE          = 0x000c,
	GFX_KEY_SCROLL_LOCK    = 0x000d,
	GFX_KEY_ESCAPE         = 0x000e,
	GFX_KEY_DELETE         = 0x000f,

	GFX_KEY_HOME           = 0x0010,
	GFX_KEY_LEFT           = 0x0011,
	GFX_KEY_UP             = 0x0012,
	GFX_KEY_RIGHT          = 0x0013,
	GFX_KEY_DOWN           = 0x0014,
	GFX_KEY_PAGE_DOWN      = 0x0015, /* NEXT */
	GFX_KEY_NEXT           = 0x0015,
	GFX_KEY_PAGE_UP        = 0x0016, /* PRIOR */
	GFX_KEY_PRIOR          = 0x0016,
	GFX_KEY_END            = 0x0017,

	GFX_KEY_SELECT         = 0x0018,
	GFX_KEY_PRINT          = 0x0019,
	GFX_KEY_EXECUTE        = 0x001a,
	GFX_KEY_INSERT         = 0x001b,
	GFX_KEY_MENU           = 0x001c,
	GFX_KEY_CANCEL         = 0x001d,
	GFX_KEY_HELP           = 0x001e,
	GFX_KEY_NUM_LOCK       = 0x001f,
	GFX_KEY_SPACE          = 0x0020,

	GFX_KEY_KP_RETURN      = 0x0021, /* KP Enter */
	GFX_KEY_KP_ENTER       = 0x0021,
	GFX_KEY_KP_0           = 0x0022,
	GFX_KEY_KP_1           = 0x0023,
	GFX_KEY_KP_2           = 0x0024,
	GFX_KEY_KP_3           = 0x0025,
	GFX_KEY_KP_4           = 0x0026,
	GFX_KEY_KP_5           = 0x0027,
	GFX_KEY_KP_6           = 0x0028,
	GFX_KEY_KP_7           = 0x0029,
	GFX_KEY_KP_8           = 0x002a,
	GFX_KEY_KP_9           = 0x002b,
	GFX_KEY_KP_MULTIPLY    = 0x002c,
	GFX_KEY_KP_ADD         = 0x002d,
	GFX_KEY_KP_SEPARATOR   = 0x002e,
	GFX_KEY_KP_SUBTRACT    = 0x002f,
	GFX_KEY_KP_DECIMAL     = 0x003a, /* Continue from after Unicode numbers */
	GFX_KEY_KP_DIVIDE      = 0x003b,

	GFX_KEY_F1             = 0x005b, /* Continue from after Unicode alphabet */
	GFX_KEY_F2             = 0x005c,
	GFX_KEY_F3             = 0x005d,
	GFX_KEY_F4             = 0x005e,
	GFX_KEY_F5             = 0x005f,
	GFX_KEY_F6             = 0x0060,
	GFX_KEY_F7             = 0x0061,
	GFX_KEY_F8             = 0x0062,
	GFX_KEY_F9             = 0x0063,
	GFX_KEY_F10            = 0x0064,
	GFX_KEY_F11            = 0x0065,
	GFX_KEY_F12            = 0x0066,
	GFX_KEY_F13            = 0x0067,
	GFX_KEY_F14            = 0x0068,
	GFX_KEY_F15            = 0x0069,
	GFX_KEY_F16            = 0x006a,
	GFX_KEY_F17            = 0x006b,
	GFX_KEY_F18            = 0x006c,
	GFX_KEY_F19            = 0x006d,
	GFX_KEY_F20            = 0x006e,
	GFX_KEY_F21            = 0x006f,
	GFX_KEY_F22            = 0x0070,
	GFX_KEY_F23            = 0x0071,
	GFX_KEY_F24            = 0x0072,

	GFX_KEY_SHIFT_LEFT     = 0x0073,
	GFX_KEY_SHIFT_RIGHT    = 0x0074,
	GFX_KEY_CONTROL_LEFT   = 0x0075,
	GFX_KEY_CONTROL_RIGHT  = 0x0076,
	GFX_KEY_ALT_LEFT       = 0x0077,
	GFX_KEY_ALT_RIGHT      = 0x0078,
	GFX_KEY_SUPER_LEFT     = 0x0079, /* Left windows key */
	GFX_KEY_SUPER_RIGHT    = 0x007a, /* Right windows key */


	/* Convertible to Unicode (char) from here */
	GFX_KEY_0              = 0x0030,
	GFX_KEY_1              = 0x0031,
	GFX_KEY_2              = 0x0032,
	GFX_KEY_3              = 0x0033,
	GFX_KEY_4              = 0x0034,
	GFX_KEY_5              = 0x0035,
	GFX_KEY_6              = 0x0036,
	GFX_KEY_7              = 0x0037,
	GFX_KEY_8              = 0x0038,
	GFX_KEY_9              = 0x0039,

	GFX_KEY_A              = 0x0041,
	GFX_KEY_B              = 0x0042,
	GFX_KEY_C              = 0x0043,
	GFX_KEY_D              = 0x0044,
	GFX_KEY_E              = 0x0045,
	GFX_KEY_F              = 0x0046,
	GFX_KEY_G              = 0x0047,
	GFX_KEY_H              = 0x0048,
	GFX_KEY_I              = 0x0049,
	GFX_KEY_J              = 0x004a,
	GFX_KEY_K              = 0x004b,
	GFX_KEY_L              = 0x004c,
	GFX_KEY_M              = 0x004d,
	GFX_KEY_N              = 0x004e,
	GFX_KEY_O              = 0x004f,
	GFX_KEY_P              = 0x0050,
	GFX_KEY_Q              = 0x0051,
	GFX_KEY_R              = 0x0052,
	GFX_KEY_S              = 0x0053,
	GFX_KEY_T              = 0x0054,
	GFX_KEY_U              = 0x0055,
	GFX_KEY_V              = 0x0056,
	GFX_KEY_W              = 0x0057,
	GFX_KEY_X              = 0x0058,
	GFX_KEY_Y              = 0x0059,
	GFX_KEY_Z              = 0x005a

} GFXKey;


/** Mouse key codes */
typedef enum GFXMouseKey
{
	GFX_MOUSE_KEY_LEFT,
	GFX_MOUSE_KEY_RIGHT,
	GFX_MOUSE_KEY_MIDDLE

} GFXMouseKey;


/** Key states */
typedef enum GFXKeyState
{
	GFX_KEY_STATE_SHIFT        = 0x01,
	GFX_KEY_STATE_CONTROL      = 0x02,
	GFX_KEY_STATE_ALT          = 0x04,
	GFX_KEY_STATE_SUPER        = 0x08,
	GFX_KEY_STATE_CAPS_LOCK    = 0x10,
	GFX_KEY_STATE_NUM_LOCK     = 0x20,
	GFX_KEY_STATE_SCROLL_LOCK  = 0x40,

	GFX_KEY_STATE_NONE         = 0x00,
	GFX_KEY_STATE_CAPITALS     = 0x11, /* Either shift or caps lock is active */
	GFX_KEY_STATE_LOCK_ALL     = 0x70  /* All toggle keys are toggled active */

} GFXKeyState;


/********************************************************
 * Top level screen
 *******************************************************/
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
 * Top level context handling
 *******************************************************/

/* OpenGL Context */
typedef struct GFXContext
{
	int major;
	int minor;

} GFXContext;


/**
 * Requests a minimal OpenGL Context for new windows.
 *
 */
void gfx_request_context(GFXContext context);


/********************************************************
 * Window Callbacks
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


/** Window flags */
typedef enum GFXWindowFlags
{
	GFX_WINDOW_FULLSCREEN  = 0x01,
	GFX_WINDOW_RESIZABLE   = 0x02

} GFXWindowFlags;


/** Color depth */
typedef struct GFXColorDepth
{
	unsigned short redBits;
	unsigned short greenBits;
	unsigned short blueBits;

} GFXColorDepth;


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
 * @param flags  Flags to apply to this window, full screen has precedence over all other flags.
 * @return NULL on failure.
 * 
 */
GFXWindow* gfx_window_create(GFXScreen screen, GFXColorDepth depth, const char* name, unsigned int width, unsigned int height, int x, int y, GFXWindowFlags flags);

/**
 * Recreates a window.
 *
 * @param screen New screen to use, NULL for default screen.
 * @param flags  Flags to apply to this window, full screen has precedence over all other flags.
 * @return zero on failure (old window is still functional).
 *
 * This method is to avoid destructing a window, thereby freeing hardware memory.
 *
 */
int gfx_window_recreate(const GFXWindow* window, GFXScreen screen, GFXColorDepth depth, GFXWindowFlags flags);

/**
 * Destroys and frees the window.
 *
 * Once all windows are destroyed, all hardware related memory is freed.
 * This is done automatically at termination.
 *
 */
void gfx_window_free(GFXWindow* window);

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

/** 
 * Swaps the internal buffers of a window.
 *
 */
void gfx_window_swap_buffers(const GFXWindow* window);


#ifdef __cplusplus
}
#endif

#endif // GFX_WINDOW_H
