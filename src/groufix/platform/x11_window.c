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
static XVisualInfo* _gfx_x11_get_visual(Screen* screen, unsigned short red, unsigned short blue, unsigned short green)
{
	if(!_gfx_x11) return NULL;

	/* Create buffer attribute array */
	int bufferAttr[] = {
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE,   GLX_RGBA_BIT,
		GLX_DOUBLEBUFFER,  True,
		GLX_RED_SIZE,      red,
		GLX_BLUE_SIZE,     blue,
		GLX_GREEN_SIZE,    green,
		None
	};

	/* Get visual info of screen */
	int buffElements;
	GLXFBConfig* config = glXChooseFBConfig(
		_gfx_x11->display,
		XScreenNumberOfScreen(screen),
		bufferAttr,
		&buffElements
	);

	if(!config) return NULL;
	XVisualInfo* visual = glXGetVisualFromFBConfig(_gfx_x11->display, *config);
	XFree(config);

	return visual;
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
				gfx_event_window_close(window);

			break;
		}

		/* Key press */
		case KeyPress :
		{
			GFXKey key;
			if(event->xkey.keycode > GFX_X11_MAX_KEYCODE) key = GFX_KEY_UNKNOWN;
			else key = _gfx_x11->keys[event->xkey.keycode];

			gfx_event_key_press(window, key, _gfx_x11_get_key_state(event->xkey.state));

			break;
		}

		/* Key release */
		case KeyRelease :
		{
			GFXKey key;
			if(event->xkey.keycode > GFX_X11_MAX_KEYCODE) key = GFX_KEY_UNKNOWN;
			else key = _gfx_x11->keys[event->xkey.keycode];

			gfx_event_key_release(window, key, _gfx_x11_get_key_state(event->xkey.state));

			break;
		}

		/* Pointer motion */
		case MotionNotify :
		{
			gfx_event_mouse_move(window,
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
				gfx_event_mouse_press(window, GFX_MOUSE_KEY_LEFT, x, y, state);
			else if(event->xbutton.button == Button2)
				gfx_event_mouse_press(window, GFX_MOUSE_KEY_MIDDLE, x, y, state);
			else if(event->xbutton.button == Button3)
				gfx_event_mouse_press(window, GFX_MOUSE_KEY_RIGHT, x, y, state);

			else if(event->xbutton.button == Button4)
				gfx_event_mouse_wheel(window, 0, 1, x, y, state);
			else if(event->xbutton.button == Button5)
				gfx_event_mouse_wheel(window, 0, -1, x, y, state);
			else if(event->xbutton.button == Button6)
				gfx_event_mouse_wheel(window, -1, 0, x, y, state);
			else if(event->xbutton.button == Button7)
				gfx_event_mouse_wheel(window, 1, 0, x, y, state);

			break;
		}

		/* Mouse key release */
		case ButtonRelease :
		{
			GFXKeyState state = _gfx_x11_get_key_state(event->xbutton.state);
			int x = event->xbutton.x;
			int y = event->xbutton.y;

			if(event->xbutton.button == Button1)
				gfx_event_mouse_release(window, GFX_MOUSE_KEY_LEFT, x, y, state);
			else if(event->xbutton.button == Button2)
				gfx_event_mouse_release(window, GFX_MOUSE_KEY_MIDDLE, x, y, state);
			else if(event->xbutton.button == Button3)
				gfx_event_mouse_release(window, GFX_MOUSE_KEY_RIGHT, x, y, state);

			break;
		}
	}
}

/******************************************************/
static void _gfx_x11_add_window(GFX_X11_Window window)
{
	unsigned int index = _gfx_x11->numWindows++;
	_gfx_x11->windows = (GFX_X11_Window*)realloc(
		_gfx_x11->windows,
		sizeof(GFX_X11_Window) * _gfx_x11->numWindows);

	_gfx_x11->windows[index] = window;
}

/******************************************************/
unsigned int _gfx_platform_get_num_windows(void)
{
	if(!_gfx_x11) return 0;
	return _gfx_x11->numWindows;
}

/******************************************************/
GFX_Platform_Window _gfx_platform_get_window(unsigned int num)
{
	if(!_gfx_x11) return NULL;

	/* Validate the number first */
	if(num >= _gfx_x11->numWindows) return NULL;
	return UINT_TO_VOID(_gfx_x11->windows[num].handle);
}

/******************************************************/
GFX_Platform_Window _gfx_platform_create_window(const GFX_Platform_Attributes* attributes)
{
	/* Get visual info */
	XVisualInfo* visual = _gfx_x11_get_visual(
		attributes->screen,
		attributes->redBits,
		attributes->blueBits,
		attributes->greenBits
	);

	if(!visual) return NULL;

	GFX_X11_Window window;

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

	/* Set protocols */
	_gfx_x11_set_atoms(window.handle);
	XStoreName(_gfx_x11->display, window.handle, attributes->name);

	/* Create OpenGL Context */
	window.context = glXCreateContext(_gfx_x11->display, visual, NULL, True);
	glXMakeCurrent(_gfx_x11->display, window.handle, window.context);

	XFree(visual);

	/* Add window to array */
	_gfx_x11_add_window(window);

	return UINT_TO_VOID(window.handle);
}

/******************************************************/
void _gfx_platform_destroy_window(GFX_Platform_Window handle)
{
	if(_gfx_x11)
	{
		/* Remove the handle from the array */
		unsigned int i;
		for(i = 0; i < _gfx_x11->numWindows; ++i)
			if(_gfx_x11->windows[i].handle == VOID_TO_UINT(handle))
		{
			/* But first destroy context */
			glXDestroyContext(_gfx_x11->display, _gfx_x11->windows[i].context);

			--_gfx_x11->numWindows;
			if(!_gfx_x11->numWindows)
			{
				/* Free the array */
				free(_gfx_x11->windows);
				_gfx_x11->windows = NULL;
			}
			else
			{
				/* Move elements and resize array */
				GFX_X11_Window* dest = _gfx_x11->windows + i;
				memmove(dest, dest + 1, sizeof(GFX_X11_Window) * (_gfx_x11->numWindows - i));
				_gfx_x11->windows = (GFX_X11_Window*)realloc(
					_gfx_x11->windows,
					sizeof(GFX_X11_Window) * _gfx_x11->numWindows);
			}
			break;
		}

		/* Destroy the window and its colormap */
		XWindowAttributes attr;
		XGetWindowAttributes(_gfx_x11->display, VOID_TO_UINT(handle), &attr);

		XDestroyWindow(_gfx_x11->display, VOID_TO_UINT(handle));
		XFreeColormap(_gfx_x11->display, attr.colormap);
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
void _gfx_platform_window_make_current(GFX_Platform_Window handle)
{
	/* Get context and make it current */
	unsigned int i;
	if(_gfx_x11) for(i = 0; i < _gfx_x11->numWindows; ++i)
		if(_gfx_x11->windows[i].handle == VOID_TO_UINT(handle))
			glXMakeCurrent(_gfx_x11->display, (Window)VOID_TO_UINT(handle), _gfx_x11->windows[i].context);
}

/******************************************************/
void _gfx_platform_window_swap_buffers(GFX_Platform_Window handle)
{
	if(_gfx_x11) glXSwapBuffers(_gfx_x11->display, (Window)VOID_TO_UINT(handle));
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
