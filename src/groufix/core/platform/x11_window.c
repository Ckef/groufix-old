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

#include "groufix/core/platform/x11.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
static GLXFBConfig* _gfx_x11_get_config(

		Screen*               screen,
		const GFXColorDepth*  depth)
{
	if(!_gfx_x11) return NULL;

	/* Create buffer attribute array */
	int bufferAttr[] = {
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE,   GLX_RGBA_BIT,
		GLX_DOUBLEBUFFER,  True,
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
static void _gfx_x11_set_fullscreen(

		Window  handle,
		Window  root)
{
	/* Create event to set full screen atom */
	XEvent event;
	event.xclient.type         = ClientMessage;
	event.xclient.serial       = 0;
	event.xclient.send_event   = True;
	event.xclient.display      = _gfx_x11->display;
	event.xclient.window       = handle;
	event.xclient.message_type = _gfx_x11->wmState;
	event.xclient.format       = 0x20;

	event.xclient.data.l[0] = 1;
	event.xclient.data.l[1] = _gfx_x11->wmStateFullscreen;
	event.xclient.data.l[2] = 0;

	/* Send it */
	XSendEvent(
		_gfx_x11->display,
		root,
		False,
		SubstructureRedirectMask | SubstructureNotifyMask,
		&event
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
			if(event->xclient.data.l[0] == _gfx_x11->wmDeleteWindow)
				_gfx_event_window_close(window);

			break;
		}

		/* Resize & Move */
		case ConfigureNotify :
		{
			GFX_X11_Window* internal =
				_gfx_x11_get_window_from_handle(event->xany.window);

			if(
				internal->x != event->xconfigure.x ||
				internal->y != event->xconfigure.y)
			{
				internal->x = event->xconfigure.x;
				internal->y = event->xconfigure.y;
				_gfx_event_window_move(window, internal->x, internal->y);
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

			if(event->xbutton.button == Button1)
				_gfx_event_mouse_press(window, GFX_MOUSE_KEY_LEFT, x, y, state);
			else if(event->xbutton.button == Button2)
				_gfx_event_mouse_press(window, GFX_MOUSE_KEY_MIDDLE, x, y, state);
			else if(event->xbutton.button == Button3)
				_gfx_event_mouse_press(window, GFX_MOUSE_KEY_RIGHT, x, y, state);

			else if(event->xbutton.button == Button4)
				_gfx_event_mouse_wheel(window, 0, 1, x, y, state);
			else if(event->xbutton.button == Button5)
				_gfx_event_mouse_wheel(window, 0, -1, x, y, state);
			else if(event->xbutton.button == Button6)
				_gfx_event_mouse_wheel(window, -1, 0, x, y, state);
			else if(event->xbutton.button == Button7)
				_gfx_event_mouse_wheel(window, 1, 0, x, y, state);

			break;
		}

		/* Mouse key release */
		case ButtonRelease :
		{
			GFXKeyState state = _gfx_x11_get_key_state(event->xbutton.state);
			int x = event->xbutton.x;
			int y = event->xbutton.y;

			if(event->xbutton.button == Button1)
				_gfx_event_mouse_release(window, GFX_MOUSE_KEY_LEFT, x, y, state);
			else if(event->xbutton.button == Button2)
				_gfx_event_mouse_release(window, GFX_MOUSE_KEY_MIDDLE, x, y, state);
			else if(event->xbutton.button == Button3)
				_gfx_event_mouse_release(window, GFX_MOUSE_KEY_RIGHT, x, y, state);

			break;
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
		&attributes->depth
	);

	if(!config) return NULL;

	/* Setup the x11 window */
	GFX_X11_Window window;
	window.config = *config;
	window.context = NULL;

	/* Get visual from config */
	XVisualInfo* visual = glXGetVisualFromFBConfig(
		_gfx_x11->display,
		*config
	);
	XFree(config);

	/* Create the window attributes */
	unsigned long mask = 0;
	XSetWindowAttributes attr;

	if(attributes->flags & GFX_WINDOW_BORDERLESS)
	{
		/* Borderless window */
		mask = CWOverrideRedirect | CWBorderPixel;

		attr.override_redirect = True;
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
		StructureNotifyMask;

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
		attributes->width,
		attributes->height,
		0,
		visual->depth,
		InputOutput,
		visual->visual,
		mask | CWColormap | CWEventMask,
		&attr
	);

	XFree(visual);

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

	/* Add window to vector */
	GFXVectorIterator it = gfx_vector_insert(
		&_gfx_x11->windows,
		&window,
		_gfx_x11->windows.end
	);

	if(it == _gfx_x11->windows.end)
	{
		XDestroyWindow(_gfx_x11->display, window.handle);
		XFreeColormap(_gfx_x11->display, attr.colormap);

		return NULL;
	}

	/* Set full screen */
	if(attributes->flags & GFX_WINDOW_FULLSCREEN)
	{
		_gfx_x11_set_fullscreen(window.handle, get.root);
		XMapWindow(_gfx_x11->display, window.handle);
	}

	/* Set size hints */
	else if(!(GFX_WINDOW_RESIZABLE & attributes->flags))
	{
		XSizeHints* hints = XAllocSizeHints();
		hints->flags = PMinSize | PMaxSize;

		hints->min_width = attributes->width;
		hints->max_width = attributes->width;
		hints->min_height = attributes->height;
		hints->max_height = attributes->height;

		XSetWMNormalHints(_gfx_x11->display, window.handle, hints);

		XFree(hints);
	}

	/* Make it visible */
	if(!(attributes->flags & GFX_WINDOW_HIDDEN))
		XMapWindow(_gfx_x11->display, window.handle);

	/* Set protocols */
	XSetWMProtocols(
		_gfx_x11->display,
		window.handle,
		&_gfx_x11->wmDeleteWindow,
		1
	);

	XStoreName(
		_gfx_x11->display,
		window.handle,
		attributes->name
	);

	return GFX_UINT_TO_VOID(window.handle);
}

/******************************************************/
void _gfx_platform_window_free(

		GFX_PlatformWindow handle)
{
	if(_gfx_x11)
	{
		/* Destroy the context */
		_gfx_platform_context_free(handle);

		/* Remove all events of the window from the queue */
		XWindowAttributes attr;
		XGetWindowAttributes(
			_gfx_x11->display,
			GFX_VOID_TO_UINT(handle),
			&attr
		);

		XEvent event;
		while(XCheckWindowEvent(
			_gfx_x11->display,
			GFX_VOID_TO_UINT(handle),
			attr.all_event_masks, &event)
		);

		/* Destroy the window and its colormap */
		XDestroyWindow(_gfx_x11->display, GFX_VOID_TO_UINT(handle));
		XFreeColormap(_gfx_x11->display, attr.colormap);

		/* Remove from vector */
		GFXVectorIterator it =
			_gfx_x11_get_window_from_handle(GFX_VOID_TO_UINT(handle));

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
	if(_gfx_x11) XMapWindow(
		_gfx_x11->display,
		GFX_VOID_TO_UINT(handle)
	);
}

/******************************************************/
void _gfx_platform_window_hide(

		GFX_PlatformWindow handle)
{
	if(_gfx_x11) XUnmapWindow(
		_gfx_x11->display,
		GFX_VOID_TO_UINT(handle)
	);
}

/******************************************************/
void _gfx_platform_poll_events(void)
{
	if(_gfx_x11)
	{
		XEvent event;

		int cnt = XPending(_gfx_x11->display);
		while(cnt--)
		{
			XNextEvent(_gfx_x11->display, &event);
			_gfx_x11_event_proc(&event);
		}
	}
}
