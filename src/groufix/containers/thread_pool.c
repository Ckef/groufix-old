/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
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
#include "groufix/core/threading.h"

#include <limits.h>
#include <stdlib.h>

/* Thread pool status */
#define GFX_INT_POOL_TERMINATE  0x00
#define GFX_INT_POOL_SUSPENDED  0x01
#define GFX_INT_POOL_RESUMED    0x02

/******************************************************/
/* Forward declarate */
struct GFX_Pool;


/* Actual task */
typedef struct GFX_Task
{
	char               priority;
	void*              data;
	GFXThreadPoolTask  task;

} GFX_Task;


/* Actual thread node */
typedef struct GFX_ThreadList
{
	/* Super class */
	GFXList node;

	/* Hidden data */
	unsigned char     alive; /* Whether or not these threads are still alive */
	struct GFX_Pool*  pool;
	unsigned char     size;  /* Number of threads of this particular node */
	void*             arg;   /* Argument for initialization */

} GFX_ThreadList;


/* Internal thread pool */
typedef struct GFX_Pool
{
	/* Super class */
	GFXThreadPool pool;

	/* Hidden data */
	unsigned char      status;
	GFXVector          tasks;   /* Priority queue storing GFX_Task */

	GFX_ThreadList*    threads; /* All associated threads */
	GFX_ThreadList*    deads;   /* Terminated threads */

	GFX_PlatformMutex  mutex;
	GFX_PlatformCond   assign;  /* Condition that waits for a task */
	GFX_PlatformCond   flush;   /* Condition that waits for a flush to finish */

} GFX_Pool;


/******************************************************/
static inline GFX_PlatformThread* _gfx_thread_list_get(

		GFX_ThreadList*  list,
		unsigned char    index)
{
	return ((GFX_PlatformThread*)(list + 1)) + index;
}

/******************************************************/
static void _gfx_thread_list_join(

		GFX_ThreadList* list)
{
	unsigned char s;
	for(s = 0; s < list->size; ++s)
		_gfx_platform_thread_join(*_gfx_thread_list_get(list, s), NULL);
}

