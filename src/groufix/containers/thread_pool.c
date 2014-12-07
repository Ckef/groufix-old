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

#include <limits.h>
#include <stdlib.h>

/* Thread pool status */
#define GFX_INT_POOL_TERMINATE  0x00
#define GFX_INT_POOL_SUSPENDED  0x01
#define GFX_INT_POOL_RESUMED    0x02

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
	unsigned char           status;
	GFXVector               tasks;   /* Priority queue storing GFX_Task */
	struct GFX_ThreadList*  threads; /* All associated threads */

	GFX_PlatformMutex       mutex;
	GFX_PlatformCond        cond;
};

/******************************************************/
static inline GFX_PlatformThread* _gfx_thread_list_get(

		struct GFX_ThreadList*  list,
		unsigned char           index)
{
	return ((GFX_PlatformThread*)(list + 1)) + index;
}

/******************************************************/
static int _gfx_thread_pool_push(

		struct GFX_Pool*  pool,
		struct GFX_Task   task)
{
	/* Insert the new element */
	size_t elem = gfx_vector_get_size(&pool->tasks);

	struct GFX_Task* et = gfx_vector_insert(
		&pool->tasks,
		&task,
		pool->tasks.end
	);

	if(et == pool->tasks.end) return 0;

	/* Correct heap again */
	while(elem > 0)
	{
		/* Get parent and compare */
		size_t parent = (elem - 1) >> 1;

		struct GFX_Task* pt = gfx_vector_at(
			&pool->tasks,
			parent);

		if(pt->priority <= et->priority)
			break;

		/* Swap */
		task = *pt;
		*pt = *et;
		*et = task;

		elem = parent;
	}

	return 1;
}

/******************************************************/
static struct GFX_Task _gfx_thread_pool_pop(

		struct GFX_Pool* pool)
{
	struct GFX_Task* et = pool->tasks.begin;
	struct GFX_Task ret = *et;

	/* Override root and remove element */
	size_t size = gfx_vector_get_size(&pool->tasks) - 1;

	*et = *(struct GFX_Task*)gfx_vector_at(
		&pool->tasks, size);
	gfx_vector_erase_at(
		&pool->tasks, size);

	/* Heapify the root */
	size_t elem = 0;

	while(1)
	{
		struct GFX_Task* bt = et;
		size_t b = elem;

		/* Get child with largest priority */
		size_t l = (elem << 1) + 1;
		size_t r = (elem << 1) + 2;

		struct GFX_Task* lt = gfx_vector_at(&pool->tasks, l);
		struct GFX_Task* rt = gfx_vector_at(&pool->tasks, r);

		if(l < size && lt->priority < bt->priority)
			bt = lt, b = l;
		if(r < size && rt->priority < bt->priority)
			bt = rt, b = r;

		if(b == elem)
			break;

		/* Swap */
		struct GFX_Task temp = *bt;
		*bt = *et;
		*et = temp;

		elem = b;
		et = bt;
	}

	return ret;
}

/******************************************************/
static unsigned int _gfx_thread_addr(

		void* arg)
{
	struct GFX_Pool* pool = (struct GFX_Pool*)arg;

	/* Initialize */
	arg = NULL;

	if(pool->pool.init)
		arg = pool->pool.init();

	/* Run as long as not terminating */
	_gfx_platform_mutex_lock(&pool->mutex);

	while(pool->status != GFX_INT_POOL_TERMINATE)
	{
		/* If allowed and available, perform a task */
		if(
			pool->status == GFX_INT_POOL_RESUMED &&
			pool->tasks.begin != pool->tasks.end)
		{

			struct GFX_Task task = _gfx_thread_pool_pop(pool);

			/* Unlock during task */
			_gfx_platform_mutex_unlock(&pool->mutex);

			task.task(task.data);

			_gfx_platform_mutex_lock(&pool->mutex);
		}

		/* If not, block */
		else _gfx_platform_cond_wait(&pool->cond, &pool->mutex);
	}

	_gfx_platform_mutex_unlock(&pool->mutex);

	/* Terminate */
	if(pool->pool.terminate)
		pool->pool.terminate(arg);

	return 0;
}

