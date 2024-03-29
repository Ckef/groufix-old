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

#include "groufix/core/platform/x11.h"

#include <stdlib.h>
#include <string.h>

#if defined(GFX_RENDERER_GL)

/******************************************************/
static GLXFBConfig* _gfx_x11_get_config(

		Screen*      screen,
		GFXBitDepth  depth,
		int          backBuffer)
{
	/* Create buffer attribute array */
	int bufferAttr[] = {
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE,   GLX_RGBA_BIT,
		GLX_DOUBLEBUFFER,  backBuffer ? True : False,
		GLX_RED_SIZE,      depth.data[0],
		GLX_GREEN_SIZE,    depth.data[1],
		GLX_BLUE_SIZE,     depth.data[2],
		None
	};

	/* Get config from screen */
	int buffElements;
	return glXChooseFBConfig(
		_gfx_x11.display,
		XScreenNumberOfScreen(screen),
		bufferAttr,
		&buffElements
	);
}

#endif

/******************************************************/
static void _gfx_x11_enter_fullscreen(

		GFX_X11_Monitor*  monitor,
		Window            handle,
		RRMode            mode)
{
	Window root =
		XRootWindowOfScreen(monitor->screen);
	XRRScreenResources* res =
		XRRGetScreenResources(_gfx_x11.display, root);
	XRRCrtcInfo* crtc =
		XRRGetCrtcInfo(_gfx_x11.display, res, monitor->crtc);

	/* Above state */
	XEvent event;
	memset(&event, 0, sizeof(XEvent));

	event.type                 = ClientMessage;
	event.xclient.window       = handle;
	event.xclient.message_type = _gfx_x11.NET_WM_STATE;
	event.xclient.format       = 32;
	event.xclient.data.l[0]    = 1;
	event.xclient.data.l[1]    = _gfx_x11.NET_WM_STATE_ABOVE;

	/* Send event, set mode and move window */
	XSendEvent(
		_gfx_x11.display,
		root,
		False,
		SubstructureRedirectMask | SubstructureNotifyMask,
		&event);

	XRRSetCrtcConfig(
		_gfx_x11.display,
		res,
		monitor->crtc,
		crtc->timestamp,
		crtc->x,
		crtc->y,
		mode,
		crtc->rotation,
		crtc->outputs,
		crtc->noutput);

	XMoveWindow(
		_gfx_x11.display,
		handle,
		crtc->x,
		crtc->y);

	XRRFreeCrtcInfo(crtc);
	XRRFreeScreenResources(res);
}

/******************************************************/
static inline void _gfx_x11_leave_fullscreen(

		GFX_X11_Monitor* monitor)
{
	Window root =
		XRootWindowOfScreen(monitor->screen);
	XRRScreenResources* res =
		XRRGetScreenResources(_gfx_x11.display, root);
	XRRCrtcInfo* crtc =
		XRRGetCrtcInfo(_gfx_x11.display, res, monitor->crtc);

	/* Set mode */
	XRRSetCrtcConfig(
		_gfx_x11.display,
		res,
		monitor->crtc,
		crtc->timestamp,
		crtc->x,
		crtc->y,
		monitor->mode,
		crtc->rotation,
		crtc->outputs,
		crtc->noutput
	);

	XRRFreeCrtcInfo(crtc);
	XRRFreeScreenResources(res);
}

/******************************************************/
static GFXKeyState _gfx_x11_get_key_state(

		unsigned int state)
{
	GFXKeyState st = GFX_KEY_STATE_NONE;

	if(state & ShiftMask)
		st |= GFX_KEY_STATE_SHIFT;
	if(state & (ControlMask | Mod5Mask))
		st |= GFX_KEY_STATE_CONTROL;
	if(state & (Mod1Mask | Mod5Mask))
		st |= GFX_KEY_STATE_ALT;
	if(state & Mod4Mask)
		st |= GFX_KEY_STATE_SUPER;
	if(state & LockMask)
		st |= GFX_KEY_STATE_CAPS_LOCK;
	if(state & Mod2Mask)
		st |= GFX_KEY_STATE_NUM_LOCK;

	return st;
}