/******************************************************/
static int _gfx_thread_pool_push(

		GFX_Pool*  pool,
		GFX_Task   task)
{
	/* Insert the new element */
	size_t elem = gfx_vector_get_size(&pool->tasks);

	GFX_Task* et = gfx_vector_insert(
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
		GFX_Task* pt = gfx_vector_at(&pool->tasks, parent);

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
static GFX_Task _gfx_thread_pool_pop(

		GFX_Pool* pool)
{
	GFX_Task* et = pool->tasks.begin;
	GFX_Task ret = *et;

	/* Override root and remove element */
	size_t size = gfx_vector_get_size(&pool->tasks) - 1;

	*et = *(GFX_Task*)gfx_vector_at(&pool->tasks, size);
	gfx_vector_erase_at(&pool->tasks, size);

	/* Heapify the root */
	size_t elem = 0;

	while(1)
	{
		GFX_Task* bt = et;
		size_t b = elem;

		/* Get child with largest priority */
		size_t l = (elem << 1) + 1;
		size_t r = (elem << 1) + 2;

		GFX_Task* lt = gfx_vector_at(&pool->tasks, l);
		GFX_Task* rt = gfx_vector_at(&pool->tasks, r);

		if(l < size && lt->priority < bt->priority)
			bt = lt, b = l;
		if(r < size && rt->priority < bt->priority)
			bt = rt, b = r;

		if(b == elem)
			break;

		/* Swap */
		GFX_Task temp = *bt;
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
	GFX_ThreadList* node = (GFX_ThreadList*)arg;
	GFX_Pool* pool = node->pool;

	/* Initialize */
	arg = NULL;

	if(pool->pool.init)
		arg = pool->pool.init(node->arg);

	/* Run as long as not terminating */
	_gfx_platform_mutex_lock(&pool->mutex);

	while(
		node->alive &&
		pool->status != GFX_INT_POOL_TERMINATE)
	{
		/* If allowed and available, perform a task */
		if(
			pool->status == GFX_INT_POOL_RESUMED &&
			pool->tasks.begin != pool->tasks.end)
		{
			GFX_Task task = _gfx_thread_pool_pop(pool);

			/* Tell flushing threads that everything is flushed */
			if(pool->tasks.begin == pool->tasks.end)
				_gfx_platform_cond_broadcast(&pool->flush);

			/* Unlock during task */
			_gfx_platform_mutex_unlock(&pool->mutex);

			task.task(task.data);

			_gfx_platform_mutex_lock(&pool->mutex);
		}

		/* If not, block */
		else _gfx_platform_cond_wait(
			&pool->assign,
			&pool->mutex
		);
	}

	_gfx_platform_mutex_unlock(&pool->mutex);

	/* Terminate */
	if(pool->pool.terminate)
		pool->pool.terminate(arg);

	return 0;
}

/******************************************************/
GFXThreadPool* gfx_thread_pool_create(

		GFXThreadPoolInit       init,
		GFXThreadPoolTerminate  terminate,
		int                     suspend)
{
	/* Create a new thread pool */
	GFX_Pool* pool = malloc(sizeof(GFX_Pool));
	if(!pool)
	{
		/* Out of memory error */
		gfx_errors_output(
			"[GFX Out Of Memory]: Thread pool could not be allocated."
		);
		return NULL;
	}

	/* Create mutex */
	if(_gfx_platform_mutex_init(&pool->mutex))
	{
		/* Create condition variable */
		if(_gfx_platform_cond_init(&pool->assign))
		{
			if(_gfx_platform_cond_init(&pool->flush))
			{
				/* Initialize */
				pool->status = suspend ?
					GFX_INT_POOL_SUSPENDED :
					GFX_INT_POOL_RESUMED;

				pool->pool.size = 0;
				pool->pool.init = init;
				pool->pool.terminate = terminate;

				gfx_vector_init(&pool->tasks, sizeof(GFX_Task));

				pool->threads = NULL;
				pool->deads = NULL;

				return (GFXThreadPool*)pool;
			}

			_gfx_platform_cond_clear(&pool->assign);
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
		GFX_Pool* internal = (GFX_Pool*)pool;

		/* Tell threads to terminate */
		_gfx_platform_mutex_lock(&internal->mutex);

		internal->status = GFX_INT_POOL_TERMINATE;
		_gfx_platform_cond_broadcast(&internal->assign);

		_gfx_platform_mutex_unlock(&internal->mutex);

		/* Join all dead threads */
		GFX_ThreadList* node = internal->threads;

		for(
			node = internal->deads;
			node;
			node = (GFX_ThreadList*)node->node.next)
		{
			_gfx_thread_list_join(node);
		}

		/* And join all alive threads */
		for(
			node = internal->threads;
			node;
			node = (GFX_ThreadList*)node->node.next)
		{
			_gfx_thread_list_join(node);
		}

		/* Clear all the things */
		gfx_vector_clear(&internal->tasks);

		gfx_list_free((GFXList*)internal->threads);
		gfx_list_free((GFXList*)internal->deads);

		_gfx_platform_mutex_clear(&internal->mutex);
		_gfx_platform_cond_clear(&internal->assign);
		_gfx_platform_cond_clear(&internal->flush);

		free(pool);
	}
}

/******************************************************/
unsigned char gfx_thread_pool_expand(

		GFXThreadPool*  pool,
		unsigned char   size,
		void*           arg)
{
	GFX_Pool* internal = (GFX_Pool*)pool;

	/* Check for overflow */
	if(USHRT_MAX - size < pool->size)
	{
		gfx_errors_push(
			GFX_ERROR_OVERFLOW,
			"Overflow occurred during thread pool expansion."
		);
		return 0;
	}

	/* Allocate new node */
	if(!size) return 0;

	GFX_ThreadList* node = (GFX_ThreadList*)gfx_list_create(
		sizeof(GFX_ThreadList) +
		sizeof(GFX_PlatformThread) * size
	);

	if(!node) return 0;

	node->alive = 1;
	node->pool = internal;
	node->arg = arg;

	/* Initialize all threads */
	unsigned char s;
	for(s = 0; s < size; ++s)
	{
		if(!_gfx_platform_thread_init(
			_gfx_thread_list_get(node, s),
			_gfx_thread_addr,
			node,
			1))
		{
			break;
		}
	}

	node->size = s;
	pool->size += s;

	/* Fail if none managed to initialize */
	if(!s)
	{
		gfx_list_free((GFXList*)node);
		return 0;
	}

	/* Add thread list */
	if(internal->threads) gfx_list_splice_before(
		(GFXList*)node,
		(GFXList*)internal->threads
	);

	internal->threads = node;

	return s;
}

/******************************************************/
unsigned char gfx_thread_pool_shrink(

		GFXThreadPool*  pool,
		int             join)
{
	unsigned char threads = 0;

	/* Get node to terminate */
	GFX_Pool* internal = (GFX_Pool*)pool;
	GFX_ThreadList* node = internal->threads;

	if(node)
	{
		threads = node->size;
		pool->size -= threads;

		/* Tell threads to terminate */
		_gfx_platform_mutex_lock(&internal->mutex);

		node->alive = 0;
		if(internal->status == GFX_INT_POOL_RESUMED)
			_gfx_platform_cond_broadcast(&internal->assign);

		_gfx_platform_mutex_unlock(&internal->mutex);

		/* Remove the node */
		internal->threads = (GFX_ThreadList*)gfx_list_unsplice(
			(GFXList*)node,
			(GFXList*)node
		);

		if(join)
		{
			/* If asked to join, join then free the node */
			_gfx_thread_list_join(node);
			gfx_list_free((GFXList*)node);
		}

		else
		{
			/* Move node to dead nodes to join later */
			if(internal->deads) gfx_list_splice_before(
				(GFXList*)node,
				(GFXList*)internal->deads
			);

			internal->deads = node;
		}
	}

	return threads;
}

/******************************************************/
int gfx_thread_pool_push(

		GFXThreadPool*     pool,
		GFXThreadPoolTask  task,
		void*              data,
		char               priority)
{
	GFX_Pool* internal = (GFX_Pool*)pool;

	/* Create a new task */
	GFX_Task elem =
	{
		.priority = priority,
		.data = data,
		.task = task
	};

	/* Push it and wake up a singular thread */
	_gfx_platform_mutex_lock(&internal->mutex);

	int success = _gfx_thread_pool_push(internal, elem);
	if(internal->status == GFX_INT_POOL_RESUMED)
		_gfx_platform_cond_signal(&internal->assign);

	_gfx_platform_mutex_unlock(&internal->mutex);

	return success;
}

/******************************************************/
void gfx_thread_pool_suspend(

		GFXThreadPool* pool)
{
	GFX_Pool* internal = (GFX_Pool*)pool;

	/* Tell threads to suspend */
	_gfx_platform_mutex_lock(&internal->mutex);

	internal->status = GFX_INT_POOL_SUSPENDED;

	_gfx_platform_mutex_unlock(&internal->mutex);
}

/******************************************************/
void gfx_thread_pool_resume(

		GFXThreadPool* pool)
{
	GFX_Pool* internal = (GFX_Pool*)pool;

	/* Tell threads to resume */
	_gfx_platform_mutex_lock(&internal->mutex);

	internal->status = GFX_INT_POOL_RESUMED;
	_gfx_platform_cond_broadcast(&internal->assign);

	_gfx_platform_mutex_unlock(&internal->mutex);
}

/******************************************************/
void gfx_thread_pool_flush(

		GFXThreadPool* pool)
{
	GFX_Pool* internal = (GFX_Pool*)pool;

	/* Wait until task queue is empty */
	_gfx_platform_mutex_lock(&internal->mutex);

	while(internal->tasks.begin != internal->tasks.end)
		_gfx_platform_cond_wait(&internal->flush, &internal->mutex);

	_gfx_platform_mutex_unlock(&internal->mutex);
}
