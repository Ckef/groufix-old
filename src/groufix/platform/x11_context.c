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

/******************************************************/
int _gfx_platform_create_context(GFX_Platform_Window handle, unsigned short major, unsigned short minor)
{
	/* Get the window */
	GFX_X11_Window* window = _gfx_x11_get_window_from_handle(VOID_TO_UINT(handle));
	if(!window) return 0;

	/* Create buffer attribute array */
	int bufferAttr[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, major,
		GLX_CONTEXT_MINOR_VERSION_ARB, minor,
		GLX_CONTEXT_FLAGS_ARB,         GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None
	};

	/* Create the context */
	window->context = _gfx_x11->extensions.CreateContextAttribsARB(
		_gfx_x11->display,
		window->config,
		NULL,
		True,
		bufferAttr
	);

	if(!window->context) return 0;
	glXMakeCurrent(_gfx_x11->display, window->handle, window->context);

	return 1;
}

/******************************************************/
void _gfx_platform_destroy_context(GFX_Platform_Window handle)
{
	/* Get the window and destroy its context */
	GFX_X11_Window* window = _gfx_x11_get_window_from_handle(VOID_TO_UINT(handle));
	if(window)
	{
		glXMakeCurrent(_gfx_x11->display, None, NULL);
		glXDestroyContext(_gfx_x11->display, window->context);

		window->context = NULL;
	}
}

/******************************************************/
int _gfx_platform_context_get(GFX_Platform_Window handle, unsigned short* major, unsigned short* minor)
{
	if(!_gfx_platform_context_make_current(handle)) return 0;

	int ma, mi;
	glGetIntegerv(GL_MAJOR_VERSION, &ma);
	glGetIntegerv(GL_MINOR_VERSION, &mi);

	*major = ma;
	*minor = mi;

	return 1;
}

/******************************************************/
int _gfx_platform_context_make_current(GFX_Platform_Window handle)
{
	GFX_X11_Window* window = _gfx_x11_get_window_from_handle(VOID_TO_UINT(handle));
	if(window) return glXMakeCurrent(_gfx_x11->display, window->handle, window->context);

	return 0;
}

/******************************************************/
void _gfx_platform_context_swap_buffers(GFX_Platform_Window handle)
{
	if(_gfx_x11) glXSwapBuffers(_gfx_x11->display, (Window)VOID_TO_UINT(handle));
}

/******************************************************/
int _gfx_platform_is_extension_supported(GFX_Platform_Window handle, const char* ext)
{
	/* Get screen */
	Screen* screen = (Screen*)_gfx_platform_window_get_screen(handle);
	if(!screen) return 0;

	return _gfx_x11_is_extension_supported(XScreenNumberOfScreen(screen), ext);
}

/******************************************************/
void* _gfx_platform_get_proc_address(const char* proc)
{
	return (void*)glXGetProcAddressARB((const GLubyte*)proc);
}