/******************************************************/
static void _gfx_x11_event_proc(

		XEvent* event)
{
	/* Get window */
	GFX_PlatformWindow window = GFX_UINT_TO_VOID(event->xany.window);

	switch(event->type)
	{
		/* Protocol messages */
	case ClientMessage :
		{
			if((Atom)event->xclient.data.l[0] == _gfx_x11.WM_DELETE_WINDOW)
				_gfx_event_window_close(window);

			break;
		}

		/* Window resizing & movement */
	case ConfigureNotify :
		{
			GFX_X11_Window* internal =
				_gfx_x11_get_window_from_handle(event->xany.window);

			if(!internal) break;

			if(
				internal->x != event->xconfigure.x ||
				internal->y != event->xconfigure.y)
			{
				internal->x = event->xconfigure.x;
				internal->y = event->xconfigure.y;

				int xS = 0;
				int yS = 0;

				GFX_X11_Monitor* monitor =
					_gfx_platform_window_get_monitor(window);

				if(monitor)
				{
					xS = monitor->x;
					yS = monitor->y;
				}

				_gfx_event_window_move(
					window,
					internal->x - xS,
					internal->y - yS
				);
			}

			if(
				internal->width != (unsigned int)event->xconfigure.width ||
				internal->height != (unsigned int)event->xconfigure.height)
			{
				internal->width = event->xconfigure.width;
				internal->height = event->xconfigure.height;

				_gfx_event_window_resize(
					window,
					internal->width,
					internal->height
				);
			}

			break;
		}

		/* Window gets focus */
	case FocusIn :
		{
			/* Enter fullscreen */
			GFX_X11_Window* internal =
				_gfx_x11_get_window_from_handle(event->xany.window);

			if(!internal) break;

			if(
				(internal->flags & GFX_X11_FULLSCREEN) &&
				!(internal->flags & GFX_X11_HIDDEN))
			{
				_gfx_x11_enter_fullscreen(
					internal->monitor,
					event->xany.window,
					internal->mode
				);
			}

			_gfx_event_window_focus(window);

			break;
		}

		/* Window loses focus */
	case FocusOut :
		{
			/* Leave fullscreen */
			GFX_X11_Window* internal =
				_gfx_x11_get_window_from_handle(event->xany.window);

			if(!internal) break;

			if(internal->flags & GFX_X11_FULLSCREEN)
			{
				_gfx_x11_leave_fullscreen(internal->monitor);

				if(!(internal->flags & GFX_X11_HIDDEN))
					XIconifyWindow(
						_gfx_x11.display,
						event->xany.window,
						XScreenNumberOfScreen(internal->monitor->screen)
					);
			}

			_gfx_event_window_blur(window);

			break;
		}

		/* Key presses */
	case KeyPress :
		{
			GFXKey key;
			if(event->xkey.keycode > GFX_X11_MAX_KEYCODE) key = GFX_KEY_UNKNOWN;
			else key = _gfx_x11.keys[event->xkey.keycode];

			_gfx_event_key_press(
				window,
				key,
				_gfx_x11_get_key_state(event->xkey.state)
			);

			break;
		}

		/* Key releases */
	case KeyRelease :
		{
			GFXKey key;
			if(event->xkey.keycode > GFX_X11_MAX_KEYCODE) key = GFX_KEY_UNKNOWN;
			else key = _gfx_x11.keys[event->xkey.keycode];

			_gfx_event_key_release(
				window,
				key,
				_gfx_x11_get_key_state(event->xkey.state)
			);

			break;
		}

		/* Mouse movement */
	case MotionNotify :
		{
			_gfx_event_mouse_move(
				window,
				event->xmotion.x,
				event->xmotion.y,
				_gfx_x11_get_key_state(event->xmotion.state)
			);

			break;
		}

		/* Mouse enters a window */
	case EnterNotify :
		{
			_gfx_event_mouse_enter(
				window,
				event->xcrossing.x,
				event->xcrossing.y,
				_gfx_x11_get_key_state(event->xcrossing.state)
			);

			break;
		}

		/* Mouse leaves a window */
	case LeaveNotify :
		{
			_gfx_event_mouse_leave(
				window,
				event->xcrossing.x,
				event->xcrossing.y,
				_gfx_x11_get_key_state(event->xcrossing.state)
			);

			break;
		}

		/* Mouse key presses */
	case ButtonPress :
		{
			GFXKeyState state = _gfx_x11_get_key_state(event->xbutton.state);
			int x = event->xbutton.x;
			int y = event->xbutton.y;

			switch(event->xbutton.button)
			{
			case Button1 :
				_gfx_event_mouse_press(window, GFX_MOUSE_KEY_LEFT, x, y, state);
				break;
			case Button2 :
				_gfx_event_mouse_press(window, GFX_MOUSE_KEY_MIDDLE, x, y, state);
				break;
			case Button3 :
				_gfx_event_mouse_press(window, GFX_MOUSE_KEY_RIGHT, x, y, state);
				break;

			case Button4 :
				_gfx_event_mouse_wheel(window, 0, 1, x, y, state);
				break;
			case Button5 :
				_gfx_event_mouse_wheel(window, 0, -1, x, y, state);
				break;
			case Button6 :
				_gfx_event_mouse_wheel(window, -1, 0, x, y, state);
				break;
			case Button7 :
				_gfx_event_mouse_wheel(window, 1, 0, x, y, state);
				break;
			}
		}

		/* Mouse key releases */
	case ButtonRelease :
		{
			GFXKeyState state = _gfx_x11_get_key_state(event->xbutton.state);
			int x = event->xbutton.x;
			int y = event->xbutton.y;

			switch(event->xbutton.button)
			{
			case Button1 :
				_gfx_event_mouse_release(window, GFX_MOUSE_KEY_LEFT, x, y, state);
				break;
			case Button2 :
				_gfx_event_mouse_release(window, GFX_MOUSE_KEY_MIDDLE, x, y, state);
				break;
			case Button3 :
				_gfx_event_mouse_release(window, GFX_MOUSE_KEY_RIGHT, x, y, state);
				break;
			}
		}
	}
}

