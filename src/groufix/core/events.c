/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/internal.h"

/******************************************************/
/** Termination request */
unsigned char _gfx_event_terminate_request = 0;


/******************************************************/
void _gfx_event_window_close(

		GFX_PlatformWindow handle)
{
	GFXWindow* window =
		(GFXWindow*)_gfx_context_get_from_handle(handle);

	if(window)
	{
		/* Destroy window if it has no callback */
		if(window->callbacks.windowClose) window->callbacks.windowClose(window);
		else _gfx_context_destroy((GFX_Context*)window);
	}
}

/******************************************************/
void _gfx_event_window_move(

		GFX_PlatformWindow  handle,
		int                 x,
		int                 y)
{
	GFXWindow* window =
		(GFXWindow*)_gfx_context_get_from_handle(handle);
	if(window && window->callbacks.windowMove)
		window->callbacks.windowMove(window, x, y);
}

/******************************************************/
void _gfx_event_window_resize(

		GFX_PlatformWindow  handle,
		unsigned int        width,
		unsigned int        height)
{
	GFXWindow* window =
		(GFXWindow*)_gfx_context_get_from_handle(handle);

	if(window)
	{
		/* Resize all processes */
		//_gfx_pipe_process_resize(
			//(GFX_Context*)window, width, height);
		if(window->callbacks.windowResize) window->callbacks.windowResize(
			window, width, height);
	}
}

/******************************************************/
void _gfx_event_window_focus(

		GFX_PlatformWindow handle)
{
	GFXWindow* window =
		(GFXWindow*)_gfx_context_get_from_handle(handle);
	if(window && window->callbacks.windowFocus)
		window->callbacks.windowFocus(window);
}

/******************************************************/
void _gfx_event_window_blur(

		GFX_PlatformWindow handle)
{
	GFXWindow* window =
		(GFXWindow*)_gfx_context_get_from_handle(handle);
	if(window && window->callbacks.windowBlur)
		window->callbacks.windowBlur(window);
}

/******************************************************/
void _gfx_event_key_press(

		GFX_PlatformWindow  handle,
		GFXKey              key,
		GFXKeyState         state)
{
	GFXWindow* window =
		(GFXWindow*)_gfx_context_get_from_handle(handle);
	if(window && window->callbacks.keyPress)
		window->callbacks.keyPress(window, key, state);
}

/******************************************************/
void _gfx_event_key_release(

		GFX_PlatformWindow  handle,
		GFXKey              key,
		GFXKeyState         state)
{
	GFXWindow* window =
		(GFXWindow*)_gfx_context_get_from_handle(handle);
	if(window && window->callbacks.keyRelease)
		window->callbacks.keyRelease(window, key, state);
}

/******************************************************/
void _gfx_event_mouse_move(

		GFX_PlatformWindow  handle,
		int                 x,
		int                 y,
		GFXKeyState         state)
{
	GFXWindow* window =
		(GFXWindow*)_gfx_context_get_from_handle(handle);
	if(window && window->callbacks.mouseMove)
		window->callbacks.mouseMove(window, x, y, state);
}

/******************************************************/
void _gfx_event_mouse_enter(

		GFX_PlatformWindow  handle,
		int                 x,
		int                 y,
		GFXKeyState         state)
{
	GFXWindow* window =
		(GFXWindow*)_gfx_context_get_from_handle(handle);
	if(window && window->callbacks.mouseEnter)
		window->callbacks.mouseEnter(window, x, y, state);
}

/******************************************************/
void _gfx_event_mouse_leave(

		GFX_PlatformWindow  handle,
		int                 x,
		int                 y,
		GFXKeyState         state)
{
	GFXWindow* window =
		(GFXWindow*)_gfx_context_get_from_handle(handle);
	if(window && window->callbacks.mouseLeave)
		window->callbacks.mouseLeave(window, x, y, state);
}

/******************************************************/
void _gfx_event_mouse_press(

		GFX_PlatformWindow  handle,
		GFXMouseKey         key,
		int                 x,
		int                 y,
		GFXKeyState         state)
{
	GFXWindow* window =
		(GFXWindow*)_gfx_context_get_from_handle(handle);
	if(window && window->callbacks.mousePress)
		window->callbacks.mousePress(window, key, x, y, state);
}

/******************************************************/
void _gfx_event_mouse_release(

		GFX_PlatformWindow  handle,
		GFXMouseKey         key,
		int                 x,
		int                 y,
		GFXKeyState         state)
{
	GFXWindow* window =
		(GFXWindow*)_gfx_context_get_from_handle(handle);
	if(window && window->callbacks.mouseRelease)
		window->callbacks.mouseRelease(window, key, x, y, state);
}

/******************************************************/
void _gfx_event_mouse_wheel(

		GFX_PlatformWindow  handle,
		int                 xoffset,
		int                 yoffset,
		int                 x,
		int                 y,
		GFXKeyState         state)
{
	GFXWindow* window =
		(GFXWindow*)_gfx_context_get_from_handle(handle);
	if(window && window->callbacks.mouseWheel)
		window->callbacks.mouseWheel(window, xoffset, yoffset, x, y, state);
}
