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

#ifndef GFX_CORE_PIPELINE_H
#define GFX_CORE_PIPELINE_H

#include "groufix/core/shading.h"
#include "groufix/core/window.h"

/* Maximum bits used for batch states */
#define GFX_BATCH_STATE_MAX_BITS  ((sizeof(GFXBatchState) << 3) - 2)

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Bucket metadata
 *******************************************************/

/** Bucket sort flags */
typedef enum GFXBucketFlags
{
	GFX_BUCKET_SORT_PROGRAM        = 0x01,
	GFX_BUCKET_SORT_VERTEX_LAYOUT  = 0x02,
	GFX_BUCKET_SORT_ALL            = 0x03

} GFXBucketFlags;


/** Source to sample from a vertex layout */
typedef struct GFXVertexSource
{
	unsigned char startDraw;
	unsigned char numDraw;

	unsigned char startFeedback;
	unsigned char numFeedback;

} GFXVertexSource;


/********************************************************
 * Buckets to hold batch units
 *******************************************************/

/** Bucket to manage batches */
typedef struct GFXBucket
{
	GFXBucketFlags  flags;
	unsigned char   bits; /* Number of state bits sorted on */

} GFXBucket;


/** Key representing a state */
typedef uint32_t GFXBatchState;


/**
 * Sets the number of bits to sort on.
 *
 * @param bits Number of manual bits to sort by (clamped to [0, GFX_BATCH_STATE_MAX_BITS]).
 *
 */
void gfx_bucket_set_bits(

		GFXBucket*     bucket,
		unsigned char  bits);

/**
 * Adds a new source to the bucket.
 *
 * @param layout Vertex layout to use for this batch, cannot be NULL.
 * @return The ID of the source, 0 on failure.
 *
 */
size_t gfx_bucket_add_source(

		GFXBucket*        bucket,
		GFXVertexLayout*  layout);

/**
 * Set the values of a source (default of everything is 0).
 *
 * @param src Source ID to change the values of.
 * @return Zero when not enough draw calls, non-zero on success.
 *
 */
int gfx_bucket_set_source(

		GFXBucket*       bucket,
		size_t           src,
		GFXVertexSource  values);

/**
 * Removes a source from the bucket.
 *
 * Any units using the source will be erased from the bucket.
 *
 */
void gfx_bucket_remove_source(

		GFXBucket*  bucket,
		size_t      src);

/**
 * Insert a unit to be processed into the bucket.
 *
 * @param state    30 manual bits of the state to associate this unit with (2 MSB bits are ignored).
 * @param map      Property map (and thus program) to use for rendering this unit.
 * @param visible  Non-zero if visible, invisible otherwise.
 * @return The ID of the inserted unit, 0 on failure.
 *
 */
size_t gfx_bucket_insert(

		GFXBucket*       bucket,
		size_t           src,
		GFXPropertyMap*  map,
		GFXBatchState    state,
		int              visible);

/**
 * Returns the index of the copy of the property map in use.
 *
 * @param unit ID from a unit (if it is never inserted or erased, behaviour is undefined).
 *
 */
size_t gfx_bucket_get_copy(

		GFXBucket*  bucket,
		size_t      unit);

/**
 * Returns the number of instances to draw.
 *
 */
size_t gfx_bucket_get_instances(

		GFXBucket*  bucket,
		size_t      unit);

/**
 * Returns the starting instance offset.
 *
 */
unsigned int gfx_bucket_get_instance_base(

		GFXBucket*  bucket,
		size_t      unit);

/**
 * Returns the bits to sort on of the state associated with a unit.
 *
 */
GFXBatchState gfx_bucket_get_state(

		GFXBucket*  bucket,
		size_t      unit);

/**
 * Returns whether a unit is visible or not.
 *
 */
int gfx_bucket_is_visible(

		GFXBucket*  bucket,
		size_t      unit);

/**
 * Sets the index of the copy of the property map to use.
 *
 */
