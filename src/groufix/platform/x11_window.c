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

#include "groufix/platform/x11.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
static GLXFBConfig* _gfx_x11_get_config(Screen* screen, const GFXColorDepth* depth)
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
static void _gfx_x11_set_atoms(Window window)
{
	/* Create atom array */
	Atom atoms[] = {
		_gfx_x11->wmDeleteWindow
	};

	/* Set atom protocols */
	XSetWMProtocols(_gfx_x11->display, window, atoms, 1);
}

/******************************************************/
static GFXKeyState _gfx_x11_get_key_state(unsigned int state)
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
static void _gfx_x11_event_proc(XEvent* event)
{
	/* Get window */
	GFX_Platform_Window window = UINT_TO_VOID(event->xany.window);

	switch(event->type)
	{
		/* Protocol messages */
		case ClientMessage :
		{
			if(event->xclient.data.l[0] == _gfx_x11->wmDeleteWindow)
				_gfx_event_window_close(window);

			break;
		}

		/* Key press */
		case KeyPress :
		{
			GFXKey key;
			if(event->xkey.keycode > GFX_X11_MAX_KEYCODE) key = GFX_KEY_UNKNOWN;
			else key = _gfx_x11->keys[event->xkey.keycode];

			_gfx_event_key_press(window, key, _gfx_x11_get_key_state(event->xkey.state));

			break;
		}

		/* Key release */
		case KeyRelease :
		{
			GFXKey key;
			if(event->xkey.keycode > GFX_X11_MAX_KEYCODE) key = GFX_KEY_UNKNOWN;
			else key = _gfx_x11->keys[event->xkey.keycode];

			_gfx_event_key_release(window, key, _gfx_x11_get_key_state(event->xkey.state));

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
GFX_Platform_Window _gfx_platform_create_window(const GFX_Platform_Attributes* attributes)
{
	/* Get FB Config */
	GLXFBConfig* config = _gfx_x11_get_config(attributes->screen, &attributes->depth);
	if(!config) return NULL;

	/* Setup the x11 window */
	GFX_X11_Window window;
	window.config = *config;
	window.context = NULL;

	/* Get visual from config */
	XVisualInfo* visual = glXGetVisualFromFBConfig(_gfx_x11->display, *config);
	XFree(config);

	/* Create the window attributes */
	XSetWindowAttributes attr;
	attr.event_mask =
		KeyPressMask |
		KeyReleaseMask |
		PointerMotionMask |
		ButtonPressMask |
		ButtonReleaseMask;

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
		CWColormap | CWEventMask,
		&attr
	);

	XFree(visual);

	/* Add window to vector */
	if(!vector_insert(_gfx_x11->windows, &window, _gfx_x11->windows->end))
	{
		XDestroyWindow(_gfx_x11->display, window.handle);
		XFreeColormap(_gfx_x11->display, attr.colormap);

		return NULL;
	}

	/* Set protocols */
	_gfx_x11_set_atoms(window.handle);
	XStoreName(_gfx_x11->display, window.handle, attributes->name);

	return UINT_TO_VOID(window.handle);
}

/******************************************************/
void _gfx_platform_destroy_window(GFX_Platform_Window handle)
{
	if(_gfx_x11)
	{
		/* Destroy the context */
		_gfx_platform_destroy_context(handle);

		/* Destroy the window and its colormap */
		XWindowAttributes attr;
		XGetWindowAttributes(_gfx_x11->display, VOID_TO_UINT(handle), &attr);

		XDestroyWindow(_gfx_x11->display, VOID_TO_UINT(handle));
		XFreeColormap(_gfx_x11->display, attr.colormap);

		/* Remove from vector */
		VectorIterator it = _gfx_x11_get_window_from_handle(VOID_TO_UINT(handle));
		vector_erase(_gfx_x11->windows, it);
	}
}

/******************************************************/
GFX_Platform_Screen _gfx_platform_window_get_screen(GFX_Platform_Window handle)
{
	if(!_gfx_x11) return NULL;

	XWindowAttributes attr;
	attr.screen = NULL;
	XGetWindowAttributes(_gfx_x11->display, VOID_TO_UINT(handle), &attr);

	return attr.screen;
}

/******************************************************/
char* _gfx_platform_window_get_name(GFX_Platform_Window handle)
{
	if(!_gfx_x11) return NULL;

	/* Check if it has a name */
	char* buff;
	XFetchName(_gfx_x11->display, VOID_TO_UINT(handle), &buff);
	if(!buff) return NULL;

	/* Copy to client side memory */
	char* name = (char*)malloc(sizeof(buff));
	strcpy(name, buff);

	XFree(buff);

	return name;
}

/******************************************************/
void _gfx_platform_window_get_size(GFX_Platform_Window handle, unsigned int* width, unsigned int* height)
{
	if(!_gfx_x11)
	{
		*width = 0;
		*height = 0;
	}
	else
	{
		XWindowAttributes attr;
		attr.width = 0;
		attr.height = 0;
		XGetWindowAttributes(_gfx_x11->display, VOID_TO_UINT(handle), &attr);

		*width = attr.width;
		*height = attr.height;
	}
}

/******************************************************/
void _gfx_platform_window_get_position(GFX_Platform_Window handle, int* x, int* y)
{
	if(!_gfx_x11)
	{
		*x = 0;
		*y = 0;
	}
	else
	{
		XWindowAttributes attr;
		attr.x = 0;
		attr.y = 0;
		XGetWindowAttributes(_gfx_x11->display, VOID_TO_UINT(handle), &attr);

		*x = attr.x;
		*y = attr.y;
	}
}

/******************************************************/
void _gfx_platform_window_set_name(GFX_Platform_Window handle, const char* name)
{
	if(_gfx_x11) XStoreName(_gfx_x11->display, VOID_TO_UINT(handle), name);
}

/******************************************************/
void _gfx_platform_window_set_size(GFX_Platform_Window handle, unsigned int width, unsigned int height)
{
	if(_gfx_x11) XResizeWindow(_gfx_x11->display, VOID_TO_UINT(handle), width, height);
}

/******************************************************/
void _gfx_platform_window_set_position(GFX_Platform_Window handle, int x, int y)
{
	if(_gfx_x11) XMoveWindow(_gfx_x11->display, VOID_TO_UINT(handle), x, y);
}

/******************************************************/
void _gfx_platform_window_show(GFX_Platform_Window handle)
{
	if(_gfx_x11) XMapWindow(_gfx_x11->display, VOID_TO_UINT(handle));
}

/******************************************************/
void _gfx_platform_window_hide(GFX_Platform_Window handle)
{
	if(_gfx_x11) XUnmapWindow(_gfx_x11->display, VOID_TO_UINT(handle));
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
