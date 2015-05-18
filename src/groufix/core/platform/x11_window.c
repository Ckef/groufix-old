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

#include "groufix/core/platform/x11.h"
#include "groufix/core/internal.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Motif hints */
typedef struct
{
	unsigned long  flags;
	unsigned long  functions;
	unsigned long  decorations;
	long           input_mode;
	unsigned long  status;

} MotifWmHints;


/******************************************************/
static void _gfx_x11_enter_fullscreen(

		Window  handle,
		Window  root)
{
	/* Disable screen saver */
	if(!(_gfx_x11->saverCount++))
	{
		XGetScreenSaver(
			_gfx_x11->display,
			&_gfx_x11->saverTimeout,
			&_gfx_x11->saverInterval,
			&_gfx_x11->saverBlank,
			&_gfx_x11->saverExposure);

		XSetScreenSaver(
			_gfx_x11->display,
			0,
			0,
			DontPreferBlanking,
			DefaultExposures);
	}

	/* Bypass compositor */
	unsigned long bypass = 1;

	XChangeProperty(
		_gfx_x11->display,
		handle,
		_gfx_x11->NET_WM_BYPASS_COMPOSITOR,
		XA_CARDINAL,
		32,
		PropModeReplace,
		(unsigned char*)&bypass,
		1
	);

	/* Create event to raise the focus of the window */
	XEvent event;
	memset(&event, 0, sizeof(XEvent));

	event.type                 = ClientMessage;
	event.xclient.window       = handle;
	event.xclient.serial       = 0;
	event.xclient.send_event   = True;
	event.xclient.format       = 32;
	event.xclient.message_type = _gfx_x11->NET_ACTIVE_WINDOW;
	event.xclient.data.l[0]    = 1;
	event.xclient.data.l[1]    = 0;

	XSendEvent(
		_gfx_x11->display,
		root,
		False,
		SubstructureRedirectMask | SubstructureNotifyMask,
		&event
	);

	/* Create event to add full screen atom */
	event.xclient.message_type = _gfx_x11->NET_WM_STATE;
	event.xclient.data.l[0]    = 1;
	event.xclient.data.l[1]    = _gfx_x11->NET_WM_STATE_FULLSCREEN;
	event.xclient.data.l[2]    = 0;
	event.xclient.data.l[3]    = 1;

	XSendEvent(
		_gfx_x11->display,
		root,
		False,
		SubstructureRedirectMask | SubstructureNotifyMask,
		&event
	);
}

/******************************************************/
static void _gfx_x11_leave_fullscreen(

		Window  handle,
		Window  root)
{
	/* Restore screen saver */
	if(!(--_gfx_x11->saverCount)) XSetScreenSaver(
		_gfx_x11->display,
		_gfx_x11->saverTimeout,
		_gfx_x11->saverInterval,
		_gfx_x11->saverBlank,
		_gfx_x11->saverExposure
	);

	/* Stop bypassing the compositor */
	unsigned long bypass = 0;

	XChangeProperty(
		_gfx_x11->display,
		handle,
		_gfx_x11->NET_WM_BYPASS_COMPOSITOR,
		XA_CARDINAL,
		32,
		PropModeReplace,
		(unsigned char*)&bypass,
		1
	);

	/* Create event to remove full screen atom */
	XEvent event;
	event.type                 = ClientMessage;
	event.xclient.window       = handle;
	event.xclient.format       = 32;
	event.xclient.message_type = _gfx_x11->NET_WM_STATE;
	event.xclient.data.l[0]    = 0;
	event.xclient.data.l[1]    = _gfx_x11->NET_WM_STATE_FULLSCREEN;
	event.xclient.data.l[2]    = 0;
	event.xclient.data.l[3]    = 1;

	XSendEvent(
		_gfx_x11->display,
		root,
		False,
		SubstructureRedirectMask | SubstructureNotifyMask,
		&event
	);
}