void gfx_bucket_set_copy(

		GFXBucket*  bucket,
		size_t      unit,
		size_t      copy);

/**
 * Sets the number of instances to draw.
 *
 */
void gfx_bucket_set_instances(

		GFXBucket*  bucket,
		size_t      unit,
		size_t      instances);

/**
 * Sets the starting instance offset.
 *
 * Note: requires GFX_EXT_INSTANCED_BASE_ATTRIBUTES for it to work
 * on instanced vertex attributes.
 *
 */
void gfx_bucket_set_instance_base(

		GFXBucket*    bucket,
		size_t        unit,
		unsigned int  base);

/**
 * Sets the bits to sort on of the state to associate a unit with.
 *
 * Note: 2 MSB bits are ignored as they're used internally.
 *
 */
void gfx_bucket_set_state(

		GFXBucket*     bucket,
		size_t         unit,
		GFXBatchState  state);

/**
 * Sets the visibility of a unit.
 *
 * @param visible Non-zero if visible, invisible otherwise.
 *
 */
void gfx_bucket_set_visible(

		GFXBucket*  bucket,
		size_t      unit,
		int         visible);

/**
 * Erases and frees a unit from its bucket.
 *
 */
void gfx_bucket_erase(

		GFXBucket*  bucket,
		size_t      unit);


/********************************************************
 * Process to perform post-processing
 *******************************************************/

/** Process to perform post-processing */
typedef void* GFXPipeProcess;


/**
 * Sets the sources to use while drawing.
 *
 * @param map  Property map (and thus program) to use for the process, NULL will disable the process.
 * @param copy Index of the copy of the property map to use.
 *
 */
void gfx_pipe_process_set_source(

		GFXPipeProcess   process,
		GFXPropertyMap*  map,
		size_t           copy);

/**
 * Sets the target window to render to.
 *
 * @param window Target window to draw to, NULL will enable render to texture.
 * @param swap   Whether to swap the window's internal buffers afterwards or not.
 *
 */
void gfx_pipe_process_set_target(

		GFXPipeProcess  process,
		GFXWindow*      target,
		int             swap);


/********************************************************
 * Pipe metadata
 *******************************************************/

/** Pipe types */
typedef enum GFXPipeType
{
	GFX_PIPE_BUCKET,
	GFX_PIPE_PROCESS

} GFXPipeType;


/** Render state */
typedef enum GFXRenderState
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

} GFXRenderState;


/** Fragment test equation */
typedef enum GFXFragmentTest
{
	GFX_FRAG_NEVER          = 0x0200,
	GFX_FRAG_LESS           = 0x0201,
	GFX_FRAG_LESS_EQUAL     = 0x0203,
	GFX_FRAG_GREATER        = 0x0204,
	GFX_FRAG_GREATER_EQUAL  = 0x0206,
	GFX_FRAG_EQUAL          = 0x0202,
	GFX_FRAG_NOT_EQUAL      = 0x0205,
	GFX_FRAG_ALWAYS         = 0x0207

} GFXFragmentTest;


/** Blend state */
typedef enum GFXBlendState
{
	GFX_BLEND_ADD           = 0x8006,
	GFX_BLEND_SUBTRACT      = 0x800a,
	GFX_BLEND_REV_SUBTRACT  = 0x800b,
	GFX_BLEND_MIN           = 0x8007,
	GFX_BLEND_MAX           = 0x8008

} GFXBlendState;


/** Blend function */
typedef enum GFXBlendFunc
{
	GFX_BLEND_ZERO                    = 0x0000,
	GFX_BLEND_ONE                     = 0x0001,

	GFX_BLEND_SOURCE                  = 0x0300,
	GFX_BLEND_BUFFER                  = 0x0306,
	GFX_BLEND_ONE_MINUS_SOURCE        = 0x0301,
	GFX_BLEND_ONE_MINUS_BUFFER        = 0x0307,

	GFX_BLEND_SOURCE_ALPHA            = 0x0302,
	GFX_BLEND_BUFFER_ALPHA            = 0x0304,
	GFX_BLEND_ONE_MINUS_SOURCE_ALPHA  = 0x0303,
	GFX_BLEND_ONE_MINUS_BUFFER_ALPHA  = 0x0305,

	GFX_BLEND_ALPHA_SATURATE          = 0x0308

} GFXBlendFunc;


