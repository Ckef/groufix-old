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


/** Batch level of detail parameters */
typedef struct GFXBatchLod
{
	unsigned int  mesh;     /* Level of detail within the mesh */
	unsigned int  index;    /* Index of the submesh within the mesh LOD to use */
	unsigned int  material; /* Level of detail within the material */

} GFXBatchLod;


/********************************************************
 * Batch (render group consisting of a mesh and material)
 *******************************************************/

/** Batch */
typedef struct GFXBatch
{
	GFXMaterial*  material;
	unsigned int  materialID;
	GFXMesh*      mesh;
	unsigned int  meshID;

} GFXBatch;


/**
 * Fetches a batch associated with a given material and mesh.
 *
 * @param params Level of detail parameters to use.
 * @return Non-zero on success (it will not touch batch on failure).
 *
 * Note: Two batches with equal materials, meshes and lod parameters are equal.
 *
 */
int gfx_batch_get(

		GFXBatch*     batch,
		GFXMaterial*  material,
		GFXMesh*      mesh,
		GFXBatchLod   params);

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
 * Retrieves the property map index as seen in gfx_material_get_all.
 *
 * @return The property map index, out of bounds if it does not exist.
 *
 */
unsigned int gfx_batch_get_property_map(

		GFXBatch* batch);

/**
 * Retrieves the submesh index as seen in gfx_mesh_get_all.
 *
 * @return The submesh index, out of bounds if it does not exist.
 *
 */
unsigned int gfx_batch_get_submesh(

		GFXBatch* batch);

/**
 * Return the level of detail parameters of a batch.
 *
 * @return Level of detail parameters, undefined if the batch does not exist.
 *
 * Note: Two batches are strictly inequal if their parameters differ.
 *
 */
GFXBatchLod gfx_batch_get_lod(

		GFXBatch* batch);

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
 * Note: the default is 0 for both the base and variant.
 *
 */
void gfx_batch_set_state(

		GFXBatch*      batch,
		GFXBatchState  base,
		GFXBatchState  variant);

/**
 * Increase the number of instances at a bucket for a given batch.
 *
 * @param bucket    Bucket to increase instances at.
 * @param instances Number of instances to add.
 * @return Zero on failure.
 *
 * Note: if this batch identifies a submesh or property map that does not exist,
 * it will fail and return zero.
 *
 * This is very expensive relative to simply changing the amount of visible instances!
 *
 */
int gfx_batch_increase(

		GFXBatch*     batch,
		GFXPipe*      bucket,
		unsigned int  instances);

/**
 * Decrease the number of instances at a bucket.
 *
 * @param bucket    Bucket to decrease instances at.
 * @param instances Number of instances to remove.
 * @return Zero if the number of instances hits 0.
 *
 * Note: if zero is returned, the batch's resources for the bucket are freed.
 *
 */
int gfx_batch_decrease(

		GFXBatch*     batch,
		GFXPipe*      bucket,
		unsigned int  instances);

/**
 * Returns the number of instances at a bucket.
 *
 */
unsigned int gfx_batch_get_instances(

		GFXBatch*  batch,
		GFXPipe*   bucket);

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

/**
 * Returns the number of visible instances at a bucket.
 *
 */
unsigned int gfx_batch_get_visible(

		GFXBatch*  batch,
		GFXPipe*   bucket);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_BATCH_H
