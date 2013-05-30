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
#include <stdlib.h>

//******************************************************/
GFX_X11_Server* _gfx_server = NULL;

//******************************************************/
int _gfx_platform_init(void)
{
	if(!_gfx_server)
	{
		/* Connect to X Server */
		Display* display = XOpenDisplay(NULL);
		if(!display) return 0;

		/* Allocate */
		_gfx_server = (GFX_X11_Server*)calloc(1, sizeof(GFX_X11_Server));
		_gfx_server->display = (void*)display;
	}
	return 1;
}

//******************************************************/
int _gfx_platform_is_initialized(void)
{
	return (size_t)_gfx_server;
}

//******************************************************/
void _gfx_platform_terminate(void)
{
	if(_gfx_server)
	{
		/* Close connection */
		XCloseDisplay((Display*)_gfx_server->display);

		/* Deallocate server */
		free(_gfx_server);
		_gfx_server = NULL;
	}
}
