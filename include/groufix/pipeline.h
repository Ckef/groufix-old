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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Units to batch using a state key
 *******************************************************/

/** \brief Key representing a state */
typedef uint64_t GFXBatchState;


/** \brief Unit to batch */
typedef struct GFXBatchUnit
{
	GFXBatchState  state;
	void*          data;

} GFXBatchUnit;


/********************************************************
 * Buckets to hold batches
 *******************************************************/

/** \brief Process applied to batches */
typedef void (*GFXBatchProcessFunc)(GFXBatchState, size_t, GFXBatchUnit*);


/** \brief Bucket to mange batches */
typedef struct GFXBucket
{
	GFXBatchProcessFunc  preprocess; /* Process to apply when the bucket is resorted */
	GFXBatchProcessFunc  process;    /* Process to apply when iterating */

} GFXBucket;


/**
 * \brief Creates a new bucket.
 *
 * \return NULL on failure.
 *
 */
GFXBucket* gfx_bucket_create(unsigned char bits);

/**
 * \brief Makes sure the bucket is freed properly.
 *
 */
void gfx_bucket_free(GFXBucket* bucket);

/**
 * \brief Insert a unit to be processed into the bucket.
 *
 */
void gfx_bucket_insert(GFXBucket* bucket, GFXBatchUnit unit);

/**
 * \brief Updates the state of a unit in the bucket.
 *
 * \param unit  How the unit currently looks.
 * \param state The new state to apply to the unit.
 *
 */
void gfx_bucket_update(GFXBucket* bucket, GFXBatchUnit unit, GFXBatchState state);

/**
 * \brief Erases a unit from the bucket.
 *
 */
void gfx_bucket_erase(GFXBucket* bucket, GFXBatchUnit unit);

/**
 * \brief Processes the bucket, calling all batch processes.
 *
 */
void gfx_bucket_process(GFXBucket* bucket);


#ifdef __cplusplus
}
#endif

#endif // GFX_PIPELINE_H
