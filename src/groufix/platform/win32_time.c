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

#include "groufix/platform/win32.h"

#include <Mmsystem.h>

/******************************************************/
/* Whether the performance counter is used */
static unsigned char _gfx_timer_pc = 0;

/* Time resolution */
static double _gfx_timer_resolution = 1e-3;

/******************************************************/
void _gfx_platform_init_timer(void)
{
	LARGE_INTEGER freq;

	/* Check whether the performance counter is available */
	if(QueryPerformanceFrequency(&freq))
	{
		_gfx_timer_pc = 1;
		_gfx_timer_resolution = 1.0 / (double)freq.QuadPart;
	}
}

/******************************************************/
uint64_t _gfx_platform_get_time(void)
{
	if(_gfx_timer_pc)
	{
		LARGE_INTEGER cnt;
		QueryPerformanceCounter(&cnt);

		return cnt.QuadPart;
	}
	else
	{
		return timeGetTime();
	}
}

/******************************************************/
double _gfx_platform_get_time_resolution(void)
{
	return _gfx_timer_resolution;
}
