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
void _gfx_bucket_process(GFXBucket* bucket, GFXPipeState state, GFX_Extensions* ext);


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
 * @param active Currently active window.
 *
 */
void _gfx_pipe_process_execute(GFXPipeProcess* process, GFXPipeline* pipeline, GFXPipeState state, GFX_Internal_Window* active);

/**
 * Prepares a window for render to texture.
 *
 * @return Non-zero on success.
 *
 * If a window was never prepared, it would mean it cannot be used in post processing in any way.
 * Note: the given target window should be current.
 *
 */
int _gfx_pipe_process_prepare(GFX_Internal_Window* target);

/**
 * Replaces a specific target with a new one.
 *
 * @param replace Target to be replaced.
 * @param target  New target to replace the old target with.
 *
 */
void _gfx_pipe_process_retarget(GFX_Internal_Window* replace, GFX_Internal_Window* target);

/**
 * Makes sure no pipe process targets the given window anymore, ever.
 *
 * @param target Window to be untargeted.
 * @param last   Non-zero if this is the last window to be untargeted.
 *
 * Note: the given target window should be current.
 *
 */
void _gfx_pipe_process_untarget(GFX_Internal_Window* target, int last);


#ifdef __cplusplus
}
#endif

#endif // GFX_PIPELINE_INTERNAL_H
