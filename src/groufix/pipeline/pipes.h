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

#ifndef GFX_PIPELINE_PIPES_H
#define GFX_PIPELINE_PIPES_H

#include "groufix/pipeline.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Internal bucket creation & processing
 *******************************************************/

/**
 * Creates a new bucket.
 *
 * @param bits    Number of bits to consider when sorting (LSB = 1st bit, 0 for all bits).
 * @param process Function to process batches with, cannot be NULL.
 * @return NULL on failure.
 *
 */
GFXBucket* _gfx_bucket_create(unsigned char bits, GFXBatchProcessFunc process);

/**
 * Makes sure the bucket is freed properly.
 *
 */
void _gfx_bucket_free(GFXBucket* bucket);

/**
 * Processes the bucket, calling all batch processes.
 *
 */
void _gfx_bucket_process(GFXBucket* bucket);


#ifdef __cplusplus
}
#endif

#endif // GFX_PIPES_H