/******************************************************/
GFX_X11_Window* _gfx_x11_get_window_from_handle(

		Window handle)
{
	GFX_X11_Window* it;
	for(
		it = _gfx_x11.windows.begin;
		it != _gfx_x11.windows.end;
		it = gfx_vector_next(&_gfx_x11.windows, it))
	{
		if(it->handle == handle) break;
	}

	return it != _gfx_x11.windows.end ? it : NULL;
}

/******************************************************/
GFX_PlatformWindow _gfx_platform_window_create(

		const GFX_PlatformAttributes* attributes)
{
	/* Setup the x11 window */
	GFX_X11_Window window;
	window.monitor = attributes->monitor;
	window.flags = 0;

	window.flags |=
		attributes->flags & GFX_WINDOW_RESIZABLE ?
		GFX_X11_RESIZABLE : 0;
	window.flags |=
		attributes->flags & GFX_WINDOW_HIDDEN ?
		GFX_X11_HIDDEN : 0;

	/* Get display mode & position */
	GFXDisplayMode mode;
	int x = window.monitor->x;
	int y = window.monitor->y;

	if(attributes->flags & GFX_WINDOW_FULLSCREEN)
	{
		window.flags |= GFX_X11_FULLSCREEN;

		GFX_X11_Mode* it = gfx_vector_at(
			&_gfx_x11.modes,
			window.monitor->modes[attributes->mode]
		);

		window.mode = it->id;
		mode = it->mode;
	}
	else
	{
		mode.width  = attributes->w;
		mode.height = attributes->h;
		mode.depth  = attributes->depth;

		x += attributes->x;
		y += attributes->y;
	}

	/* Get visual */
	XVisualInfo* visual = NULL;

#if defined(GFX_RENDERER_GL)

	/* Get FB Config */
	GLXFBConfig* config = _gfx_x11_get_config(
		window.monitor->screen,
		mode.depth,
		attributes->flags & GFX_WINDOW_DOUBLE_BUFFER
	);

	if(!config) return NULL;

	/* Get visual from config */
	visual = glXGetVisualFromFBConfig(
		_gfx_x11.display,
		*config
	);

	window.config = *config;
	XFree(config);

#endif

	/* Create the window attributes */
	unsigned long mask = CWColormap | CWEventMask;
	XSetWindowAttributes attr;

	if(
		attributes->flags & GFX_WINDOW_BORDERLESS ||
		attributes->flags & GFX_WINDOW_FULLSCREEN)
	{
		/* Borderless window */
		mask |= CWBorderPixel;
		attr.border_pixel = 0;
	}

	/* Event mask & Color map */
	Window root =
		XRootWindowOfScreen(window.monitor->screen);

	attr.event_mask =
		KeyPressMask |
		KeyReleaseMask |
		PointerMotionMask |
		EnterWindowMask |
		LeaveWindowMask |
		ButtonPressMask |
		ButtonReleaseMask |
		StructureNotifyMask |
		FocusChangeMask;

	attr.colormap = XCreateColormap(
		_gfx_x11.display,
		root,
		visual->visual,
		AllocNone
	);

	/* Create the actual window */
	window.handle = XCreateWindow(
		_gfx_x11.display,
		root,
		x,
		y,
		mode.width,
		mode.height,
		0,
		visual->depth,
		InputOutput,
		visual->visual,
		mask,
		&attr
	);

	XFree(visual);

	if(window.handle)
	{
		/* Get properties to check for events */
		XWindowAttributes get;
		get.x      = 0;
		get.y      = 0;
		get.width  = 0;
		get.height = 0;
		XGetWindowAttributes(_gfx_x11.display, window.handle, &get);

		window.x      = get.x;
		window.y      = get.y;
		window.width  = get.width;
		window.height = get.height;

		/* Delete protocol & name */
		XSetWMProtocols(
			_gfx_x11.display,
			window.handle,
			&_gfx_x11.WM_DELETE_WINDOW,
			1);

		XStoreName(
			_gfx_x11.display,
			window.handle,
			attributes->name);

		/* Disable decorations */
		if(mask & CWBorderPixel)
		{
			unsigned long hints[5];
			hints[0] = MWM_HINTS_DECORATIONS;
			hints[2] = 0;

			XChangeProperty(
				_gfx_x11.display,
				window.handle,
				_gfx_x11.MOTIF_WM_HINTS,
				_gfx_x11.MOTIF_WM_HINTS,
				32,
				PropModeReplace,
				(unsigned char*)hints,
				5);
		}

		/* Bypass compositor */
		if(attributes->flags & GFX_WINDOW_FULLSCREEN)
		{
			unsigned long bypass = 1;

			XChangeProperty(
				_gfx_x11.display,
				window.handle,
				_gfx_x11.NET_WM_BYPASS_COMPOSITOR,
				XA_CARDINAL,
				32,
				PropModeReplace,
				(unsigned char*)&bypass,
				1
			);
		}

		/* Set size hints */
		if(!(attributes->flags & GFX_WINDOW_RESIZABLE))
		{
			XSizeHints* hints = XAllocSizeHints();
			hints->flags = PMinSize | PMaxSize;

			hints->min_width = mode.width;
			hints->max_width = mode.width;
			hints->min_height = mode.height;
			hints->max_height = mode.height;

			XSetWMNormalHints(_gfx_x11.display, window.handle, hints);

			XFree(hints);
		}

		/* Add window to vector */
		GFXVectorIterator it = gfx_vector_insert(
			&_gfx_x11.windows,
			&window,
			_gfx_x11.windows.end
		);

		if(it != _gfx_x11.windows.end)
		{
			/* Make it visible */
			/* Triggers FocusIn event for fullscreen */
			if(!(attributes->flags & GFX_WINDOW_HIDDEN))
				XMapWindow(_gfx_x11.display, window.handle);

			return GFX_UINT_TO_VOID(window.handle);
		}

		XDestroyWindow(_gfx_x11.display, window.handle);
	}

	XFreeColormap(_gfx_x11.display, attr.colormap);

	return NULL;
}

