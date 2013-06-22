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

/******************************************************/
GFX_X11_Connection* _gfx_x11 = NULL;

/******************************************************/
VectorIterator _gfx_x11_get_window_from_handle(Window handle)
{
	VectorIterator it;
	if(_gfx_x11) for(it = _gfx_x11->windows->begin; it != _gfx_x11->windows->end; it = vector_next(_gfx_x11->windows, it))
		if(((GFX_X11_Window*)it)->handle == handle) return it;

	return NULL;
}

/******************************************************/
int _gfx_x11_error_handler(Display* display, XErrorEvent* evt)
{
	return 0;
}

/******************************************************/
static GFXKey _gfx_x11_get_key(KeySym symbol)
{
	/* Unicode numbers */
	if(symbol >= XK_0 && symbol <= XK_9) return (GFXKey)(symbol - XK_0 + GFX_KEY_0);

	/* Unicode capitals */
	if(symbol >= XK_A && symbol <= XK_Z) return (GFXKey)(symbol - XK_A + GFX_KEY_A);

	/* Unicode lowercase */
	if(symbol >= XK_a && symbol <= XK_z) return (GFXKey)(symbol - XK_a + GFX_KEY_A);

	/* Function keys */
	if(symbol >= XK_F1 && symbol <= XK_F24) return (GFXKey)(symbol - XK_F1 + GFX_KEY_F1);

	/* Non-unicode */
	switch(symbol)
	{
		case XK_VoidSymbol   : return GFX_KEY_UNKNOWN;

		case XK_BackSpace    : return GFX_KEY_BACKSPACE;
		case XK_Tab          : return GFX_KEY_TAB;
		case XK_KP_Tab       : return GFX_KEY_TAB;
		case XK_Clear        : return GFX_KEY_CLEAR;
		case XK_Return       : return GFX_KEY_RETURN;
		case XK_Pause        : return GFX_KEY_PAUSE;
		case XK_Scroll_Lock  : return GFX_KEY_SCROLL_LOCK;
		case XK_Escape       : return GFX_KEY_ESCAPE;
		case XK_Delete       : return GFX_KEY_DELETE;
		case XK_KP_Delete    : return GFX_KEY_DELETE;

		case XK_Home         : return GFX_KEY_HOME;
		case XK_KP_Home      : return GFX_KEY_HOME;
		case XK_Left         : return GFX_KEY_LEFT;
		case XK_KP_Left      : return GFX_KEY_LEFT;
		case XK_Up           : return GFX_KEY_UP;
		case XK_KP_Up        : return GFX_KEY_UP;
		case XK_Right        : return GFX_KEY_RIGHT;
		case XK_KP_Right     : return GFX_KEY_RIGHT;
		case XK_Down         : return GFX_KEY_DOWN;
		case XK_KP_Down      : return GFX_KEY_DOWN;
		case XK_Page_Down    : return GFX_KEY_PAGE_DOWN;
		case XK_KP_Page_Down : return GFX_KEY_PAGE_DOWN;
		case XK_Page_Up      : return GFX_KEY_PAGE_UP;
		case XK_KP_Page_Up   : return GFX_KEY_PAGE_UP;
		case XK_End          : return GFX_KEY_END;
		case XK_KP_End       : return GFX_KEY_END;

		case XK_Select       : return GFX_KEY_SELECT;
		case XK_Print        : return GFX_KEY_PRINT;
		case XK_Execute      : return GFX_KEY_EXECUTE;
		case XK_Insert       : return GFX_KEY_INSERT;
		case XK_KP_Insert    : return GFX_KEY_INSERT;
		case XK_Menu         : return GFX_KEY_MENU;
		case XK_Cancel       : return GFX_KEY_CANCEL;
		case XK_Help         : return GFX_KEY_HELP;
		case XK_Num_Lock     : return GFX_KEY_NUM_LOCK;
		case XK_KP_Space     : return GFX_KEY_SPACE;
		case XK_space        : return GFX_KEY_SPACE;

		case XK_KP_Enter     : return GFX_KEY_KP_RETURN;
		case XK_KP_0         : return GFX_KEY_KP_0;
		case XK_KP_1         : return GFX_KEY_KP_1;
		case XK_KP_2         : return GFX_KEY_KP_2;
		case XK_KP_3         : return GFX_KEY_KP_3;
		case XK_KP_4         : return GFX_KEY_KP_4;
		case XK_KP_5         : return GFX_KEY_KP_5;
		case XK_KP_6         : return GFX_KEY_KP_6;
		case XK_KP_7         : return GFX_KEY_KP_7;
		case XK_KP_8         : return GFX_KEY_KP_8;
		case XK_KP_9         : return GFX_KEY_KP_9;
		case XK_KP_Multiply  : return GFX_KEY_KP_MULTIPLY;
		case XK_KP_Add       : return GFX_KEY_KP_ADD;
		case XK_KP_Separator : return GFX_KEY_KP_SEPARATOR;
		case XK_KP_Subtract  : return GFX_KEY_KP_SUBTRACT;
		case XK_KP_Decimal   : return GFX_KEY_KP_DECIMAL;
		case XK_KP_Divide    : return GFX_KEY_KP_DIVIDE;

		case XK_Shift_L      : return GFX_KEY_SHIFT_LEFT;
		case XK_Shift_R      : return GFX_KEY_SHIFT_RIGHT;
		case XK_Control_L    : return GFX_KEY_CONTROL_LEFT;
		case XK_Control_R    : return GFX_KEY_CONTROL_RIGHT;
		case XK_Alt_L        : return GFX_KEY_ALT_LEFT;
		case XK_Alt_R        : return GFX_KEY_ALT_RIGHT;
		case XK_Super_L      : return GFX_KEY_SUPER_LEFT;
		case XK_Super_R      : return GFX_KEY_SUPER_RIGHT;
	}

	return GFX_KEY_UNKNOWN;
}

