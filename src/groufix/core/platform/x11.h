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

#ifndef GFX_CORE_PLATFORM_X11_H
#define GFX_CORE_PLATFORM_X11_H


/* Validate platform */
#include "groufix/core/platform.h"

#if !defined(GFX_GL)
	#error "Must compile X11 target using desktop GL"
#elif !defined(GFX_UNIX)
	#error "Cannot compile X11 target on this platform"
#endif

/* Includes */
#include "groufix/containers/vector.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>


/* Maximum key code lookup */
#define GFX_X11_MAX_KEYCODE   0x0ff
#define GFX_X11_NUM_KEYCODES  0x100

/* Max error length */
#define GFX_X11_ERROR_LENGTH  0x100

/* Missing things */
#ifndef Button6
	#define Button6 6
#endif

#ifndef Button7
	#define Button7 7
#endif

#ifndef MWM_HINTS_DECORATIONS
	#define MWM_HINTS_DECORATIONS 2L
#endif


#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Vital X11 Extensions
 *******************************************************/

/** X11 Extensions */
typedef struct GFX_X11_Extensions
{
	PFNGLXCREATECONTEXTATTRIBSARBPROC  CreateContextAttribsARB;
	PFNGLXSWAPINTERVALEXTPROC          SwapIntervalEXT;
	unsigned char                      EXT_swap_control_tear;

} GFX_X11_Extensions;


/********************************************************
 * X11 Window
 *******************************************************/

/** X11 Window Flags */
typedef enum GFX_X11_Flags
{
	GFX_X11_FULLSCREEN  = 0x01

} GFX_X11_Flags;


/** X11 Window */
typedef struct GFX_X11_Window
{
	Window         handle;  /* Given to the outside world */
	GLXFBConfig    config;
	GLXContext     context;
	GFX_X11_Flags  flags;

	int            x;
	int            y;
	unsigned int   width;
	unsigned int   height;

} GFX_X11_Window;


/********************************************************
 * X11 Connection
 *******************************************************/

/** X11 Connection & data */
typedef struct GFX_X11_Connection
{
	/* X Display and Windows */
	Display*      display;
	GFXVector     windows;           /* Stores GFX_X11_Window */
	char          errors;            /* Zero to ignore errors */

	/* Atoms */
	Atom          activeWindow;      /* _NET_ACTIVE_WINDOW */
	Atom          wmDeleteWindow;    /* WM_DELETE_WINDOW */
	Atom          wmHints;           /* _MOTIF_WM_HINTS */
	Atom          wmState;           /* _NET_WM_STATE */
	Atom          wmStateFullscreen; /* _NET_WM_STATE_FULLSCREEN */

	/* Screensaver */
	unsigned int  saverCount;
	int           saverTimeout;
	int           saverInterval;
	int           saverBlank;
	int           saverExposure;

	/* Key table */
	GFXKey        keys[GFX_X11_NUM_KEYCODES];

	/* Extensions */
	GFX_X11_Extensions extensions;

} GFX_X11_Connection;


/**
 * Connection pointer
 */
extern GFX_X11_Connection* _gfx_x11;


/**
 * Returns an X11 window from its handle.
 *
 */
GFX_X11_Window* _gfx_x11_get_window_from_handle(

		Window handle);

/**
 * Returns an X11 window from its context.
 *
 */
GFX_X11_Window* _gfx_x11_get_window_from_context(

		GLXContext context);


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_PLATFORM_X11_H