/******************************************************/
void _gfx_platform_window_free(

		GFX_PlatformWindow handle)
{
	GFX_X11_Window* it =
		_gfx_x11_get_window_from_handle(GFX_VOID_TO_UINT(handle));

	/* Get attributes */
	XWindowAttributes attr;
	XGetWindowAttributes(_gfx_x11.display, GFX_VOID_TO_UINT(handle), &attr);

	/* Make sure to undo fullscreen */
	if(it->flags & GFX_X11_FULLSCREEN)
		_gfx_x11_leave_fullscreen(it->monitor);

	/* Destroy context, the window and its colormap */
	_gfx_platform_context_clear(handle);
	XDestroyWindow(_gfx_x11.display, GFX_VOID_TO_UINT(handle));
	XFreeColormap(_gfx_x11.display, attr.colormap);

	/* Remove from vector */
	gfx_vector_erase(&_gfx_x11.windows, it);
}

/******************************************************/
GFX_PlatformMonitor _gfx_platform_window_get_monitor(

		GFX_PlatformWindow handle)
{
	GFX_X11_Window* internal =
		_gfx_x11_get_window_from_handle(GFX_VOID_TO_UINT(handle));

	if(internal) return internal->monitor;

	return NULL;
}

/******************************************************/
char* _gfx_platform_window_get_name(

		GFX_PlatformWindow handle)
{
	/* Check if it has a name */
	char* buff;
	XFetchName(_gfx_x11.display, GFX_VOID_TO_UINT(handle), &buff);
	if(!buff) return NULL;

	/* Copy to client side memory */
	char* name = malloc(strlen(buff) + 1);
	strcpy(name, buff);

	XFree(buff);

	return name;
}

