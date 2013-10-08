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

#include "groufix.h"
#include "groufix/internal.h"

/******************************************************/
int gfx_init(void)
{
	/* Initialize platform */
	if(_gfx_platform_init()) return 1;

	return 0;
}

/******************************************************/
int gfx_poll_events(void)
{
	/* Check if platform is initialized */
	if(!_gfx_platform_is_initialized()) return 0;

	/* If so, poll events! */
	_gfx_platform_poll_events();

	return 1;
}

/******************************************************/
void gfx_terminate(void)
{
	/* Destroy all windows */
	unsigned int i = gfx_get_num_windows();
	while(i) _gfx_window_destroy((GFX_Internal_Window*)gfx_get_window(--i));

	/* Terminate platform */
	_gfx_platform_terminate();

	/* Empty error queue */
	gfx_errors_empty();
}