/** Stencil function */
typedef enum GFXStencilFunc
{
	GFX_STENCIL_KEEP           = 0x1e00,
	GFX_STENCIL_ZERO           = 0x0000,
	GFX_STENCIL_REPLACE        = 0x1e01,
	GFX_STENCIL_INCREASE       = 0x1e02,
	GFX_STENCIL_INCREASE_WRAP  = 0x8507,
	GFX_STENCIL_DECREASE       = 0x1e03,
	GFX_STENCIL_DECREASE_WRAP  = 0x8508,
	GFX_STENCIL_INVERT         = 0x150a

} GFXStencilFunc;


/** Pipe state */
typedef struct GFXPipeState
{
	/* Render state */
	struct
	{
		GFXRenderState   state;

	} render;


	/* Depth test */
	struct
	{
		GFXFragmentTest  test;

	} depth;


	/* Blending */
	struct
	{
		GFXBlendState    stateRGB;
		GFXBlendState    stateA;
		GFXBlendFunc     sourceRGB;
		GFXBlendFunc     sourceA;
		GFXBlendFunc     bufferRGB;
		GFXBlendFunc     bufferA;

	} blend;


	/* Stencil test */
	struct
	{
		GFXFragmentTest  testFront;
		GFXFragmentTest  testBack;

		GFXStencilFunc   frontFail;
		GFXStencilFunc   frontDepth;
		GFXStencilFunc   frontPass;
		int              frontRef;
		unsigned int     frontMask;

		GFXStencilFunc   backFail;
		GFXStencilFunc   backDepth;
		GFXStencilFunc   backPass;
		int              backRef;
		unsigned int     backMask;

	} stencil;


} GFXPipeState;


/********************************************************
 * Pipe (processes to transfer between states)
 *******************************************************/

/** Individual pipe */
typedef union GFXPipe
{
	GFXBucket*      bucket;  /* Bucket to be processed */
	GFXPipeProcess  process; /* Process for post-processing */

} GFXPipe;


/** Pipe callback object */
typedef struct GFXPipeCallback
{
	unsigned char  key;
	void*          data;

} GFXPipeCallback;


/** Pipe register callback */
typedef void (*GFXPipeCallbackFunc) (GFXPipe*, GFXPipeCallback*);


/** Pipe callback list */
typedef void* GFXPipeCallbackList;


/**
 * Returns the type of a pipe.
 *
 */
GFXPipeType gfx_pipe_get_type(

		GFXPipe* pipe);

/**
 * Returns the state of a pipe.
 *
 * @return Pointer to a modifiable struct.
 *
 * Note: as soon as the pipe is removed the pointer is invalidated.
 *
 */
GFXPipeState* gfx_pipe_get_state(

		GFXPipe* pipe);

/**
 * Register a new callback object for a pipe, it is issued when the pipe is freed.
 *
 * @param callback Key of the callback plus arbitrary data to store.
 * @param func     Function to be issued when the pipe is freed (can be NULL).
 * @return Zero on failure.
 *
 */
int gfx_pipe_register(

		GFXPipe*             pipe,
		GFXPipeCallback      callback,
		GFXPipeCallbackFunc  func);

/**
 * Unregister a callback object from a pipe.
 *
 * An object can be registered multiple times, in that case it will unregister all of them.
 * Note: unregistering can be a costly operation.
 *
 */
void gfx_pipe_unregister(

		GFXPipe*         pipe,
		GFXPipeCallback  callback);

