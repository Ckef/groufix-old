/**
 * Groufix  :  Graphics Engine produced by Ckef Worx
 * www      :  http://www.ejb.ckef-worx.com
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef GFX_PIPELINE_INTERNAL_H
#define GFX_PIPELINE_INTERNAL_H

#include "groufix/internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * OpenGL State management
 *******************************************************/

/**
 * Sets the state of a context.
 *
 * Note: this assumes the context is current.
 *
 */
void _gfx_states_set(GFXPipeState state, GFX_Extensions* ext);

/**
 * Forces all state fields of a context.
 *
 * This will reset all state fields, regardless of previous states.
 * Note: this assumes the context is current.
 *
 */
void _gfx_states_force_set(GFXPipeState state, GFX_Extensions* ext);


/********************************************************
 * Internal bucket creation & processing
 *******************************************************/

/**
 * Creates a new bucket.
 *
 * @param bits Number of manual bits to consider when sorting (LSB = 1st bit, 0 for all bits).
 * @return NULL on failure.
 *
 */
GFXBucket* _gfx_bucket_create(unsigned char bits, GFXBucketFlags flags);

/**
 * Makes sure the bucket is freed properly.
 *
 */
void _gfx_bucket_free(GFXBucket* bucket);

/**
 * Processes the bucket, calling all batch processes.
 *
 */
void _gfx_bucket_process(GFXBucket* bucket, const GFX_Extensions* ext);


/********************************************************
 * Internal pipe process creation & execution
 *******************************************************/

/**
 * Creates a new process.
 *
 * @param dataSize Bytes of the data to pass to the process.
 * @return NULL on failure.
 *
 */
GFXPipeProcess* _gfx_pipe_process_create(size_t dataSize);

/**
 * Makes sure the pipe process is freed properly.
 *
 */
void _gfx_pipe_process_free(GFXPipeProcess* process);

/**
 * Executes the pipe process.
 *
 * @param pipeline Calling pipeline.
 * @param fallback Window to make active after rendering to the target window.
 *
 */
void _gfx_pipe_process_execute(GFXPipeProcess* process, GFXPipeline* pipeline, GFX_Internal_Window* fallback);


#ifdef __cplusplus
}
#endif

#endif // GFX_PIPELINE_INTERNAL_H
