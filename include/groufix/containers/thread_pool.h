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

#ifndef GFX_CONTAINERS_THREAD_POOL_H
#define GFX_CONTAINERS_THREAD_POOL_H

#include "groufix/utils.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Thread pool pattern
 *******************************************************/

/** Thread pool task */
typedef void (*GFXThreadPoolTask) (void*);


/** Thread initialization */
typedef void* (*GFXThreadPoolInit) (void);


/** Thread termination */
typedef void (*GFXThreadPoolTerminate) (void*);


/** Thread pool */
typedef struct GFXThreadPool
{
	unsigned short          size; /* Number of threads */
	GFXThreadPoolInit       init;
	GFXThreadPoolTerminate  terminate;

} GFXThreadPool;


/**
 * Creates a new thread pool.
 *
 * @param size      Number of threads to create.
 * @param init      Initialization function to initialize all threads with (can be NULL).
 * @param terminate Termination function to terminate all threads with (can be NULL).
 * @param suspend   Non-zero if the pool should initialize into a suspended state.
 * @return NULL on failure.
 *
 * What the init function returns gets passed as argument to the terminate function.
 * If no init function was given the argument's value will be NULL.
 *
 */
GFX_API GFXThreadPool* gfx_thread_pool_create(

		unsigned char           size,
		GFXThreadPoolInit       init,
		GFXThreadPoolTerminate  terminate,
		int                     suspend);

/**
 * Makes sure a thread pool is freed properly.
 *
 * Note: this method will block until all threads are done with their current tasks.
 *
 */
GFX_API void gfx_thread_pool_free(

		GFXThreadPool* pool);

/**
 * Suspends the thread pool, blocking any thread from executing any new tasks.
 *
 * Note: the threads will finish their current task before they are blocked.
 *
 */
GFX_API void gfx_thread_pool_suspend(

		GFXThreadPool* pool);

/**
 * Resumes the thread from suspension, allowing all threads to execute tasks again.
 *
 */
GFX_API void gfx_thread_pool_resume(

		GFXThreadPool* pool);

/**
 * Expands a thread pool by a number of threads.
 *
 * @param size Number of threads to add to the pool.
 * @return Actual number of threads created (<= size).
 *
 */
GFX_API unsigned char gfx_thread_pool_expand(

		GFXThreadPool*  pool,
		unsigned char   size);

/**
 * Pushes a new task to the queue of the thread pool.
 *
 * @param task     Function to execute.
 * @param data     Data to pass as argument to task.
 * @param priority Priority of the task, a lower value means higher priority.
 * @return Zero on failure.
 *
 */
GFX_API int gfx_thread_pool_push(

		GFXThreadPool*     pool,
		GFXThreadPoolTask  task,
		void*              data,
		char               priority);


#ifdef __cplusplus
}
#endif

#endif // GFX_CONTAINERS_THREAD_POOL_H
