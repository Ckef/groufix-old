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

#include "groufix/events.h"

#include "groufix/platform.h"
#include <stdio.h>

/******************************************************/
void _gfx_event_window_close(void* window)
{
	/* TEMPORARY TO NOT HAVE TO CRASH THE APPLICATION!!!!!!!!!!!!! */
	_gfx_platform_terminate();
}

/******************************************************/
void _gfx_event_key_press(void* window, GFXKey key, GFXKeyState state)
{
	printf("%i <- KEY PRESS\n", key);

	/* TEMPORARY TO NOT HAVE TO CRASH THE APPLICATION!!!!!!!!!!!!! */
	if(key == GFX_KEY_RETURN) _gfx_platform_terminate();
}

/******************************************************/
void _gfx_event_key_release(void* window, GFXKey key, GFXKeyState state)
{
	printf("%i <- KEY RELEASE\n", key);
}

/******************************************************/
void _gfx_event_mouse_move(void* window, int x, int y, GFXKeyState state)
{
	printf("%i, %i\n%i <- MOUSE MOVE\n", x, y, state);
}

/******************************************************/
void _gfx_event_mouse_press(void* window, GFXMouseKey key, int x, int y, GFXKeyState state)
{
	printf("%i, %i\n%i <- MOUSE PRESS\n", x, y, key);
}

/******************************************************/
void _gfx_event_mouse_release(void* window, GFXMouseKey key, int x, int y, GFXKeyState state)
{
	printf("%i, %i\n%i <- MOUSE RELEASE\n", x, y, key);
}

/******************************************************/
void _gfx_event_mouse_wheel(void* window, int xoffset, int yoffset, int x, int y, GFXKeyState state)
{
	printf("%i, %i\n%i, %i <- MOUSE WHEEL\n", x, y, xoffset, yoffset);
}
