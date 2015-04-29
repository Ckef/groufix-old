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

#ifndef GFX_CORE_PIPELINE_H
#define GFX_CORE_PIPELINE_H

#include "groufix/core/shading.h"
#include "groufix/core/window.h"

/* Maximum bits used for unit states */
#define GFX_UNIT_STATE_MAX_BITS  ((sizeof(GFXUnitState) << 3) - 2)

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
	unsigned char  drawIndex;
	unsigned char  startFeedback;
	unsigned char  numFeedback;

} GFXVertexSource;


/********************************************************
 * Buckets to hold render units
 *******************************************************/

/** Key representing a state */
typedef uint32_t GFXUnitState;


/** Bucket source */
typedef unsigned int GFXBucketSource;


/** Bucket unit */
typedef unsigned int GFXBucketUnit;


/** Bucket to manage render units */
typedef struct GFXBucket
{
	GFXBucketFlags  flags;
	unsigned char   bits; /* Number of state bits sorted on */

} GFXBucket;


/**
 * Sets the number of bits to sort on.
 *
 * @param bits Number of manual bits to sort by (clamped to [0, GFX_UNIT_STATE_MAX_BITS]).
 *
 */
GFX_API void gfx_bucket_set_bits(

		GFXBucket*     bucket,
		unsigned char  bits);

/**
 * Adds a new source to the bucket.
 *
 * @param layout Vertex layout to use, cannot be NULL.
 * @return The ID of the source, 0 on failure.
 *
 */
GFX_API GFXBucketSource gfx_bucket_add_source(

		GFXBucket*              bucket,
		const GFXVertexLayout*  layout);

/**
 * Set the values of a source (default of everything is 0).
 *
 * @param src Source ID to change the values of.
 * @return Zero when not enough draw calls, non-zero on success.
 *
 */
GFX_API int gfx_bucket_set_source(

		GFXBucket*       bucket,
		GFXBucketSource  src,
		GFXVertexSource  values);

/**
 * Removes a source from the bucket.
 *
 * Any units using the source will be erased from the bucket.
 *
 */
GFX_API void gfx_bucket_remove_source(

		GFXBucket*       bucket,
		GFXBucketSource  src);

/**
 * Insert a unit to be processed into the bucket.
 *
 * @param map      Property map (and thus program) to use for rendering this unit.
 * @param copy     Index of the copy of the property map to use.
 * @param visible  Non-zero if visible, invisible otherwise.
 * @return The ID of the inserted unit, 0 on failure.
 *
 */
GFX_API GFXBucketUnit gfx_bucket_insert(

		GFXBucket*             bucket,
		GFXBucketSource        src,
		const GFXPropertyMap*  map,
		unsigned int           copy,
		int                    visible);

/**
 * Returns the index of the copy of the property map in use.
 *
 */
GFX_API unsigned int gfx_bucket_get_copy(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit);

/**
 * Returns the number of instances to draw.
 *
 */
GFX_API size_t gfx_bucket_get_instances(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit);

/**
 * Returns the starting instance offset.
 *
 */
GFX_API unsigned int gfx_bucket_get_instance_base(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit);

/**
 * Returns the bits to sort on of the state associated with a unit.
 *
 */
GFX_API GFXUnitState gfx_bucket_get_state(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit);

/**
 * Returns whether a unit is visible or not.
 *
 */
GFX_API int gfx_bucket_is_visible(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit);

/**
 * Sets the index of the copy of the property map to use.
 *
 * Note: undefined behaviour if copy is out of bounds of the available copies
 * at the property map!
 *
 */
GFX_API void gfx_bucket_set_copy(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		unsigned int   copy);

/**
 * Sets the number of instances to draw.
 *
 */
GFX_API void gfx_bucket_set_instances(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		size_t         instances);

/**
 * Sets the starting instance offset.
 *
 * Note: requires GFX_EXT_INSTANCED_BASE_ATTRIBUTES for it to work
 * on instanced vertex attributes.
 *
 */
GFX_API void gfx_bucket_set_instance_base(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		unsigned int   base);

/**
 * Sets the bits to sort on of the state to associate a unit with.
 *
 * Note: 2 MSB bits are ignored as they're used internally.
 *
 */
GFX_API void gfx_bucket_set_state(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		GFXUnitState   state);

/**
 * Sets the visibility of a unit.
 *
 * @param visible Non-zero if visible, invisible otherwise.
 *
 */
GFX_API void gfx_bucket_set_visible(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		int            visible);

/**
 * Erases and frees a unit from its bucket.
 *
 */
GFX_API void gfx_bucket_erase(

		GFXBucket*     bucket,
		GFXBucketUnit  unit);


/********************************************************
 * Process to perform post-processing
 *******************************************************/

/** Process to perform post-processing */
typedef void* GFXPipeProcess;


/**
 * Creates and retrieves the property map associated with a pipe process.
 *
 * @param properties Fixed number of property indices associated with this map.
 * @return NULL on failure.
 *
 * Note: this will signal the internal program map to setup its executable pipeline.
 * After this the internal program map cannot be altered anymore.
 * Also, any calls after the first call will ignore properties.
 *
 */
GFX_API GFXPropertyMap* gfx_pipe_process_get_map(

		GFXPipeProcess  process,
		unsigned char   properties);

