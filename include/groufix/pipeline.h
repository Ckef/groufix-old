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

#ifndef GFX_PIPELINE_H
#define GFX_PIPELINE_H

#include "groufix/containers/list.h"

#include <stdint.h>

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

/** Process applied to batches */
typedef void (*GFXBatchProcessFunc)(GFXBatchState, GFXBatchUnit*, GFXBatchUnit*);


/** Bucket to manage batches */
typedef struct GFXBucket
{
	GFXBatchProcessFunc process;

} GFXBucket;


/**
 * Insert a unit to be processed into the bucket.
 *
 * @param state    State to associate this unit with.
 * @param dataSize Size of extra data to copy and attach.
 * @return The inserted unit, NULL on failure.
 *
 * Note: this forces the bucket to have to preprocess.
 *
 */
GFXBatchUnit* gfx_bucket_insert(GFXBucket* bucket, GFXBatchState state, size_t dataSize, const void* data);

/**
 * Returns the state associated with a unit.
 *
 */
GFXBatchState gfx_bucket_get_state(const GFXBatchUnit* unit);

/**
 * Sets the state to associate a unit with.
 *
 * Note: this forces the bucket to have to preprocess.
 *
 */
void gfx_bucket_set_state(GFXBatchUnit* unit, GFXBatchState state);

/**
 * Returns a pointer to the data with previously requested uploaded data.
 *
 */
const void* gfx_bucket_get_data(const GFXBatchUnit* unit);

/**
 * Erases and frees a unit from its bucket.
 *
 * Note: this DOES NOT cause the need to preprocess the bucket.
 *
 */
void gfx_bucket_erase(GFXBatchUnit* unit);


/********************************************************
 * Pipe (processes to transfer between states)
 *******************************************************/

/** Process to push to a pipeline */
typedef void (*GFXPipeProcessFunc)(void*);


/** Pipe types */
typedef enum GFXPipeType
{
	GFX_PIPE_BUCKET,
	GFX_PIPE_PROCESS

} GFXPipeType;


/** Individual pipe */
typedef union GFXPipe
{
	GFXBucket*   bucket; /* Bucket to be processed */
	void*        data;   /* Data associated with a process */

} GFXPipe;


/********************************************************
 * Pipelines (execute arbitrary processes in order)
 *******************************************************/

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
 * Adds a bucket to the pipeline.
 *
 * @param bits    Number of bits to sort by.
 * @param process Process to apply to batches.
 * @return Index of the pipe (0 on failure).
 *
 */
unsigned short gfx_pipeline_push_bucket(GFXPipeline* pipeline, unsigned char bits, GFXBatchProcessFunc process);

/**
 * Adds a process to the pipeline.
 *
 * @param dataSize Bytes of the data to pass to the process.
 * @return Index of the pipe (0 on failure).
 *
 */
unsigned short gfx_pipeline_push_process(GFXPipeline* pipeline, GFXPipeProcessFunc process, size_t dataSize);

/**
 * Changes a pipe to be a bucket.
 *
 * @param index   Index to change.
 * @param bits    Number of bits to sort by.
 * @param process Process to apply to batches.
 * @return Non-zero if the pipe could be changed.
 *
 */
int gfx_pipeline_set_bucket(GFXPipeline* pipeline, unsigned short index, unsigned char bits, GFXBatchProcessFunc process);

/**
 * Changes a pipe to be a process.
 *
 * @param index    Index to change.
 * @param dataSize Bytes of the data to pass to the process.
 * @return Non-zero if the pipe could be changed.
 *
 */
int gfx_pipeline_set_process(GFXPipeline* pipeline, unsigned short index, GFXPipeProcessFunc process, size_t dataSize);

/**
 * Returns the data associated with a pipe.
 *
 * @param index Index to return.
 * @param type  Returns the type of the pipe.
 * @param pipe  Returns the pipe itself.
 * @return Non-zero on success.
 *
 */
int gfx_pipeline_get(GFXPipeline* pipeline, unsigned short index, GFXPipeType* type, GFXPipe* pipe);

/**
 * Removes the last added pipe.
 *
 * @return Index of the removed pipe (0 if no pipes were present).
 *
 */
unsigned short gfx_pipeline_pop(GFXPipeline* pipeline);


#ifdef __cplusplus
}
#endif

#endif // GFX_PIPELINE_H
