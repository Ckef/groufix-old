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

/** Batch data upload classification */
typedef enum GFXBatchType
{
	GFX_BATCH_SINGLE_DATA,
	GFX_BATCH_MULTIPLE_DATA,

	GFX_BATCH_DEFAULT = GFX_BATCH_SINGLE_DATA

} GFXBatchType;


/** Batch level of detail parameters */
typedef struct GFXBatchLod
{
	size_t  mesh;     /* Level of detail within the mesh */
	size_t  index;    /* Index of the submesh within the mesh LOD to use */
	size_t  material; /* Level of detail within the material */

} GFXBatchLod;


/********************************************************
 * Batch (render group consisting of a mesh and material)
 *******************************************************/

/** Batch */
typedef struct GFXBatch
{
	GFXMaterial*  material;
	size_t        materialID;
	GFXMesh*      mesh;
	size_t        meshID;

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
 * Returns the type of a given batch.
 *
 * @return GFX_BATCH_DEFAULT on failure.
 *
 */
GFXBatchType gfx_batch_get_type(

		GFXBatch* batch);

/**
 * Returns the number of instances at a bucket for a given batch.
 *
 */
size_t gfx_batch_get_instances(

		GFXBatch*  batch,
		GFXPipe*   bucket);

/**
 * Sets the type of a given batch.
 *
 * Note: the default is GFX_BATCH_DEFAULT.
 *
 */
void gfx_batch_set_type(

		GFXBatch*     batch,
		GFXBatchType  type);

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

		GFXBatch*  batch,
		GFXPipe*   bucket,
		size_t     instances);

/**
 * Decrease the number of instances at a bucket for a given batch.
 *
 * @param bucket    Bucket to decrease instances at.
 * @param instances Number of instances to remove.
 *
 * Note: if the number of instances hits 0, the batch's resources for the bucket are freed.
 *
 */
void gfx_batch_decrease(

		GFXBatch*  batch,
		GFXPipe*   bucket,
		size_t     instances);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_BATCH_H