/******************************************************/
static GLXFBConfig* _gfx_x11_get_config(

		Screen*               screen,
		const GFXColorDepth*  depth,
		int                   backBuffer)
{
	if(!_gfx_x11) return NULL;

	/* Create buffer attribute array */
	int bufferAttr[] = {
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE,   GLX_RGBA_BIT,
		GLX_DOUBLEBUFFER,  backBuffer ? True : False,
		GLX_RED_SIZE,      depth->redBits,
		GLX_GREEN_SIZE,    depth->greenBits,
		GLX_BLUE_SIZE,     depth->blueBits,
		None
	};

	/* Get config from screen */
	int buffElements;
	return glXChooseFBConfig(
		_gfx_x11->display,
		XScreenNumberOfScreen(screen),
		bufferAttr,
		&buffElements
	);
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
			if(event->xclient.data.l[0] == _gfx_x11->WM_DELETE_WINDOW)
				_gfx_event_window_close(window);

			break;
		}

		/* Resize & Move */
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

				_gfx_event_window_move(
					window,
					internal->x,
					internal->y
				);
			}

			if(
				internal->width != event->xconfigure.width ||
				internal->height != event->xconfigure.height)
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

		/* Focus */
		case FocusIn :
		{
			_gfx_event_window_focus(window);
			break;
		}

		/* Blur */
		case FocusOut :
		{
			_gfx_event_window_blur(window);
			break;
		}

		/* Key press */
		case KeyPress :
		{
			GFXKey key;
			if(event->xkey.keycode > GFX_X11_MAX_KEYCODE) key = GFX_KEY_UNKNOWN;
			else key = _gfx_x11->keys[event->xkey.keycode];

			_gfx_event_key_press(
				window,
				key,
				_gfx_x11_get_key_state(event->xkey.state)
			);

			break;
		}

		/* Key release */
		case KeyRelease :
		{
			GFXKey key;
			if(event->xkey.keycode > GFX_X11_MAX_KEYCODE) key = GFX_KEY_UNKNOWN;
			else key = _gfx_x11->keys[event->xkey.keycode];

			_gfx_event_key_release(
				window,
				key,
				_gfx_x11_get_key_state(event->xkey.state)
			);

			break;
		}

		/* Pointer motion */
		case MotionNotify :
		{
			_gfx_event_mouse_move(window,
				event->xmotion.x,
				event->xmotion.y,
				_gfx_x11_get_key_state(event->xmotion.state)
			);

			break;
		}

		/* Pointer enter */
		case EnterNotify :
		{
			_gfx_event_mouse_enter(window,
				event->xcrossing.x,
				event->xcrossing.y,
				_gfx_x11_get_key_state(event->xcrossing.state)
			);

			break;
		}

		/* Pointer leave */
		case LeaveNotify :
		{
			_gfx_event_mouse_leave(window,
				event->xcrossing.x,
				event->xcrossing.y,
				_gfx_x11_get_key_state(event->xcrossing.state)
			);

			break;
		}

		/* Mouse key press */
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

		/* Mouse key release */
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
GFX_PlatformWindow _gfx_platform_window_create(

		const GFX_PlatformAttributes* attributes)
{
	/* Get FB Config */
	GLXFBConfig* config = _gfx_x11_get_config(
		attributes->screen,
		&attributes->mode.depth,
		attributes->flags & GFX_WINDOW_DOUBLE_BUFFER
	);

	if(!config) return NULL;

	/* Setup the x11 window */
	GFX_X11_Window window;
	window.config  = *config;
	window.context = NULL;
	window.flags   = 0;

	/* Get visual from config */
	XVisualInfo* visual = glXGetVisualFromFBConfig(
		_gfx_x11->display,
		*config
	);
	XFree(config);

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
		_gfx_x11->display,
		RootWindowOfScreen((Screen*)attributes->screen),
		visual->visual,
		AllocNone
	);

	/* Create the actual window */
	window.handle = XCreateWindow(
		_gfx_x11->display,
		RootWindowOfScreen((Screen*)attributes->screen),
		attributes->x,
		attributes->y,
		attributes->mode.width,
		attributes->mode.height,
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
		XGetWindowAttributes(_gfx_x11->display, window.handle, &get);

		window.x      = get.x;
		window.y      = get.y;
		window.width  = get.width;
		window.height = get.height;

		/* Disable decorations */
		if(mask & CWBorderPixel)
		{
			MotifWmHints hints;
			hints.flags = MWM_HINTS_DECORATIONS;
			hints.decorations = 0;

			XChangeProperty(
				_gfx_x11->display,
				window.handle,
				_gfx_x11->MOTIF_WM_HINTS,
				_gfx_x11->MOTIF_WM_HINTS,
				32,
				PropModeReplace,
				(unsigned char*)&hints,
				sizeof(MotifWmHints) / sizeof(long)
			);
		}

		/* Set protocols */
		XSetWMProtocols(
			_gfx_x11->display,
			window.handle,
			&_gfx_x11->WM_DELETE_WINDOW,
			1
		);

		XStoreName(
			_gfx_x11->display,
			window.handle,
			attributes->name
		);

		/* Set full screen */
		if(attributes->flags & GFX_WINDOW_FULLSCREEN)
		{
			window.flags |= GFX_X11_FULLSCREEN;
			XMapWindow(_gfx_x11->display, window.handle);

			_gfx_x11_enter_fullscreen(window.handle, get.root);
		}

		else
		{
			/* Set size hints */
			if(!(GFX_WINDOW_RESIZABLE & attributes->flags))
			{
				XSizeHints* hints = XAllocSizeHints();
				hints->flags = PMinSize | PMaxSize;

				hints->min_width = attributes->mode.width;
				hints->max_width = attributes->mode.width;
				hints->min_height = attributes->mode.height;
				hints->max_height = attributes->mode.height;

				XSetWMNormalHints(_gfx_x11->display, window.handle, hints);

				XFree(hints);
			}

			/* Make it visible */
			if(!(attributes->flags & GFX_WINDOW_HIDDEN))
				XMapWindow(_gfx_x11->display, window.handle);
		}

		/* Add window to vector */
		GFXVectorIterator it = gfx_vector_insert(
			&_gfx_x11->windows,
			&window,
			_gfx_x11->windows.end
		);

		if(it != _gfx_x11->windows.end)
			return GFX_UINT_TO_VOID(window.handle);

		/* Undo fullscreen */
		if(window.flags & GFX_X11_FULLSCREEN)
			_gfx_x11_leave_fullscreen(window.handle, get.root);

		XDestroyWindow(_gfx_x11->display, window.handle);
	}

	XFreeColormap(_gfx_x11->display, attr.colormap);

	return NULL;
}

