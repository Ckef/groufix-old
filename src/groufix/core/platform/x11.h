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

#ifndef GFX_CORE_PLATFORM_X11_H
#define GFX_CORE_PLATFORM_X11_H


/* Validate platform */
#include "groufix/core/renderer.h"

#if !defined(GFX_GL)
	#error "Must compile X11 target using desktop GL"
#elif !defined(GFX_UNIX)
	#error "Cannot compile X11 target on this platform"
#endif


/* Includes */
#include "groufix/containers/vector.h"

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/Xrandr.h>

#if defined(GFX_RENDERER_GL)
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#endif


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


/* Maximum key code lookup */
#define GFX_X11_MAX_KEYCODE   0x0ff
#define GFX_X11_NUM_KEYCODES  0x100

/* Max error length */
#define GFX_X11_ERROR_LENGTH  0x100


/********************************************************
 * Vital X11 Extensions
 *******************************************************/

#if defined(GFX_RENDERER_GL)

/** X11 Extensions */
typedef struct GFX_X11_Extensions
{
	PFNGLXCREATECONTEXTATTRIBSARBPROC  CreateContextAttribsARB;
	PFNGLXSWAPINTERVALEXTPROC          SwapIntervalEXT;
	unsigned char                      EXT_swap_control_tear;

} GFX_X11_Extensions;

#endif


/********************************************************
 * X11 Window & Monitor
 *******************************************************/

/** X11 Window Flags */
typedef enum GFX_X11_Flags
{
	GFX_X11_FULLSCREEN  = 0x01,
	GFX_X11_RESIZABLE   = 0x02,
	GFX_X11_HIDDEN      = 0x04

} GFX_X11_Flags;


/** X11 Monitor */
typedef struct GFX_X11_Monitor
{
	Screen*       screen;
	RRCrtc        crtc;
	RRMode        mode;    /* Original mode */
	unsigned int  numModes;
	size_t*       modes;   /* Indices into _gfx_x11->modes */

	int           x;
	int           y;
	unsigned int  width;
	unsigned int  height;

} GFX_X11_Monitor;


/** X11 Mode */
typedef struct GFX_X11_Mode
{
	RRMode          id;
	GFXDisplayMode  mode;

} GFX_X11_Mode;


/** X11 Window */
typedef struct GFX_X11_Window
{
	Window            handle;  /* Given to the outside world */
	GFX_X11_Monitor*  monitor;
	RRMode            mode;    /* Fullscreen mode */
	GFX_X11_Flags     flags;

	int               x;       /* Relative to screen */
	int               y;       /* Relative to screen */
	unsigned int      width;
	unsigned int      height;

#if defined(GFX_RENDERER_GL)

	GLXFBConfig       config;
	GLXContext        context;

#endif

} GFX_X11_Window;


/********************************************************
 * X11 Connection
 *******************************************************/

/** X11 Connection & data */
typedef struct GFX_X11_Connection
{
	/* X Display and Windows */
	Display*      display;
	GFXVector     monitors;                 /* Stores GFX_X11_Monitor */
	GFXVector     modes;                    /* Stores GFX_X11_Mode */
	GFXVector     windows;                  /* Stores GFX_X11_Window */
	char          errors;                   /* Zero to ignore errors */

	/* Atoms */
	Atom          MOTIF_WM_HINTS;           /* _MOTIF_WM_HINTS */
	Atom          NET_WM_BYPASS_COMPOSITOR; /* _NET_WM_BYPASS_COMPOSITOR */
	Atom          NET_WM_STATE;             /* _NET_WM_STATE */
	Atom          NET_WM_STATE_ABOVE;       /* _NET_WM_STATE_ABOVE */
	Atom          WM_DELETE_WINDOW;         /* WM_DELETE_WINDOW */

	/* Key table */
	GFXKey        keys[GFX_X11_NUM_KEYCODES];

#if defined(GFX_RENDERER_GL)
	GFX_X11_Extensions extensions;
#endif

} GFX_X11_Connection;


/** Connection pointer */
extern GFX_X11_Connection _gfx_x11;


/**
 * Returns an X11 window from its handle.
 *
 */
GFX_X11_Window* _gfx_x11_get_window_from_handle(

		Window handle);


#endif // GFX_CORE_PLATFORM_X11_H
