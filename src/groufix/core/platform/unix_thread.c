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

#include <stdlib.h>

/******************************************************/
/* Internal thread arguments */
struct GFX_ThreadArgs
{
	GFX_ThreadAddress  addr;
	void*              args;
};

/******************************************************/
static void* _gfx_unix_thread_addr(void* arg)
{
	struct GFX_ThreadArgs data = *(struct GFX_ThreadArgs*)arg;
	free(arg);

	return GFX_UINT_TO_VOID(data.addr(data.args));
}

/******************************************************/
int _gfx_platform_thread_init(GFX_PlatformThread* thread, GFX_ThreadAddress func, void* arg, int joinable)
{
	/* Create arguments */
	struct GFX_ThreadArgs* data = malloc(sizeof(struct GFX_ThreadArgs));
	if(!data) return 0;

	data->addr = func;
	data->args = arg;

	/* Create attributes */
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, joinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED);

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
int _gfx_platform_thread_join(GFX_PlatformThread handle, unsigned int* ret)
{
	void* val = NULL;
	if(pthread_join(handle, &val)) return 0;

	if(ret) *ret = GFX_VOID_TO_UINT(val);

	return 1;
}

/******************************************************/
void _gfx_platform_thread_exit(unsigned int ret)
{
	pthread_exit(GFX_UINT_TO_VOID(ret));
}
