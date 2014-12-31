/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/threading.h"

#include <errno.h>
#include <stdlib.h>
#include <time.h>

/******************************************************/
/* Internal thread arguments */
typedef struct GFX_ThreadArgs
{
	GFX_ThreadAddress  addr;
	void*              args;

} GFX_ThreadArgs;


/******************************************************/
static void* _gfx_unix_thread_addr(

		void* arg)
{
	GFX_ThreadArgs data = *(GFX_ThreadArgs*)arg;
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
	GFX_ThreadArgs* data = malloc(sizeof(GFX_ThreadArgs));
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
int _gfx_platform_thread_join(

		GFX_PlatformThread  thread,
		unsigned int*       ret)
{
	void* val = NULL;
	if(pthread_join(thread, &val)) return 0;

	if(ret) *ret = GFX_VOID_TO_UINT(val);

	return 1;
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
