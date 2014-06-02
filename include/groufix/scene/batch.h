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
 * @param index Index of the submesh within a mesh level of detail to use.
 * @return Non-zero on success (it will not touch batch on failure).
 *
 * Note: A batch with equal materials, meshes and indices are themselves equal.
 *
 */
int gfx_batch_get(

		GFXBatch*     batch,
		GFXMaterial*  material,
		GFXMesh*      mesh,
		size_t        index);

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
 * Increase the number of instances at a bucket for a given batch.
 *
 * @param bucket    Bucket to increase instances at.
 * @param instances Number of instances to add.
 * @return Zero on failure or if instances is 0.
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
 */
void gfx_batch_decrease(

		GFXBatch*  batch,
		GFXPipe*   bucket,
		size_t     instances);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_BATCH_H
