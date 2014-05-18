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
 * Batcher (Handles units of submesh/material pairs)
 *******************************************************/

typedef struct GFXBatcher
{
	GFXMaterial*  material;
	GFXSubMesh*   submesh;

} GFXBatcher;


/**
 * References a bucket at a batcher, created if it doesn't exist yet.
 *
 * @param material Material of the batcher to reference.
 * @param submesh  Submesh of the batcher to reference.
 * @param bucket   Pipe to reference, must be a bucket pipe.
 * @return Referenced batcher, NULL on failure.
 *
 * Note: if the bucket is freed, the batcher is automatically dereferenced.
 * If either the material or submesh is freed before the batch, undefined behaviour is expected.
 *
 */
GFXBatcher* gfx_batcher_reference(

		GFXMaterial*  material,
		GFXSubMesh*   submesh,
		GFXPipe*      bucket);

/**
 * References a bucket at an existing, already found batcher.
 *
 * @param batcher Batcher to reference.
 * @param bucket  Pipe to reference, must be a bucket pipe.
 * @return Zero on failure.
 *
 * Note: if the bucket is freed, the batcher is automatically dereferenced.
 *
 */
int gfx_batcher_reference_direct(

		GFXBatcher*  batcher,
		GFXPipe*     bucket);

/**
 * Dereferences a batcher, freed if not referenced by anyone anymore.
 *
 * @param batcher Batcher to dereference.
 * @param bucket  Bucket at the batcher to dereference.
 *
 * Note: if freed, batcher will be invalidated as pointer.
 *
 */
void gfx_batcher_dereference(

		GFXBatcher*  batch,
		GFXPipe*     bucket);

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
int gfx_batcher_increase(

		GFXBatcher*    batcher,
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
size_t gfx_batcher_get(

		GFXBatcher*    batcher,
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
int gfx_batcher_decrease(

		GFXBatcher*    batcher,
		GFXPipe*       bucket,
		size_t         index,
		unsigned char  source,
		size_t         instances);

/**
 * Increase the number of instances to be visible within a bucket.
 *
 * @param bucket  Bucket to increase at.
 * @param level   Level of detail of the material to change the visibility of.
 * @param visible Number of instances to make visible, result is clamped to existing instances.
 * @return Zero if the instances do not exist.
 *
 */
int gfx_batcher_increase_visible(

		GFXBatcher*    batcher,
		GFXPipe*       bucket,
		size_t         level,
		size_t         index,
		unsigned char  source,
		size_t         visible);

/**
 * Decrease the number of instances to be visible within a bucket.
 *
 * @param bucket  Bucket to decrease at.
 * @param visible Number of instances to make invisible, result is clamped to 0.
 * @return Zero if the instances do not exist.
 *
 */
int gfx_batcher_decrease_visible(

		GFXBatcher*    batcher,
		GFXPipe*       bucket,
		size_t         level,
		size_t         index,
		unsigned char  source,
		size_t         visible);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_BATCHER_H
