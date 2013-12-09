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

#include "groufix/containers/list.h"
#include "groufix/memory.h"
#include "groufix/shading.h"
#include "groufix/window.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Units to batch using a state key
 *******************************************************/

/** Key representing a state */
typedef uint64_t GFXBatchState;


/** Unit to batch */
typedef GFXList GFXBatchUnit;


/********************************************************
 * Buckets to hold batches
 *******************************************************/

/** Bucket sort flags */
typedef enum GFXBucketFlags
{
	GFX_BUCKET_SORT_PROGRAM        = 0x01, /* If not set, it assumes the program is the same for each unit */
	GFX_BUCKET_SORT_VERTEX_LAYOUT  = 0x02, /* If not set, it assumes the layout is the same for each unit */
	GFX_BUCKET_SORT_ALL            = 0x03

} GFXBucketFlags;


/** Batch draw flag */
typedef enum GFXBatchMode
{
	GFX_BATCH_DIRECT,
	GFX_BATCH_INDEXED,
	GFX_BATCH_DIRECT_INSTANCED,
	GFX_BATCH_INDEXED_INSTANCED,

} GFXBatchMode;


/** Bucket to manage batches */
typedef struct GFXBucket
{
	GFXBucketFlags  flags;
	unsigned char   bits; /* Number of state bits that can be changed */

} GFXBucket;


/**
 * Insert a unit to be processed into the bucket.
 *
 * @param state   Manual bits of the state to associate this unit with.
 * @param visible Non-zero if visible, invisible otherwise.
 * @return The inserted unit, NULL on failure.
 *
 * Note: this forces the bucket to have to preprocess.
 *
 */
GFXBatchUnit* gfx_bucket_insert(GFXBucket* bucket, GFXBatchState state, unsigned char visible);

/**
 * Sets the sources the draw from for a unit.
 *
 * @param program Program to use for this unit, cannot be NULL.
 * @param layout  Vertex layout to use for this unit, cannot be NULL.
 * @param start   First draw call to issue.
 * @param num     Number of draw calls to issue starting at start.
 *
 * Note: If this call is not called, execution of a pipeline has undefined behaviour!
 *
 */
void gfx_bucket_set_source(GFXBatchUnit* unit, GFXProgram* program, GFXVertexLayout* layout, unsigned char start, unsigned char num);

/**
 * Sets the drawing mode of a unit.
 *
 * @param inst Number of instances to draw (only relevant when mode includes INSTANCED).
 *
 */
void gfx_bucket_set_mode(GFXBatchUnit* unit, GFXBatchMode mode, size_t inst);

/**
 * Returns the manual bits of the state associated with a unit.
 *
 */
GFXBatchState gfx_bucket_get_state(const GFXBatchUnit* unit);

/**
 * Sets the manual bits of the state to associate a unit with.
 *
 * Note: this forces the bucket to have to preprocess.
 *
 */
void gfx_bucket_set_state(GFXBatchUnit* unit, GFXBatchState state);

/**
 * Sets the visibility of a unit.
 *
 * @param visible Non-zero if visible, invisible otherwise.
 *
 * Note: making a unit visible is expensive regardless of its previous visibility.
 *
 */
void gfx_bucket_set_visible(GFXBatchUnit* unit, unsigned char visible);

/**
 * Erases and frees a unit from its bucket.
 *
 * Note: this DOES NOT cause the need to preprocess the bucket.
 *
 */
void gfx_bucket_erase(GFXBatchUnit* unit);


/********************************************************
 * Process to perform post-processing
 *******************************************************/

/** Forward declerate */
struct GFXPipeline;


/** Process to push to a pipeline */
typedef void (*GFXPipeProcessFunc)(struct GFXPipeline*, void*);


/** Process to perform post-processing */
typedef struct GFXPipeProcess
{
	GFXPipeProcessFunc  preprocess;  /* Custom process to perform before post-processing */
	GFXProgram*         program;     /* Program to use for post-processing */
	GFXPipeProcessFunc  postprocess; /* Custom process to perform after post-processing */

} GFXPipeProcess;


/**
 * Returns a pointer to the custom data.
 *
 */
void* gfx_pipe_process_get_data(GFXPipeProcess* process);

/**
 * Sets the target window to render to.
 *
 * @param program Program to use for post processing.
 * @param target  Target window.
 *
 * Note: to disable targeted window rendering either program or target should be NULL.
 *
 */
void gfx_pipe_process_set_target(GFXPipeProcess* process, GFXProgram* program, GFXWindow* target);


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

	GFX_STATE_NO_RASTERIZER  = 0x0008,
	GFX_STATE_DEPTH_WRITE    = 0x0010,
	GFX_STATE_DEPTH_TEST     = 0x0020,
	GFX_STATE_CULL_FRONT     = 0x0040,
	GFX_STATE_CULL_BACK      = 0x0080,
	GFX_STATE_BLEND          = 0x0100,
	GFX_STATE_STENCIL_TEST   = 0x0200,

	GFX_STATE_DEFAULT = GFX_STATE_CULL_BACK

} GFXPipeState;


/** Individual pipe */
typedef union GFXPipe
{
	GFXBucket*       bucket;  /* Bucket to be processed */
	GFXPipeProcess*  process; /* Process for post-processing */

} GFXPipe;


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
 * Specifies what color attachments to draw to.
 *
 * @param indices Array (num length) of color attachment indices to draw to.
 * @return Number of targets actually used.
 *
 * Use negative indices to discard the drawn values.
 * Note: the number of indices must be < GFX_LIM_MAX_COLOR_TARGETS.
 *
 */
size_t gfx_pipeline_target(GFXPipeline* pipeline, size_t num, const char* indices);

/**
 * Adds a bucket to the pipeline.
 *
 * @param bits Number of manual bits to sort by (LSB = 1st bit, 0 for all bits).
 * @return Index of the pipe (0 on failure).
 *
 */
unsigned short gfx_pipeline_push_bucket(GFXPipeline* pipeline, unsigned char bits, GFXBucketFlags flags);

/**
 * Adds a process to the pipeline.
 *
 * @param dataSize Bytes of the data to pass to the process.
 * @return Index of the pipe (0 on failure).
 *
 */
unsigned short gfx_pipeline_push_process(GFXPipeline* pipeline, size_t dataSize);

/**
 * Sets the state of a pipe.
 *
 * @param index Index to set the state of.
 * @return Non-zero if the state could be changed.
 *
 * The default is that of the previous pipe minus clearing bits,
 * the first pipe will have GFX_STATE_DEFAULT as default.
 *
 */
int gfx_pipeline_set_state(GFXPipeline* pipeline, unsigned short index, GFXPipeState state);

/**
 * Returns the data associated with a pipe.
 *
 * @param type  Returns the type of the pipe (can be NULL).
 * @param state Returns the state of the pipe (can be NULL).
 * @param pipe  Returns the pipe itself (can be NULL).
 * @return Non-zero on success.
 *
 */
int gfx_pipeline_get(GFXPipeline* pipeline, unsigned short index, GFXPipeType* type, GFXPipeState* state, GFXPipe* pipe);

/**
 * Removes the last added pipe.
 *
 * @return Index of the removed pipe (0 if no pipes were present).
 *
 */
unsigned short gfx_pipeline_pop(GFXPipeline* pipeline);

/**
 * Executes all pipes in order.
 *
 */
void gfx_pipeline_execute(GFXPipeline* pipeline);


#ifdef __cplusplus
}
#endif

#endif // GFX_PIPELINE_H
