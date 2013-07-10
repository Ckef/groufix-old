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

#include "groufix/internal.h"

/******************************************************/
void _gfx_event_window_close(GFX_Platform_Window handle)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window)
	{
		if(window->callbacks.windowClose) window->callbacks.windowClose(window);

		/* Close window if it has no callback */
		else gfx_window_free(window);
	}
}

/******************************************************/
void _gfx_event_key_press(GFX_Platform_Window handle, GFXKey key, GFXKeyState state)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window) if(window->callbacks.keyPress) window->callbacks.keyPress(window, key, state);
}

/******************************************************/
void _gfx_event_key_release(GFX_Platform_Window handle, GFXKey key, GFXKeyState state)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window) if(window->callbacks.keyRelease) window->callbacks.keyRelease(window, key, state);
}

/******************************************************/
void _gfx_event_mouse_move(GFX_Platform_Window handle, int x, int y, GFXKeyState state)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window) if(window->callbacks.mouseMove) window->callbacks.mouseMove(window, x, y, state);
}

/******************************************************/
void _gfx_event_mouse_press(GFX_Platform_Window handle, GFXMouseKey key, int x, int y, GFXKeyState state)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window) if(window->callbacks.mousePress) window->callbacks.mousePress(window, key, x, y, state);
}

/******************************************************/
void _gfx_event_mouse_release(GFX_Platform_Window handle, GFXMouseKey key, int x, int y, GFXKeyState state)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window) if(window->callbacks.mouseRelease) window->callbacks.mouseRelease(window, key, x, y, state);
}

/******************************************************/
void _gfx_event_mouse_wheel(GFX_Platform_Window handle, int xoffset, int yoffset, int x, int y, GFXKeyState state)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window) if(window->callbacks.mouseWheel) window->callbacks.mouseWheel(window, xoffset, yoffset, x, y, state);
}
