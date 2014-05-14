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

#ifndef GFX_SCENE_BATCHER_H
#define GFX_SCENE_BATCHER_H

#include "groufix/scene/material.h"
#include "groufix/scene/mesh.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Batch (Handles draw calls of submesh/material pairs)
 *******************************************************/

typedef struct GFXBatch
{
	GFXMaterial*  material;
	GFXSubMesh*   submesh;

} GFXBatch;


/**
 * References a bucket at a batch, created if it doesn't exist yet.
 *
 * @param material Material of the batch to reference.
 * @param submesh  Submesh of the batch to reference.
 * @param bucket   Pipe to reference, must be a bucket pipe.
 * @return Referenced batch, NULL on failure.
 *
 * Note: if the bucket is freed, the batch is automatically dereferenced.
 * If either the material or submesh is freed before the batch, undefined behaviour is expected.
 *
 */
GFXBatch* gfx_batch_reference(

		GFXMaterial*  material,
		GFXSubMesh*   submesh,
		GFXPipe*      bucket);

/**
 * References a bucket at an existing, already found batch.
 *
 * @param batch  Batch to reference.
 * @param bucket Pipe to reference, must be a bucket pipe.
 * @return Zero on failure.
 *
 * Note: if the bucket is freed, the batch is automatically dereferenced.
 *
 */
int gfx_batch_reference_direct(

		GFXBatch*  batch,
		GFXPipe*   bucket);

/**
 * Dereferences a batch, freed if not referenced by anyone anymore.
 *
 * @param batch  Batch to dereference.
 * @param bucket Bucket at the batch to dereference.
 *
 * Note: if freed, batch will be invalidated as pointer.
 *
 */
void gfx_batch_dereference(

		GFXBatch*  batch,
		GFXPipe*   bucket);

/**
 * Increase the number of instances drawn within a bucket.
 *
 * @param bucket    Bucket to increase at, it must first be referenced.
 * @param index     Index within all material levels to use for the instances.
 * @param source    Index of the submesh source to use for the instances.
 * @param instances Number of instances to add.
 * @return Zero on failure.
 *
 */
int gfx_batch_increase(

		GFXBatch*      batch,
		GFXPipe*       bucket,
		size_t         index,
		unsigned char  source,
		size_t         instances);

/**
 * Returns the number of instances drawn within a bucket.
 *
 * @param bucket Bucket to retrieve the number of instances of, it must be referenced.
 * @return Number of instances, 0 if the bucket is not referenced.
 *
 */
size_t gfx_batch_get(

		GFXBatch*      batch,
		GFXPipe*       bucket,
		size_t         index,
		unsigned char  source);

/**
 * Decrease the number of instances drawn within a bucket.
 *
 * @param bucket    Bucket to decrease at, it must first be referenced.
 * @param instances Number of instances to remove.
 * @return Zero if the instances do not exist.
 *
 */
int gfx_batch_decrease(

		GFXBatch*      batch,
		GFXPipe*       bucket,
		size_t         index,
		unsigned char  source,
		size_t         instances);

/**
 * Sets a number of instances to be visible within a bucket.
 *
 * @param bucket  Bucket to change the visibility of units of.
 * @param level   Level of detail of the material to change the visibility of.
 * @param visible Number of instances to make visible, the rest will be made invisible.
 * @return Zero if the instances do not exist.
 *
 */
int gfx_batch_set_visible(

		GFXBatch*      batch,
		GFXPipe*       bucket,
		size_t         level,
		size_t         index,
		unsigned char  source,
		size_t         visible);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_BATCHER_H