/******************************************************/
static void _gfx_x11_create_key_table(void)
{
	/* Get permitted keycodes and their symbols */
	int minKey, maxKey;
	XDisplayKeycodes(_gfx_x11->display, &minKey, &maxKey);
	if(maxKey > GFX_X11_MAX_KEYCODE) maxKey = GFX_X11_MAX_KEYCODE;
	int numKeys = maxKey - minKey + 1;

	int symbolsPerKey;
	KeySym* symbols = XGetKeyboardMapping(_gfx_x11->display, minKey, numKeys, &symbolsPerKey);

	/* Use the first symbol of all keycodes */
	size_t i;
	for(i = minKey; i <= maxKey; ++i)
		_gfx_x11->keys[i] = _gfx_x11_get_key(symbols[(i - minKey) * symbolsPerKey]);

	XFree(symbols);
}

/******************************************************/
int _gfx_platform_init(void)
{
	if(!_gfx_x11)
	{
		/* Allocate */
		_gfx_x11 = (GFX_X11_Connection*)calloc(1, sizeof(GFX_X11_Connection));
		if(!_gfx_x11) return 0;

		/* Connect to X Server */
		_gfx_x11->display = XOpenDisplay(NULL);
		
		/* Setup memory */
		_gfx_x11->windows = vector_create(sizeof(GFX_X11_Window));
		if(!_gfx_x11->display || !_gfx_x11->windows)
		{
			_gfx_platform_terminate();
			return 0;
		}
		_gfx_x11_create_key_table();

		/* Setup Xlib */
		XSetErrorHandler(_gfx_x11_error_handler);
		_gfx_x11->wmDeleteWindow = XInternAtom(_gfx_x11->display, "WM_DELETE_WINDOW", False);
	}
	return 1;
}

/******************************************************/
int _gfx_platform_is_initialized(void)
{
	return (size_t)_gfx_x11;
}

/******************************************************/
void _gfx_platform_terminate(void)
{
	if(_gfx_x11)
	{
		/* Close connection (destroys all resources) */
		XCloseDisplay(_gfx_x11->display);
		vector_free(_gfx_x11->windows);

		/* Deallocate */
		free(_gfx_x11);
		_gfx_x11 = NULL;
	}
}

/******************************************************/
Display* gfx_x11_get_display(void)
{
	if(!_gfx_x11) return NULL;
	return _gfx_x11->display;
}

/******************************************************/
Screen* gfx_x11_get_screen(GFX_Platform_Screen screen)
{
	return (Screen*)screen;
}

/******************************************************/
Window gfx_x11_get_window(GFX_Platform_Window window)
{
	return (Window)VOID_TO_UINT(window);
}

/******************************************************/
GLXContext gfx_x11_get_context(GFX_Platform_Window window)
{
	GFX_X11_Window* it = _gfx_x11_get_window_from_handle(VOID_TO_UINT(window));
	if(it) return it->context;

	return NULL;
}