/******************************************************/
void _gfx_platform_window_get_size(

		GFX_PlatformWindow  handle,
		unsigned int*       width,
		unsigned int*       height)
{
	GFX_X11_Window* internal =
		_gfx_x11_get_window_from_handle(GFX_VOID_TO_UINT(handle));

	if(!internal)
	{
		*width = 0;
		*height = 0;
	}
	else
	{
		*width = internal->width;
		*height = internal->height;
	}
}

/******************************************************/
void _gfx_platform_window_get_position(

		GFX_PlatformWindow  handle,
		int*                x,
		int*                y)
{
	GFX_X11_Window* internal =
		_gfx_x11_get_window_from_handle(GFX_VOID_TO_UINT(handle));

	if(!internal)
	{
		*x = 0;
		*y = 0;
	}
	else
	{
		*x = internal->x;
		*y = internal->y;

		/* Get window's monitor position */
		if(internal->monitor)
		{
			*x -= internal->monitor->x;
			*y -= internal->monitor->y;
		}
	}
}

/******************************************************/
void _gfx_platform_window_set_name(

		GFX_PlatformWindow  handle,
		const char*         name)
{
	XStoreName(_gfx_x11.display, GFX_VOID_TO_UINT(handle), name);
}

/******************************************************/
void _gfx_platform_window_set_size(

		GFX_PlatformWindow  handle,
		unsigned int        width,
		unsigned int        height)
{
	GFX_X11_Window* internal =
		_gfx_x11_get_window_from_handle(GFX_VOID_TO_UINT(handle));

	if(internal && (internal->flags & GFX_X11_RESIZABLE))
		XResizeWindow(
			_gfx_x11.display,
			GFX_VOID_TO_UINT(handle),
			width,
			height
		);
}

/******************************************************/
void _gfx_platform_window_set_position(

		GFX_PlatformWindow  handle,
		int                 x,
		int                 y)
{
	/* Check if fullscreen */
	GFX_X11_Window* internal =
		_gfx_x11_get_window_from_handle(GFX_VOID_TO_UINT(handle));

	if(internal && !(internal->flags & GFX_X11_FULLSCREEN))
	{
		/* Get window's monitor position */
		int xM = 0;
		int yM = 0;

		if(internal->monitor)
		{
			xM = internal->monitor->x;
			yM = internal->monitor->y;
		}

		XMoveWindow(
			_gfx_x11.display,
			GFX_VOID_TO_UINT(handle),
			x + xM,
			y + yM
		);
	}
}

/******************************************************/
void _gfx_platform_window_show(

		GFX_PlatformWindow handle)
{
	GFX_X11_Window* internal =
		_gfx_x11_get_window_from_handle(GFX_VOID_TO_UINT(handle));

	if(internal)
	{
		internal->flags &= ~GFX_X11_HIDDEN;
		XMapWindow(_gfx_x11.display, GFX_VOID_TO_UINT(handle));
	}
}

/******************************************************/
void _gfx_platform_window_hide(

		GFX_PlatformWindow handle)
{
	GFX_X11_Window* internal =
		_gfx_x11_get_window_from_handle(GFX_VOID_TO_UINT(handle));

	if(internal)
	{
		internal->flags |= GFX_X11_HIDDEN;
		XUnmapWindow(_gfx_x11.display, GFX_VOID_TO_UINT(handle));
	}
}

/******************************************************/
int _gfx_platform_poll_events(void)
{
	XEvent event;
	int cnt = XPending(_gfx_x11.display);

	while(cnt--)
	{
		XNextEvent(_gfx_x11.display, &event);
		_gfx_x11_event_proc(&event);
	}

	return 1;
}
