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

/******************************************************/
int _gfx_platform_context_create(

		GFX_PlatformWindow  handle,
		int                 major,
		int                 minor,
		GFX_PlatformWindow  share)
{
	/* Get the windows */
	GFX_X11_Window* window = _gfx_x11_get_window_from_handle(
		GFX_VOID_TO_UINT(handle));

	if(!window) return 0;

	GFX_X11_Window* shareWind = NULL;
	if(share) shareWind = _gfx_x11_get_window_from_handle(
		GFX_VOID_TO_UINT(share));

	/* Create buffer attribute array */
	int bufferAttr[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, major,
		GLX_CONTEXT_MINOR_VERSION_ARB, minor,
		GLX_CONTEXT_FLAGS_ARB,         GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None
	};

	/* Create the context */
	GLXContext shareCont = NULL;
	if(shareWind) shareCont = shareWind->context;

	window->context = _gfx_x11->extensions.CreateContextAttribsARB(
		_gfx_x11->display,
		window->config,
		shareCont,
		True,
		bufferAttr
	);

	/* Make it current */
	if(window->context)
	{
		glXMakeCurrent(
			_gfx_x11->display,
			window->handle,
			window->context
		);
		_gfx_x11->current = window->handle;

		return 1;
	}
	return 0;
}

/******************************************************/
void _gfx_platform_context_free(

		GFX_PlatformWindow handle)
{
	/* Get the window and destroy its context */
	GFX_X11_Window* window = _gfx_x11_get_window_from_handle(
		GFX_VOID_TO_UINT(handle));

	if(window)
	{
		glXMakeCurrent(_gfx_x11->display, None, NULL);
		glXDestroyContext(_gfx_x11->display, window->context);

		window->context = NULL;
	}
}

/******************************************************/
void _gfx_platform_context_make_current(

		GFX_PlatformWindow handle)
{
	GFX_X11_Window* window = _gfx_x11_get_window_from_handle(
		GFX_VOID_TO_UINT(handle));

	if(window)
	{
		glXMakeCurrent(_gfx_x11->display, window->handle, window->context);
		_gfx_x11->current = window->handle;
	}
}

/******************************************************/
void _gfx_platform_context_set_swap_interval(

		GFX_PlatformWindow  handle,
		int                 num)
{
	/* Correct if adaptive vsync is not supported */
	if(!_gfx_x11->extensions.EXT_swap_control_tear && num < 0)
		num = -num;

	if(_gfx_x11->extensions.SwapIntervalEXT)
		_gfx_x11->extensions.SwapIntervalEXT(
			_gfx_x11->display,
			(Window)GFX_VOID_TO_UINT(handle), num
		);
}

/******************************************************/
void _gfx_platform_context_swap_buffers(void)
{
	if(_gfx_x11) glXSwapBuffers(_gfx_x11->display, _gfx_x11->current);
}

/******************************************************/
int _gfx_platform_is_extension_supported(

		GFX_PlatformWindow  handle,
		const char*         ext)
{
	/* Get screen */
	Screen* screen = (Screen*)_gfx_platform_window_get_screen(handle);
	if(!screen) return 0;

	return _gfx_x11_is_extension_supported(
		XScreenNumberOfScreen(screen),
		ext
	);
}

/******************************************************/
GFX_ProcAddress _gfx_platform_get_proc_address(

		const char* proc)
{
	return (GFX_ProcAddress)glXGetProcAddressARB((const GLubyte*)proc);
}
