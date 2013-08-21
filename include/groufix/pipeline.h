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
typedef struct GFXBatchUnit
{
	GFXList  node;
	void*    data;

} GFXBatchUnit;


/********************************************************
 * Buckets to hold batches
 *******************************************************/

/** Process applied to batches */
typedef void (*GFXBatchProcessFunc)(GFXBatchState, GFXBatchUnit*, GFXBatchUnit*);


/** Bucket to mange batches */
typedef struct GFXBucket
{
	GFXBatchProcessFunc  preprocess; /* Process to apply when a batch changes */
	GFXBatchProcessFunc  process;    /* Process to apply when iterating, cannot be NULL */

} GFXBucket;


/**
 * Creates a new bucket.
 *
 * @param bits Number of bits to consider when sorting (LSB = 1st bit).
 * @param process Cannot be NULL.
 * @return NULL on failure.
 *
 */
GFXBucket* gfx_bucket_create(unsigned char bits, GFXBatchProcessFunc process);

/**
 * Makes sure the bucket is freed properly.
 *
 */
void gfx_bucket_free(GFXBucket* bucket);

/**
 * Insert a unit to be processed into the bucket.
 *
 * @param data  Arbitrary data to attach.
 * @param state State to associate this unit with.
 * @return The inserted unit, NULL on failure.
 *
 * Note: this forces the bucket to preprocess.
 *
 */
GFXBatchUnit* gfx_bucket_insert(GFXBucket* bucket, void* data, GFXBatchState state);

/**
 * Returns the state associated with a unit.
 *
 */
GFXBatchState gfx_bucket_get_state(GFXBatchUnit* unit);

/**
 * Sets the state to associate a unit with.
 *
 * Note: this forces the bucket to preprocess.
 *
 */
void gfx_bucket_set_state(GFXBatchUnit* unit, GFXBatchState state);

/**
 * Erases and frees a unit from its bucket.
 *
 * Note: this DOES NOT preprocess the bucket.
 *
 */
void gfx_bucket_erase(GFXBatchUnit* unit);

/**
 * Processes the bucket, calling all batch processes.
 *
 * @return The first element.
 *
 */
void gfx_bucket_process(GFXBucket* bucket);


#ifdef __cplusplus
}
#endif

#endif // GFX_PIPELINE_H
