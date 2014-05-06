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
 * @param bucket   Pipe to reference, must be a bucket pipe.
 * @param material Material of the batch to reference.
 * @param submesh  Submesh of the batch to reference.
 * @return Referenced batch, NULL on failure.
 *
 * Note: if the bucket is freed, the batch is automatically dereferenced.
 * If either the material or submesh is freed before the batch, undefined behaviour is expected.
 *
 */
GFXBatch* gfx_batch_reference(

		GFXPipe*      bucket,
		GFXMaterial*  material,
		GFXSubMesh*   submesh);

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


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_BATCHER_H
