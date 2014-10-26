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

#ifndef GFX_CORE_PIPELINE_INTERNAL_H
#define GFX_CORE_PIPELINE_INTERNAL_H

#include "groufix/containers/list.h"
#include "groufix/core/renderer.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * OpenGL State management
 *******************************************************/

/**
 * Sets all values of a state to their defaults.
 *
 * @param state Structure who's values will be set to their defaults.
 *
 */
void _gfx_states_set_default(

		GFXPipeState* state);

/**
 * Sets the state of the current context.
 *
 */
void _gfx_states_set(

		GFXPipeState* state,
		GFX_WIND_ARG);

/**
 * Forces all state fields of the current context.
 *
 * This will reset all state fields, regardless of previous states.
 *
 */
void _gfx_states_force_set(

		GFXPipeState* state,
		GFX_WIND_ARG);

/**
 * Sets the viewport size of the current context.
 *
 */
void _gfx_states_set_viewport(

		GFXViewport viewport,
		GFX_WIND_ARG);

/**
 * Sets the pixel pack alignment of the current context.
 *
 */
void _gfx_states_set_pixel_pack_alignment(

		unsigned char align,
		GFX_WIND_ARG);

/**
 * Sets the pixel unpack alignment of the current context.
 *
 */
void _gfx_states_set_pixel_unpack_alignment(

		unsigned char align,
		GFX_WIND_ARG);

/**
 * Sets the number of vertices per patch.
 *
 */
void _gfx_states_set_patch_vertices(

		unsigned int vertices,
		GFX_WIND_ARG);


/********************************************************
 * Internal pipe
 *******************************************************/

/* Internal Pipe */
typedef struct GFX_Pipe
{
	/* Super class */
	GFXList node;

	GFXPipeType    type;
	GFXPipe        ptr;  /* Public pointer */
	GFXPipeState   state;

	/* Associated pipeline */
	GFXPipeline*   pipeline;

} GFX_Pipe;


/**
 * Creates a new bucket pipe.
 *
 * @param bits Number of manual bits to sort by (LSB = 1st bit, 0 for all bits).
 * @return NULL on failure
 *
 */
GFX_Pipe* _gfx_pipe_create_bucket(

		GFXPipeline*    pipeline,
		unsigned char   bits,
		GFXBucketFlags  flags);

/**
 * Creates a new process pipe.
 *
 * @param window Target window to draw to, NULL to render using current pipeline.
 * @param swap   Whether to swap the window's internal buffers afterwards or not.
 * @return NULL on failure.
 *
 */
GFX_Pipe* _gfx_pipe_create_process(

		GFXPipeline*  pipeline,
		GFXWindow*    target,
		int           swap);

/**
 * Makes sure the pipe is freed properly.
 *
 * @return The pipe taking its place (can be NULL).
 *
 * If no pipe takes its place, it will try to return the previous pipe instead.
 *
 */
GFX_Pipe* _gfx_pipe_free(

		GFX_Pipe* pipe);

/**
 * Sets the framebuffer handle associated with a pipeline as current for the current context.
 *
 */
void _gfx_pipeline_bind(

		GLenum  target,
		GLuint  framebuffer,
		GFX_WIND_ARG);


/********************************************************
 * Internal bucket creation & processing
 *******************************************************/

/**
 * Creates a new bucket.
 *
 * @param bits Number of manual bits to sort by (LSB = 1st bit, 0 for all bits).
 * @return NULL on failure.
 *
 */
GFXBucket* _gfx_bucket_create(

		unsigned char   bits,
		GFXBucketFlags  flags);

/**
 * Makes sure the bucket is freed properly.
 *
 */
void _gfx_bucket_free(

		GFXBucket* bucket);

/**
 * Processes the bucket, drawing all batches.
 *
 */
void _gfx_bucket_process(

		GFXBucket*     bucket,
		GFXPipeState*  state,
		GFX_WIND_ARG);


/********************************************************
 * Internal pipe process creation & execution
 *******************************************************/

/**
 * Creates a new process.
 *
 * @param window Target window to draw to, NULL to render using current pipeline.
 * @param swap   Whether to swap the window's internal buffers afterwards or not.
 * @return NULL on failure.
 *
 */
GFXPipeProcess _gfx_pipe_process_create(

		GFXWindow*  target,
		int         swap);

/**
 * Makes sure the pipe process is freed properly.
 *
 */
void _gfx_pipe_process_free(

		GFXPipeProcess process);

/**
 * Prepares the current window for render to texture.
 *
 * @return Non-zero on success.
 *
 * If a window was never prepared, it would mean it cannot be used in post processing in any way.
 *
 */
int _gfx_pipe_process_prepare(void);

/**
 * Makes sure no pipe process targets the current window anymore, ever.
 *
 * @param last Non-zero if this is the last window to be unprepared.
 *
 */
void _gfx_pipe_process_unprepare(

		int last);

/**
 * Makes sure all the pipes that target the current window target a new one.
 *
 * @param target New target for all pipes.
 *
 */
void _gfx_pipe_process_retarget(

		GFX_Window* target);

/**
 * Forwards a new size of a window to all processes.
 *
 * @target Window which was resized.
 * @width  New width.
 * @height New height.
 *
 */
void _gfx_pipe_process_resize(

		GFX_Window*   target,
		unsigned int  width,
		unsigned int  height);

/**
 * Executes the pipe process.
 *
 * @param pipeline Calling pipeline.
 *
 */
void _gfx_pipe_process_execute(

		GFXPipeProcess  process,
		GFXPipeState*   state,
		GFX_WIND_ARG);


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_PIPELINE_INTERNAL_H