/**
 * Returns whether a given callback has been registered.
 *
 * @return Non-zero if it was found.
 *
 */
int gfx_pipe_exists(

		GFXPipe*         pipe,
		GFXPipeCallback  callback);

/**
 * Finds and returns an abstract list of callback objects with a given key.
 *
 * @param num Returns the number of callback objects found.
 * @return List of found objects.
 *
 * Note: as soon as a callback object is registered/unregistered the list is invalidated.
 *
 */
GFXPipeCallbackList gfx_pipe_find(

		GFXPipe*       pipe,
		unsigned char  key,
		size_t*        num);

/**
 * Indexes into the list of callback objects.
 *
 * @param list List of callbacks returned by gfx_pipe_find.
 *
 * Note: you can only retrieve a callbacks with index < num returnd by gfx_pipe_find.
 *
 */
GFXPipeCallback* gfx_pipe_at(

		GFXPipeCallbackList  list,
		size_t               index);


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
void gfx_pipeline_free(

		GFXPipeline* pipeline);

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
size_t gfx_pipeline_target(

		GFXPipeline*  pipeline,
		unsigned int  width,
		unsigned int  height,
		size_t        num,
		const char*   indices);

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
int gfx_pipeline_attach(

		GFXPipeline*           pipeline,
		GFXTextureImage        image,
		GFXPipelineAttachment  attach,
		unsigned char          index);

/**
 * Adds a bucket to the pipeline.
 *
 * @param bits Number of manual bits to sort by (clamped to [0, GFX_BATCH_STATE_MAX_BITS]).
 * @return The new pipe (NULL on failure).
 *
 * Note: all state and parameters will be copied from the previous pipe.
 *
 */
GFXPipe* gfx_pipeline_push_bucket(

		GFXPipeline*    pipeline,
		unsigned char   bits,
		GFXBucketFlags  flags);

/**
 * Adds a process to the pipeline.
 *
 * @return The new pipe (NULL on failure).
 *
 * Note: all state and parameters will be copied from the previous pipe.
 *
 */
GFXPipe* gfx_pipeline_push_process(

		GFXPipeline* pipeline);

/**
 * Removes all pipes from the execution list without destroying them.
 *
 * This acts as if gfx_pipeline_unlink was called on each pipe.
 *
 */
void gfx_pipeline_unlink_all(

		GFXPipeline* pipeline);

/**
 * Removes a pipe from the execution list but does not destroy it.
 *
 * Afterwards it can still be moved or swapped.
 *
 */
void gfx_pipeline_unlink(

		GFXPipe* pipe);

/**
 * Moves a pipe to be after a given pipe in execution order.
 *
 * @param after The pipe to be moved after (NULL to move to start).
 *
 */
void gfx_pipeline_move(

		GFXPipe*  pipe,
		GFXPipe*  after);

/**
 * Swaps the position of two pipes in their execution order.
 *
 * Note: if they belong to different pipelines this call is ignored.
 *
 */
void gfx_pipeline_swap(

		GFXPipe*  pipe1,
		GFXPipe*  pipe2);

/**
 * Unlinks all current pipes and moves all given pipes to be executed (in order).
 *
 * @param num   Number of pipes to link.
 * @param pipes Num pipes to link, in order.
 *
 * Note: if the pipes don't belong to the same pipeline, nothing happens.
 *
 */
void gfx_pipeline_relink(

		size_t     num,
		GFXPipe**  pipes);

/**
 * Removes and destroys a pipe from its associated pipeline.
 *
 * The pipe cannot be used anymore after this call.
 *
 */
void gfx_pipeline_remove(

		GFXPipe* pipe);

/**
 * Executes all pipes in order.
 *
 * @param num Number of pipes to execute, 0 to execute all remaining pipes.
 *
 */
void gfx_pipeline_execute(

		GFXPipeline*  pipeline,
		unsigned int  num);


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_PIPELINE_H
