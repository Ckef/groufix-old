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

#include "groufix/core/platform.h"

/******************************************************/
int _gfx_platform_thread_init(GFX_PlatformThread* thread, GFX_ThreadAddress func, void* arg, int joinable)
{
	/* Create attributes */
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, joinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED);

	/* Create thread */
	int ret = pthread_create(thread, &attr, func, arg);

	pthread_attr_destroy(&attr);

	return !ret;
}

/******************************************************/
int _gfx_platform_thread_equal(GFX_PlatformThread thread1, GFX_PlatformThread thread2)
{
	return pthread_equal(thread1, thread2);
}

/******************************************************/
void _gfx_platform_thread_exit(void* ret)
{
	pthread_exit(ret);
}

/******************************************************/
int _gfx_platform_thread_join(GFX_PlatformThread handle, void** ret)
{
	return !pthread_join(handle, ret);
}
