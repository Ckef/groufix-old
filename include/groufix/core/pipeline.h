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

#ifndef GFX_PIPELINE_H
#define GFX_PIPELINE_H

#include "groufix/core/shading.h"
#include "groufix/core/window.h"

#define GFX_BUCKET_KEY_WIDTH_DEFAULT  17

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Buckets to hold batch units
 *******************************************************/

/** Bucket sort flags */
typedef enum GFXBucketFlags
{
	GFX_BUCKET_SORT_PROGRAM        = 0x01,
	GFX_BUCKET_SORT_VERTEX_LAYOUT  = 0x02,
	GFX_BUCKET_SORT_ALL            = 0x03

} GFXBucketFlags;


/** Bucket to manage batches */
typedef struct GFXBucket
{
	GFXBucketFlags  flags;
	unsigned char   bits; /* Number of state bits that can be changed */

} GFXBucket;


/** Key representing a state */
typedef uint64_t GFXBatchState;


/**
 * Sets the sorting key width limit (the default is GFX_BUCKET_KEY_WIDTH_DEFAULT).
 *
 * @param bits Number of manual bits to sort by.
 *
 * This limit applies to program/vertex layout sorting keys (separate keys).
 * Note: it can only be changed if no sources are added yet.
 *
 */
void gfx_bucket_set_key_width(GFXBucket* bucket, unsigned char width, unsigned char bits);

/**
 * Adds a new source to the bucket.
 *
 * @param map    Property map (and thus program) to use for this unit, cannot be NULL.
 * @param layout Vertex layout to use for this batch, cannot be NULL.
 * @return The ID of the source, 0 on failure.
 *
 */
size_t gfx_bucket_add_source(GFXBucket* bucket, GFXPropertyMap* map, GFXVertexLayout* layout);

/**
 * Removes a source from the bucket.
 *
 * Any units using the source will be erased from the buket.
 *
 */
void gfx_bucket_remove_source(GFXBucket* bucket, size_t src);

/**
 * Sets the draw calls to issue from the source.
 *
 * @param start First draw call to issue.
 * @param num   Number of draw calls to issue starting at start.
 *
 */
void gfx_bucket_set_draw_calls(GFXBucket* bucket, size_t src, unsigned char start, unsigned char num);

/**
 * Sets the active buffers to use for feedback.
 *
 * @param start First buffer to to use.
 * @param num   Number of buffers to sequentially use.
 *
 */
void gfx_bucket_set_feedback(GFXBucket* bucket, size_t src, unsigned char start, unsigned char num);

/**
 * Insert a unit to be processed into the bucket.
 *
 * @param state    Manual bits of the state to associate this unit with.
 * @param visible  Non-zero if visible, invisible otherwise.
 * @return The ID of the inserted unit, 0 on failure.
 *
 */
size_t gfx_bucket_insert(GFXBucket* bucket, size_t src, GFXBatchState state, int visible);

/**
 * Returns the number of instances to draw.
 *
 * @param unit ID from a unit (if it is never inserted or erased, behaviour is undefined).
 *
 */
size_t gfx_bucket_get_instances(GFXBucket* bucket, size_t unit);

/**
 * Returns the manual bits of the state associated with a unit.
 *
 */
GFXBatchState gfx_bucket_get_state(GFXBucket* bucket, size_t unit);

/**
 * Returns whether a unit is visible or not.
 *
 */
int gfx_bucket_is_visible(GFXBucket* bucket, size_t unit);

/**
 * Sets the number of instances to draw (only active when the batch mode include INSTANCED).
 *
 * Note: if the source mode includes INSTANCED but only 1 instance is drawn, a performance hit might be expected.
 *
 */
void gfx_bucket_set_instances(GFXBucket* bucket, size_t unit, size_t instances);

/**
 * Sets the manual bits of the state to associate a unit with.
 *
 */
void gfx_bucket_set_state(GFXBucket* bucket, size_t unit, GFXBatchState state);

/**
 * Sets the visibility of a unit.
 *
 * @param visible Non-zero if visible, invisible otherwise.
 *
 * Note: making a unit visible is expensive regardless of its previous visibility.
 *
 */
void gfx_bucket_set_visible(GFXBucket* bucket, size_t unit, int visible);

/**
 * Erases and frees a unit from its bucket.
 *
 */
void gfx_bucket_erase(GFXBucket* bucket, size_t unit);


/********************************************************
 * Process to perform post-processing
 *******************************************************/

/** Process to perform post-processing */
typedef void* GFXPipeProcess;


/**
 * Sets the sources to use while drawing.
 *
 * @param map Property Map (and thus program) to use for the process, NULL will disable the process.
 *
 */
void gfx_pipe_process_set_source(GFXPipeProcess process, GFXPropertyMap* map);

/**
 * Sets the target window to render to.
 *
 * @param window Target window to draw to, NULL will enable render to texture.
 * @param swap   Whether to swap the window's internal buffers afterwards or not.
 *
 */
void gfx_pipe_process_set_target(GFXPipeProcess process, GFXWindow* target, int swap);


/********************************************************
 * Pipe (processes to transfer between states)
 *******************************************************/

/** Pipe types */
typedef enum GFXPipeType
{
	GFX_PIPE_BUCKET,
	GFX_PIPE_PROCESS

} GFXPipeType;


