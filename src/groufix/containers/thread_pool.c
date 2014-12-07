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

#include "groufix/containers/thread_pool.h"
#include "groufix/containers/list.h"
#include "groufix/containers/vector.h"
#include "groufix/core/errors.h"
#include "groufix/core/platform.h"

#include <stdlib.h>

/******************************************************/
/* Actual task */
struct GFX_Task
{
	char               priority;
	void*              data;
	GFXThreadPoolTask  task;
};

/* Actual thread node */
struct GFX_ThreadList
{
	GFXList        node; /* Super class */
	unsigned char  size; /* Number of threads of this particular node */
};

/* Internal thread pool */
struct GFX_Pool
{
	/* Super class */
	GFXThreadPool pool;

	/* Hidden data */
	GFXVector              tasks;   /* Priority queue storing GFX_Task */
	struct GFXThreadList*  threads; /* All associated threads */

	GFX_PlatformMutex      mutex;
	GFX_PlatformCond       cond;
};

/******************************************************/
GFXThreadPool* gfx_thread_pool_create(

		unsigned char size)
{
	/* Create a new thread pool */
	struct GFX_Pool* pool = malloc(sizeof(struct GFX_Pool));
	if(!pool)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Thread pool could not be allocated."
		);
		return NULL;
	}

	/* Create mutex */
	if(_gfx_platform_mutex_init(&pool->mutex))
	{
		/* Create condition variable */
		if(_gfx_platform_cond_init(&pool->cond))
		{
			pool->pool.size = 0;
			pool->threads = NULL;
			gfx_vector_init(&pool->tasks, sizeof(struct GFX_Task));

			return (GFXThreadPool*)pool;
		}

		_gfx_platform_mutex_clear(&pool->mutex);
	}

	/* Nevermind */
	free(pool);

	return NULL;
}

/******************************************************/
void gfx_thread_pool_free(

		GFXThreadPool* pool)
{
	if(pool)
	{
		struct GFX_Pool* internal = (struct GFX_Pool*)pool;

		/* Clear all the things */
		gfx_vector_clear(&internal->tasks);
		gfx_list_free((GFXList*)internal->threads);

		_gfx_platform_mutex_clear(&internal->mutex);
		_gfx_platform_cond_clear(&internal->cond);

		free(pool);
	}
}
