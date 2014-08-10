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

#include <errno.h>
#include <stdlib.h>
#include <time.h>

/******************************************************/
/* Internal thread arguments */
struct GFX_ThreadArgs
{
	GFX_ThreadAddress  addr;
	void*              args;
};

/******************************************************/
static void* _gfx_unix_thread_addr(

		void* arg)
{
	struct GFX_ThreadArgs data = *(struct GFX_ThreadArgs*)arg;
	free(arg);

	return GFX_UINT_TO_VOID(data.addr(data.args));
}

/******************************************************/
int _gfx_platform_thread_init(

		GFX_PlatformThread*  thread,
		GFX_ThreadAddress    func,
		void*                arg,
		int                  joinable)
{
	/* Create arguments */
	struct GFX_ThreadArgs* data = malloc(sizeof(struct GFX_ThreadArgs));
	if(!data) return 0;

	data->addr = func;
	data->args = arg;

	/* Create attributes */
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(
		&attr,
		joinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED
	);

	/* Create thread */
	if(pthread_create(thread, &attr, _gfx_unix_thread_addr, data))
	{
		pthread_attr_destroy(&attr);
		free(data);

		return 0;
	}

	pthread_attr_destroy(&attr);

	return 1;
}

/******************************************************/
void _gfx_platform_thread_detach(

		GFX_PlatformThread handle)
{
	pthread_detach(handle);
}

/******************************************************/
int _gfx_platform_thread_join(

		GFX_PlatformThread  handle,
		unsigned int*       ret)
{
	void* val = NULL;
	if(pthread_join(handle, &val)) return 0;

	if(ret) *ret = GFX_VOID_TO_UINT(val);

	return 1;
}

/******************************************************/
void _gfx_platform_thread_exit(

		unsigned int ret)
{
	pthread_exit(GFX_UINT_TO_VOID(ret));
}

/******************************************************/
int _gfx_platform_mutex_init(

		GFX_PlatformMutex* mutex)
{
	return !pthread_mutex_init(mutex, NULL);
}

/******************************************************/
void _gfx_platform_mutex_clear(

		GFX_PlatformMutex* mutex)
{
	pthread_mutex_destroy(mutex);
}

/******************************************************/
int _gfx_platform_mutex_lock(

		GFX_PlatformMutex* mutex)
{
	return !pthread_mutex_lock(mutex);
}

/******************************************************/
int _gfx_platform_mutex_try_lock(

		GFX_PlatformMutex* mutex)
{
	return !pthread_mutex_trylock(mutex);
}

/******************************************************/
void _gfx_platform_mutex_unlock(

		GFX_PlatformMutex* mutex)
{
	pthread_mutex_unlock(mutex);
}

/******************************************************/
int _gfx_platform_cond_init(

		GFX_PlatformCond* cond)
{
	return !pthread_cond_init(cond, NULL);
}

/******************************************************/
void _gfx_platform_cond_clear(

		GFX_PlatformCond* cond)
{
	pthread_cond_destroy(cond);
}

/******************************************************/
int _gfx_platform_cond_wait(

		GFX_PlatformCond*   cond,
		GFX_PlatformMutex*  mutex)
{
	return !pthread_cond_wait(cond, mutex);
}

/******************************************************/
int _gfx_platform_cond_wait_time(

		GFX_PlatformCond*   cond,
		GFX_PlatformMutex*  mutex,
		uint64_t            nsec)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);

	ts.tv_sec += nsec / 1000000000;
	ts.tv_nsec += nsec % 1000000000;

	ts.tv_sec += ts.tv_nsec / 1000000000;
	ts.tv_nsec = ts.tv_nsec % 1000000000;

	int ret = pthread_cond_timedwait(cond, mutex, &ts);

	return (ret == ETIMEDOUT) ? -1 : (ret ? 0 : 1);
}

/******************************************************/
void _gfx_platform_cond_signal(

		GFX_PlatformCond* cond)
{
	pthread_cond_signal(cond);
}

/******************************************************/
void _gfx_platform_cond_broadcast(

		GFX_PlatformCond* cond)
{
	pthread_cond_broadcast(cond);
}