/******************************************************/
void _gfx_platform_window_free(

		GFX_PlatformWindow handle)
{
	if(_gfx_x11)
	{
		GFX_X11_Window* it =
			_gfx_x11_get_window_from_handle(GFX_VOID_TO_UINT(handle));

		/* Get attributes */
		XWindowAttributes attr;
		XGetWindowAttributes(
			_gfx_x11->display,
			GFX_VOID_TO_UINT(handle),
			&attr
		);

		/* Make sure to undo fullscreen */
		if(it->flags & GFX_X11_FULLSCREEN) _gfx_x11_leave_fullscreen(
			GFX_VOID_TO_UINT(handle),
			attr.root
		);

		/* Destroy context, the window and its colormap */
		_gfx_platform_context_clear(handle);
		XDestroyWindow(_gfx_x11->display, GFX_VOID_TO_UINT(handle));
		XFreeColormap(_gfx_x11->display, attr.colormap);

		/* Remove from vector */
		gfx_vector_erase(&_gfx_x11->windows, it);
	}
}

/******************************************************/
GFX_PlatformScreen _gfx_platform_window_get_screen(

		GFX_PlatformWindow handle)
{
	if(!_gfx_x11) return NULL;

	XWindowAttributes attr;
	attr.screen = NULL;
	XGetWindowAttributes(_gfx_x11->display, GFX_VOID_TO_UINT(handle), &attr);

	return attr.screen;
}

/******************************************************/
char* _gfx_platform_window_get_name(

		GFX_PlatformWindow handle)
{
	if(!_gfx_x11) return NULL;

	/* Check if it has a name */
	char* buff;
	XFetchName(_gfx_x11->display, GFX_VOID_TO_UINT(handle), &buff);
	if(!buff) return NULL;

	/* Copy to client side memory */
	char* name = malloc(sizeof(char) * (strlen(buff) + 1));
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
	}
}

/******************************************************/
void _gfx_platform_window_set_name(

		GFX_PlatformWindow  handle,
		const char*         name)
{
	if(_gfx_x11) XStoreName(
		_gfx_x11->display,
		GFX_VOID_TO_UINT(handle),
		name
	);
}

/******************************************************/
void _gfx_platform_window_set_size(

		GFX_PlatformWindow  handle,
		unsigned int        width,
		unsigned int        height)
{
	if(_gfx_x11) XResizeWindow(
		_gfx_x11->display,
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
	if(_gfx_x11) XMoveWindow(
		_gfx_x11->display,
		GFX_VOID_TO_UINT(handle),
		x,
		y
	);
}

/******************************************************/
void _gfx_platform_window_show(

		GFX_PlatformWindow handle)
{
	GFX_X11_Window* internal =
		_gfx_x11_get_window_from_handle(GFX_VOID_TO_UINT(handle));

	if(internal)
	{
		XMapWindow(
			_gfx_x11->display,
			GFX_VOID_TO_UINT(handle)
		);

		/* Also enter fullscreen */
		if(internal->flags & GFX_X11_FULLSCREEN)
		{
			XWindowAttributes attr;

			XGetWindowAttributes(
				_gfx_x11->display, GFX_VOID_TO_UINT(handle), &attr);
			_gfx_x11_enter_fullscreen(
				GFX_VOID_TO_UINT(handle), attr.root);
		}
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
		XUnmapWindow(
			_gfx_x11->display,
			GFX_VOID_TO_UINT(handle)
		);

		/* Also leave fullscreen */
		if(internal->flags & GFX_X11_FULLSCREEN)
		{
			XWindowAttributes attr;

			XGetWindowAttributes(
				_gfx_x11->display, GFX_VOID_TO_UINT(handle), &attr);
			_gfx_x11_leave_fullscreen(
				GFX_VOID_TO_UINT(handle), attr.root);
		}
	}
}

/******************************************************/
int _gfx_platform_poll_events(void)
{
	if(!_gfx_x11) return 0;

	XEvent event;
	int cnt = XPending(_gfx_x11->display);

	while(cnt--)
	{
		XNextEvent(_gfx_x11->display, &event);
		_gfx_x11_event_proc(&event);
	}

	return 1;
}
