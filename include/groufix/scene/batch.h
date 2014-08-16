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

#ifndef GFX_SCENE_BATCH_H
#define GFX_SCENE_BATCH_H

#include "groufix/scene/material.h"
#include "groufix/scene/mesh.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Batch metadata
 *******************************************************/

/** Batch flags */
typedef enum GFXBatchFlags
{
	GFX_BATCH_MULTIPLE_DATA       = 0x01, /* Use increasing copies for units */
	GFX_BATCH_INITIAL_ZERO_STATE  = 0x02

} GFXBatchFlags;


/** Batch parameters */
typedef struct GFXBatchParams
{
	unsigned char  source; /* Source index to use at the submesh */
	unsigned int   level;  /* Level of detail to use at the material */
	unsigned int   index;  /* Index within the level of detail at the material */

} GFXBatchParams;


/********************************************************
 * Batch (defined by a submesh and material)
 *******************************************************/

/** Batch */
typedef struct GFXBatch
{
	GFXMaterial*  material;
	unsigned int  materialID;
	GFXSubMesh*   submesh;
	unsigned int  submeshID;

} GFXBatch;


/**
 * Fetches a batch associated with a given material and submesh.
 *
 * @param params Parameters to use.
 * @return Non-zero on success (it will not touch batch on failure).
 *
 * If the parameters define a property map that does not exist, this call will fail.
 * Note: Two batches with equal materials, submeshes and parameters are equal.
 *
 */
int gfx_batch_get(

		GFXBatch*       batch,
		GFXMaterial*    material,
		GFXSubMesh*     submesh,
		GFXBatchParams  params);

/**
 * Force a batch to erase all its resources.
 *
 * Note: it is not required to make this call, it is simply meant to let the
 * system know the batch is not required to exist any longer.
 *
 */
void gfx_batch_erase(

		GFXBatch* batch);

/**
 * Returns the parameters of a batch.
 *
 * @return parameters, all 0 if the batch does not exist.
 *
 */
GFXBatchParams gfx_batch_get_params(

		GFXBatch* batch);

/**
 * Retrieves the property map index as seen in gfx_material_get_all.
 *
 * @return The property map index, out of bounds if it does not exist.
 *
 */
unsigned int gfx_batch_get_property_map(

		GFXBatch* batch);

/**
 * Retrieves the copies used at a property map by the given batch.
 *
 * @param num The number of copies used, always 1 if GFX_BATCH_MULTIPLE_DATA was not set.
 * @return Index of the first copy used.
 *
 */
unsigned int gfx_batch_get_copies(

		GFXBatch*      batch,
		unsigned int*  num);

/**
 * Returns the flags of a given batch.
 *
 */
GFXBatchFlags gfx_batch_get_flags(

		GFXBatch* batch);

/**
 * Returns the state of a given batch.
 *
 * @param base    Returns the base, it is OR'd with the final variant state of each unit.
 * @param variant Returns the variant, what is added to each consecutive unit in a bucket.
 *
 */
void gfx_batch_get_state(

		GFXBatch*       batch,
		GFXBatchState*  base,
		GFXBatchState*  variant);

/**
 * Sets the flags of a given batch.
 *
 * Note: the default is 0.
 *
 */
void gfx_batch_set_flags(

		GFXBatch*      batch,
		GFXBatchFlags  flags);

/**
 * Sets the state of a given batch.
 *
 * @param base    Is OR'd with the resulting variant state of each unit.
 * @param variant Is added to each consecutive unit, the first is 0 if GFX_BATCH_INITIAL_ZERO_STATE is set.
 *
 * Note: the default is 0 for both the base and variant.
 *
 */
void gfx_batch_set_state(

		GFXBatch*      batch,
		GFXBatchState  base,
		GFXBatchState  variant);

/**
 * Returns the number of instances at a bucket.
 *
 */
unsigned int gfx_batch_get_instances(

		GFXBatch*  batch,
		GFXPipe*   bucket);

/**
 * Returns the number of visible instances at a bucket.
 *
 */
unsigned int gfx_batch_get_visible(

		GFXBatch*  batch,
		GFXPipe*   bucket);

/**
 * Sets the number of instances at a bucket for a given batch.
 *
 * @return Zero on failure.
 *
 * Note: the bucket cannot be freed while any batch has more than 0 instances at it,
 * doing so nonetheless will result in undefined behaviour.
 *
 * This is very expensive relative to simply changing the amount of visible instances!
 *
 */
int gfx_batch_set_instances(

		GFXBatch*     batch,
		GFXPipe*      bucket,
		unsigned int  instances);

/**
 * Sets the number of visible instances to draw at a bucket.
 *
 * @param instances Number of instances to draw.
 * @return Number of instances actually made visible.
 *
 */
unsigned int gfx_batch_set_visible(

		GFXBatch*     batch,
		GFXPipe*      bucket,
		unsigned int  instances);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_BATCH_H
