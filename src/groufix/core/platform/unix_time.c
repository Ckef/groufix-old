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

#include <stdint.h>
#include <time.h>

/******************************************************/
/* The clock ID being used */
static clockid_t _gfx_timer_clock_id = CLOCK_REALTIME;

/******************************************************/
void _gfx_platform_init_timer(void)
{
	struct timespec ts;

	/* Check whether monotonic time is available */
	if(!clock_gettime(CLOCK_MONOTONIC, &ts))
	{
		_gfx_timer_clock_id = CLOCK_MONOTONIC;
	}
}

/******************************************************/
uint64_t _gfx_platform_get_time(void)
{
	struct timespec ts;
	clock_gettime(_gfx_timer_clock_id, &ts);

	return (uint64_t)ts.tv_sec * 1000000000 + (uint64_t)ts.tv_nsec;
}

/******************************************************/
double _gfx_platform_get_time_resolution(void)
{
	return 1e-9;
}
