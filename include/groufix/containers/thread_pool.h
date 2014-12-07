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


/** Thread pool */
typedef struct GFXThreadPool
{
	unsigned char size; /* Number of threads */

} GFXThreadPool;


/**
 * Creates a new thread pool.
 *
 * @param size Number of threads to create.
 * @return NULL on failure.
 *
 */
GFX_API GFXThreadPool* gfx_thread_pool_create(

		unsigned char size);

/**
 * Makes sure a thread pool is freed properly.
 *
 */
GFX_API void gfx_thread_pool_free(

		GFXThreadPool* pool);


#ifdef __cplusplus
}
#endif

#endif // GFX_CONTAINERS_THREAD_POOL_H
