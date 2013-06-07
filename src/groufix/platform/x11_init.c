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

#include "groufix/platform.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdlib.h>

/******************************************************/
GFX_X11_Connection* _gfx_x11 = NULL;

/******************************************************/
static GFXKey _gfx_x11_get_key(KeySym symbol)
{
	/* Unicode numbers */
	if(symbol >= GFX_KEY_0 && symbol <= GFX_KEY_9) return (GFXKey)symbol;

	/* Unicode capitals */
	if(symbol >= GFX_KEY_A && symbol <= GFX_KEY_Z) return (GFXKey)symbol;

	/* Unicode lowercase */
	if(symbol >= XK_a && symbol <= XK_z) return (GFXKey)(symbol - XK_a + XK_A);

	/* Non-unicode */
	switch(symbol)
	{
		case XK_VoidSymbol   : return GFX_KEY_UNKNOWN;

		case XK_BackSpace    : return GFX_KEY_BACKSPACE;
		case XK_Tab          : return GFX_KEY_TAB;
		case XK_KP_Tab       : return GFX_KEY_TAB;
		case XK_Clear        : return GFX_KEY_CLEAR;
		case XK_Return       : return GFX_KEY_RETURN;
		case XK_KP_Enter     : return GFX_KEY_RETURN;
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

		case XK_F1           : return GFX_KEY_F1;
		case XK_F2           : return GFX_KEY_F2;
		case XK_F3           : return GFX_KEY_F3;
		case XK_F4           : return GFX_KEY_F4;
		case XK_F5           : return GFX_KEY_F5;
		case XK_F6           : return GFX_KEY_F6;
		case XK_F7           : return GFX_KEY_F7;
		case XK_F8           : return GFX_KEY_F8;
		case XK_F9           : return GFX_KEY_F9;
		case XK_F10          : return GFX_KEY_F10;
		case XK_F11          : return GFX_KEY_F11;
		case XK_F12          : return GFX_KEY_F12;
		case XK_F13          : return GFX_KEY_F13;
		case XK_F14          : return GFX_KEY_F14;
		case XK_F15          : return GFX_KEY_F15;
		case XK_F16          : return GFX_KEY_F16;
		case XK_F17          : return GFX_KEY_F17;
		case XK_F18          : return GFX_KEY_F18;
		case XK_F19          : return GFX_KEY_F19;
		case XK_F20          : return GFX_KEY_F20;
		case XK_F21          : return GFX_KEY_F21;
		case XK_F22          : return GFX_KEY_F22;
		case XK_F23          : return GFX_KEY_F23;
		case XK_F24          : return GFX_KEY_F24;

		case XK_Shift_L      : return GFX_KEY_SHIFT_LEFT;
		case XK_Shift_R      : return GFX_KEY_SHIFT_RIGHT;
		case XK_Control_L    : return GFX_KEY_CONTROL_LEFT;
		case XK_Control_R    : return GFX_KEY_CONTROL_RIGHT;
		case XK_Alt_L        : return GFX_KEY_ALT_LEFT;
		case XK_Alt_R        : return GFX_KEY_ALT_RIGHT;
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
		/* Connect to X Server */
		Display* display = XOpenDisplay(NULL);
		if(!display) return 0;

		/* Allocate */
		_gfx_x11 = (GFX_X11_Connection*)calloc(1, sizeof(GFX_X11_Connection));
		_gfx_x11->display = (void*)display;

		/* Construct a keycode lookup */
		_gfx_x11_create_key_table();
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
		/* Destroy all windows (to deallocate the pointers) */
		while(_gfx_x11->numWindows) _gfx_platform_destroy_window(_gfx_x11->windows[0]);

		/* Close connection */
		XCloseDisplay(_gfx_x11->display);

		/* Deallocate server */
		free(_gfx_x11);
		_gfx_x11 = NULL;
	}
}
