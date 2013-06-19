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
GFX_Platform_Context _gfx_platform_create_context(GFX_Platform_Window handle, const GFX_Platform_ContextAttributes* attributes)
{
	/* Get the screen associated with the window */
	Screen* screen = _gfx_platform_window_get_screen(handle);
	if(!screen) return NULL;

	/* Create buffer attribute array */
	int bufferAttr[] = {
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE,   GLX_RGBA_BIT,
		GLX_DOUBLEBUFFER,  True,
		GLX_RED_SIZE,      attributes->redBits,
		GLX_BLUE_SIZE,     attributes->blueBits,
		GLX_GREEN_SIZE,    attributes->greenBits,
		None
	};

	/* Get visual info of screen */
	int nelements;
	GLXFBConfig* config = glXChooseFBConfig(_gfx_x11->display, XScreenNumberOfScreen(screen), bufferAttr, &nelements);
	XVisualInfo* visual = glXGetVisualFromFBConfig(_gfx_x11->display, *config);
	if(!visual)
	{
		XFree(config);
		return NULL;
	}

	XFree(config);
	XFree(visual);

	return NULL;
}

/******************************************************/
void _gfx_platform_destroy_context(GFX_Platform_Context handle)
{
	if(_gfx_x11) glXDestroyContext(_gfx_x11->display, handle);
}

/******************************************************/
void _gfx_platform_context_swap_buffers(GFX_Platform_Context handle)
{
}
