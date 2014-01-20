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

#include "groufix.h"
#include "groufix/internal.h"

/******************************************************/
/* Starting point of time */
static uint64_t _gfx_time_start = 0;

/******************************************************/
int gfx_init(void)
{
	/* Initialize platform */
	if(!_gfx_platform_init()) return 0;

	/* To make it clip against the maximum */
	gfx_hardware_set_max_id_width(GFX_HARDWARE_ID_WIDTH_DEFAULT);

	/* Get starting point of time */
	_gfx_platform_init_timer();
	_gfx_time_start = _gfx_platform_get_time();

	return 1;
}

/******************************************************/
void gfx_poll_events(void)
{
	_gfx_platform_poll_events();
}

/******************************************************/
double gfx_get_time(void)
{
	return (double)(_gfx_platform_get_time() - _gfx_time_start) *
		_gfx_platform_get_time_resolution();
}

/******************************************************/
void gfx_set_time(double time)
{
	_gfx_time_start = _gfx_platform_get_time() -
		(uint64_t)(time / _gfx_platform_get_time_resolution());
}

/******************************************************/
void gfx_terminate(void)
{
	/* Destroy all windows */
	unsigned int i = gfx_get_num_windows();
	while(i) _gfx_window_destroy((GFX_Window*)gfx_get_window(--i));

	/* Terminate platform */
	_gfx_platform_terminate();

	/* Empty error queue */
	gfx_errors_empty();
}
