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
 * @return Non-zero on success (it will not touch batch on failure).
 *
 * Note: A batch with equal materials and meshes are themselves equal.
 *
 */
int gfx_batch_get(

		GFXBatch*     batch,
		GFXMaterial*  material,
		GFXMesh*      mesh);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_BATCH_H