/******************************************************/
GFXThreadPool* gfx_thread_pool_create(

		unsigned char           size,
		GFXThreadPoolInit       init,
		GFXThreadPoolTerminate  terminate,
		int                     suspend)
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
			/* Initialize */
			pool->status = suspend ?
				GFX_INT_POOL_SUSPENDED :
				GFX_INT_POOL_RESUMED;

			pool->pool.size = 0;
			pool->pool.init = init;
			pool->pool.terminate = terminate;

			gfx_vector_init(&pool->tasks, sizeof(struct GFX_Task));
			pool->threads = NULL;

			/* Attempt to add the threads */
			if(gfx_thread_pool_expand((GFXThreadPool*)pool, size))
				return (GFXThreadPool*)pool;

			/* Failure */
			gfx_vector_clear(&pool->tasks);
			_gfx_platform_cond_clear(&pool->cond);
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

		/* Tell threads to terminate */
		_gfx_platform_mutex_lock(&internal->mutex);

		internal->status = GFX_INT_POOL_TERMINATE;
		_gfx_platform_cond_broadcast(&internal->cond);

		_gfx_platform_mutex_unlock(&internal->mutex);

		/* Join all threads */
		struct GFX_ThreadList* node = internal->threads;
		unsigned char s;

		while(node)
		{
			for(s = 0; s < node->size; ++s) _gfx_platform_thread_join(
				*_gfx_thread_list_get(node, s),
				NULL
			);

			node = (struct GFX_ThreadList*)node->node.next;
		}

		/* Clear all the things */
		gfx_vector_clear(&internal->tasks);
		gfx_list_free((GFXList*)internal->threads);

		_gfx_platform_mutex_clear(&internal->mutex);
		_gfx_platform_cond_clear(&internal->cond);

		free(pool);
	}
}

/******************************************************/
void gfx_thread_pool_suspend(

		GFXThreadPool* pool)
{
	struct GFX_Pool* internal = (struct GFX_Pool*)pool;

	/* Tell threads to suspend */
	_gfx_platform_mutex_lock(&internal->mutex);

	internal->status = GFX_INT_POOL_SUSPENDED;

	_gfx_platform_mutex_unlock(&internal->mutex);
}

/******************************************************/
void gfx_thread_pool_resume(

		GFXThreadPool* pool)
{
	struct GFX_Pool* internal = (struct GFX_Pool*)pool;

	/* Tell threads to resume */
	_gfx_platform_mutex_lock(&internal->mutex);

	internal->status = GFX_INT_POOL_RESUMED;
	_gfx_platform_cond_broadcast(&internal->cond);

	_gfx_platform_mutex_unlock(&internal->mutex);
}

/******************************************************/
unsigned char gfx_thread_pool_expand(

		GFXThreadPool*  pool,
		unsigned char   size)
{
	if(USHRT_MAX - size < pool->size)
	{
		/* Overflow error */
		gfx_errors_push(
			GFX_ERROR_OVERFLOW,
			"Overflow occurred during thread pool expansion."
		);
		return 0;
	}

	/* Allocate new node */
	struct GFX_ThreadList* node = (struct GFX_ThreadList*)gfx_list_create(
		sizeof(struct GFX_ThreadList) +
		sizeof(GFX_PlatformThread) * size
	);

	if(!node) return 0;

	/* Initialize all threads */
	struct GFX_Pool* internal = (struct GFX_Pool*)pool;
	unsigned char s;

	for(s = 0; s < size; ++s)
		if(!_gfx_platform_thread_init(
			_gfx_thread_list_get(node, s),
			_gfx_thread_addr,
			internal,
			1))
		{
			break;
		}

	/* Fail if none managed to initialize */
	if(!s)
	{
		gfx_list_free((GFXList*)node);
		return 0;
	}

	/* Add thread list */
	node->size = s;
	pool->size += s;

	if(internal->threads) gfx_list_splice_before(
		(GFXList*)node,
		(GFXList*)internal->threads
	);

	internal->threads = node;

	return s;
}

/******************************************************/
int gfx_thread_pool_push(

		GFXThreadPool*     pool,
		GFXThreadPoolTask  task,
		void*              data,
		char               priority)
{
	struct GFX_Pool* internal = (struct GFX_Pool*)pool;

	/* Create a new task */
	struct GFX_Task elem =
	{
		.priority = priority,
		.data = data,
		.task = task
	};

	/* Push it and wake up a singular thread */
	_gfx_platform_mutex_lock(&internal->mutex);

	int success = _gfx_thread_pool_push(internal, elem);
	if(internal->status == GFX_INT_POOL_RESUMED)
		_gfx_platform_cond_signal(&internal->cond);

	_gfx_platform_mutex_unlock(&internal->mutex);

	return success;
}
