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

#include "groufix/internal.h"

/******************************************************/
void _gfx_event_window_close(GFX_Platform_Window handle)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window->callbacks.windowClose) window->callbacks.windowClose(window);

	/* Destroy window if it has no callback */
	else _gfx_window_destroy((GFX_Internal_Window*)window);
}

/******************************************************/
void _gfx_event_window_move(GFX_Platform_Window handle, int x, int y)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window->callbacks.windowMove) window->callbacks.windowMove(window, x, y);
}

/******************************************************/
void _gfx_event_window_resize(GFX_Platform_Window handle, unsigned int width, unsigned int height)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window->callbacks.windowResize) window->callbacks.windowResize(window, width, height);
}

/******************************************************/
void _gfx_event_key_press(GFX_Platform_Window handle, GFXKey key, GFXKeyState state)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window->callbacks.keyPress) window->callbacks.keyPress(window, key, state);
}

/******************************************************/
void _gfx_event_key_release(GFX_Platform_Window handle, GFXKey key, GFXKeyState state)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window->callbacks.keyRelease) window->callbacks.keyRelease(window, key, state);
}

/******************************************************/
void _gfx_event_mouse_move(GFX_Platform_Window handle, int x, int y, GFXKeyState state)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window->callbacks.mouseMove) window->callbacks.mouseMove(window, x, y, state);
}

/******************************************************/
void _gfx_event_mouse_enter(GFX_Platform_Window handle, int x, int y, GFXKeyState state)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window->callbacks.mouseEnter) window->callbacks.mouseEnter(window, x, y, state);
}

/******************************************************/
void _gfx_event_mouse_leave(GFX_Platform_Window handle, int x, int y, GFXKeyState state)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window->callbacks.mouseLeave) window->callbacks.mouseLeave(window, x, y, state);
}

/******************************************************/
void _gfx_event_mouse_press(GFX_Platform_Window handle, GFXMouseKey key, int x, int y, GFXKeyState state)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window->callbacks.mousePress) window->callbacks.mousePress(window, key, x, y, state);
}

/******************************************************/
void _gfx_event_mouse_release(GFX_Platform_Window handle, GFXMouseKey key, int x, int y, GFXKeyState state)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window->callbacks.mouseRelease) window->callbacks.mouseRelease(window, key, x, y, state);
}

/******************************************************/
void _gfx_event_mouse_wheel(GFX_Platform_Window handle, int xoffset, int yoffset, int x, int y, GFXKeyState state)
{
	GFXWindow* window = (GFXWindow*)_gfx_window_get_from_handle(handle);
	if(window->callbacks.mouseWheel) window->callbacks.mouseWheel(window, xoffset, yoffset, x, y, state);
}
