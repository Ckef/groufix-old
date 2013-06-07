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
	/* Validate symbol */
	if(symbol == NoSymbol) return GFX_KEY_UNKNOWN;

	switch(symbol)
	{
		case XK_BackSpace : return GFX_KEY_BACKSPACE;
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