/** Pipe state */
typedef enum GFXPipeState
{
	GFX_CLEAR_COLOR          = 0x0001,
	GFX_CLEAR_DEPTH          = 0x0002,
	GFX_CLEAR_STENCIL        = 0x0004,
	GFX_CLEAR_ALL            = 0x0007,

	GFX_STATE_WIREFRAME      = 0x0008, /* Requires GFX_EXT_POLYGON_STATE */
	GFX_STATE_POINTCLOUD     = 0x0010, /* Requires GFX_EXT_POLYGON_STATE */

	GFX_STATE_NO_RASTERIZER  = 0x0020,
	GFX_STATE_DEPTH_WRITE    = 0x0040,
	GFX_STATE_DEPTH_TEST     = 0x0080,
	GFX_STATE_CULL_FRONT     = 0x0100,
	GFX_STATE_CULL_BACK      = 0x0200,
	GFX_STATE_BLEND          = 0x0400,
	GFX_STATE_STENCIL_TEST   = 0x0800,

	GFX_STATE_DEFAULT = GFX_STATE_CULL_BACK

} GFXPipeState;


/** Individual pipe */
typedef union GFXPipe
{
	GFXBucket*      bucket;  /* Bucket to be processed */
	GFXPipeProcess  process; /* Process for post-processing */

} GFXPipe;


/**
 * Returns the type of a pipe.
 *
 */
GFXPipeType gfx_pipe_get_type(GFXPipe* pipe);

/**
 * Returns the state of a pipe.
 *
 * The default is that of the previous pipe minus clearing bits,
 * the first pipe will have GFX_STATE_DEFAULT as default.
 *
 */
GFXPipeState gfx_pipe_get_state(GFXPipe* pipe);

/**
 * Sets the state of a pipe.
 *
 */
void gfx_pipe_set_state(GFXPipe* pipe, GFXPipeState state);


/********************************************************
 * Pipe callback objects (for pipe destruction)
 *******************************************************/

/** Pipe callback object */
typedef struct GFXPipeCallback
{
	unsigned char  key;
	void*          data;

} GFXPipeCallback;


/** Pipe register callback */
typedef void (*GFXPipeCallbackFunc) (GFXPipe*, GFXPipeCallback*);


/**
 * Register a new callback object for a pipe, it is issued when the pipe is freed.
 *
 * @callback Key of the callback plus arbitrary data to store.
 * @return Zero on failure.
 *
 */
int gfx_pipe_register(GFXPipe* pipe, GFXPipeCallback callback, GFXPipeCallbackFunc func);

/**
 * Unregister a callback object from a pipe.
 *
 * An object can be registered multiple times, in that case it will unregister all of them.
 *
 */
void gfx_pipe_unregister(GFXPipe* pipe, GFXPipeCallback callback);


/********************************************************
 * Pipelines (execute arbitrary processes in order)
 *******************************************************/

/** Pipeline attachments */
typedef enum GFXPipelineAttachment
{
	GFX_COLOR_ATTACHMENT          = 0x8ce0,
	GFX_DEPTH_ATTACHMENT          = 0x8d00,
	GFX_DEPTH_STENCIL_ATTACHMENT  = 0x821a

} GFXPipelineAttachment;


/** Pipeline */
typedef struct GFXPipeline
{
	size_t id; /* Hardware Object ID */

} GFXPipeline;


/**
 * Creates a new pipeline.
 *
 * @return NULL on failure.
 *
 */
GFXPipeline* gfx_pipeline_create(void);

/**
 * Makes sure the pipeline is freed properly.
 *
 */
void gfx_pipeline_free(GFXPipeline* pipeline);

/**
 * Specifies what color attachments to draw to.
 *
 * @param width   Width of the drawn area.
 * @param height  Height of the draw area.
 * @param indices Array (num length) of color attachment indices to draw to.
 * @return Number of targets actually used.
 *
 * Use negative indices to discard the drawn values.
 * Note: the number of indices must be < GFX_LIM_MAX_COLOR_TARGETS.
 *
 */
size_t gfx_pipeline_target(GFXPipeline* pipeline, unsigned int width, unsigned int height, size_t num, const char* indices);

/**
 * Attaches a texture image to the pipeline as render target.
 *
 * @param attach Attachment point to attach to.
 * @param index  Index of the attachment point (only relevant for color attachments).
 * @return Non-zero on success.
 *
 * Note: the index must be < GFX_LIM_MAX_COLOR_ATTACHMENTS.
 *
 */
int gfx_pipeline_attach(GFXPipeline* pipeline, GFXTextureImage image, GFXPipelineAttachment attach, unsigned char index);

/**
 * Adds a bucket to the pipeline.
 *
 * @param bits Number of manual bits to sort by.
 * @return The new pipe (NULL on failure).
 *
 */
GFXPipe* gfx_pipeline_push_bucket(GFXPipeline* pipeline, unsigned char bits, GFXBucketFlags flags);

/**
 * Adds a process to the pipeline.
 *
 * @return The new pipe (NULL on failure).
 *
 */
GFXPipe* gfx_pipeline_push_process(GFXPipeline* pipeline);

/**
 * Removes a pipe from its associated pipeline.
 *
 */
void gfx_pipeline_remove(GFXPipe* pipe);

/**
 * Executes all pipes in order.
 *
 */
void gfx_pipeline_execute(GFXPipeline* pipeline);


#ifdef __cplusplus
}
#endif

#endif // GFX_PIPELINE_H
