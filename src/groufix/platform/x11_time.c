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

#include <time.h>
#include <stdint.h>
#include <sys/time.h>

/******************************************************/
/* Whether the monotonic timer is used */
static unsigned char _gfx_timer_monotonic = 0;

/* Time resolution */
static double _gfx_timer_resolution = 1e-6;

/******************************************************/
void _gfx_platform_init_timer(void)
{
	struct timespec ts;

	/* Check whether monotonic time is available */
	if(!clock_gettime(CLOCK_MONOTONIC, &ts))
	{
		_gfx_timer_monotonic = 1;
		_gfx_timer_resolution = 1e-9;
	}
}

/******************************************************/
uint64_t _gfx_platform_get_time(void)
{
	if(_gfx_timer_monotonic)
	{
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);

		return (uint64_t)ts.tv_sec * 1000000000 + (uint64_t)ts.tv_nsec;
	}
	else
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);

		return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;
	}
}

/******************************************************/
double _gfx_platform_get_time_resolution(void)
{
	return _gfx_timer_resolution;
}