/**
 * Sets the copy of the associated property map to use while drawing.
 *
 * @param copy Index of the copy of the property map to use.
 *
 * The default is 0.
 *
 */
GFX_API void gfx_pipe_process_set_copy(

		GFXPipeProcess  process,
		unsigned int    copy);

/**
 * Calls gfx_program_map_add for the internal program map of a process.
 *
 */
GFX_API GFXProgram* gfx_pipe_process_add(

		GFXPipeProcess  process,
		GFXShaderStage  stage,
		size_t          instances);

/**
 * Calls gfx_program_map_add_share for the internal program map of a process.
 *
 */
GFX_API int gfx_pipe_process_add_share(

		GFXPipeProcess  process,
		GFXShaderStage  stage,
		GFXProgram*     share);

/**
 * Calls gfx_program_map_get for the internal program map of a process.
 *
 */
GFX_API GFXProgram* gfx_pipe_process_get(

		const GFXPipeProcess  process,
		GFXShaderStage        stage);


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


/**
 * Returns the type of a pipe.
 *
 */
GFX_API GFXPipeType gfx_pipe_get_type(

		const GFXPipe* pipe);

/**
 * Returns the state of a pipe.
 *
 * @return Pointer to a modifiable struct.
 *
 * Note: as soon as the pipe is removed the pointer is invalidated.
 *
 */
GFX_API GFXPipeState* gfx_pipe_get_state(

		GFXPipe* pipe);


/********************************************************
 * Pipelines (execute arbitrary processes in order)
 *******************************************************/

/** Viewport description */
typedef struct GFXViewport
{
	int           x;
	int           y;
	unsigned int  width;
	unsigned int  height;

} GFXViewport;


/** Pipeline attachments */
typedef enum GFXPipelineAttachment
{
	GFX_COLOR_ATTACHMENT          = 0x8ce0,
	GFX_DEPTH_ATTACHMENT          = 0x8d00,
	GFX_DEPTH_STENCIL_ATTACHMENT  = 0x821a,
	GFX_STENCIL_ATTACHMENT        = 0x8d20

} GFXPipelineAttachment;


/** Pipeline */
typedef struct GFXPipeline
{
	GFXViewport viewport; /* Viewport used to render to textures */

} GFXPipeline;


/**
 * Creates a new pipeline.
 *
 * @return NULL on failure.
 *
 */
GFX_API GFXPipeline* gfx_pipeline_create(void);

/**
 * Makes sure the pipeline is freed properly.
 *
 */
GFX_API void gfx_pipeline_free(

		GFXPipeline* pipeline);

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
GFX_API unsigned int gfx_pipeline_target(

		GFXPipeline*  pipeline,
		unsigned int  num,
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
GFX_API int gfx_pipeline_attach(

		GFXPipeline*           pipeline,
		GFXTextureImage        image,
		GFXPipelineAttachment  attach,
		unsigned char          index);

/**
 * Adds a bucket to the pipeline.
 *
 * @param bits Number of manual bits to sort by (clamped to [0, GFX_UNIT_STATE_MAX_BITS]).
 * @return The new pipe (NULL on failure).
 *
 * Note: all state and parameters will be copied from the previous pipe.
 *
 */
GFX_API GFXPipe* gfx_pipeline_push_bucket(

		GFXPipeline*    pipeline,
		unsigned char   bits,
		GFXBucketFlags  flags);

/**
 * Adds a process to the pipeline.
 *
 * @param window Target window to draw to, NULL will enable render to texture.
 * @param swap   Whether to swap the window's internal buffers afterwards or not.
 * @return The new pipe (NULL on failure).
 *
 * Note: all state and parameters will be copied from the previous pipe.
 *
 */
GFX_API GFXPipe* gfx_pipeline_push_process(

		GFXPipeline*  pipeline,
		GFXWindow*    target,
		int           swap);

/**
 * Removes all pipes from the execution list without destroying them.
 *
 * This acts as if gfx_pipeline_unlink was called on each pipe.
 *
 */
GFX_API void gfx_pipeline_unlink_all(

		GFXPipeline* pipeline);

/**
 * Removes a pipe from the execution list but does not destroy it.
 *
 * Afterwards it can still be moved or swapped.
 *
 */
GFX_API void gfx_pipeline_unlink(

		GFXPipe* pipe);

/**
 * Moves a pipe to be after a given pipe in execution order.
 *
 * @param after The pipe to be moved after (NULL to move to start).
 *
 */
GFX_API void gfx_pipeline_move(

		GFXPipe*  pipe,
		GFXPipe*  after);

/**
 * Swaps the position of two pipes in their execution order.
 *
 * Note: if they belong to different pipelines this call is ignored.
 *
 */
GFX_API void gfx_pipeline_swap(

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
GFX_API void gfx_pipeline_relink(

		size_t     num,
		GFXPipe**  pipes);

/**
 * Removes and destroys a pipe from its associated pipeline.
 *
 * The pipe cannot be used anymore after this call.
 *
 */
GFX_API void gfx_pipeline_remove(

		GFXPipe* pipe);

/**
 * Executes all pipes in order.
 *
 * @param num Number of pipes to execute, 0 to execute all remaining pipes.
 *
 */
GFX_API void gfx_pipeline_execute(

		GFXPipeline*  pipeline,
		size_t        num);


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_PIPELINE_H
