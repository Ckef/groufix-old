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

#ifndef GFX_SCENE_MESH_H
#define GFX_SCENE_MESH_H

#include "groufix/core/pipeline.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * SubMesh (vertex layout & buffer data abstraction)
 *******************************************************/

/** SubMesh */
typedef struct GFXSubMesh
{
	GFXVertexLayout*  layout;
	unsigned char     sources; /* Number of draw sources */

} GFXSubMesh;


/**
 * Sets a source of the submesh.
 *
 * @param index Index of the submesh source (must be < mesh->sources).
 * @return Zero if the source does not exist.
 *
 */
int gfx_submesh_set_source(GFXSubMesh* mesh, unsigned char index, GFXVertexSource source);

/**
 * Returns a source of the submesh.
 *
 * @param index Index of the submesh source.
 * @return The source values, all 0 if it does not exist.
 *
 */
GFXVertexSource gfx_submesh_get_source(GFXSubMesh* mesh, unsigned char index);


/********************************************************
 * Mesh (collection of submeshes)
 *******************************************************/

/** Mesh */
typedef struct GFXMesh
{
	size_t        num;       /* Number of elements in subMeshes */
	GFXSubMesh**  subMeshes; /* Array containing the referenced subMeshes */

} GFXMesh;


/**
 * Initializes a mesh.
 *
 */
void gfx_mesh_init(GFXMesh* mesh);

/**
 * Initializes a copy of a mesh.
 *
 */
void gfx_mesh_init_copy(GFXMesh* mesh, GFXMesh* src);

/**
 * Clears content of a mesh.
 *
 */
void gfx_mesh_clear(GFXMesh* mesh);

/**
 * Appends a newly created submesh to the end of the mesh.
 *
 * @param drawCalls Fixed number of draw calls associated with the layout.
 * @param sources   Fixed number of sources to create.
 * @return The new submesh on success, NULL on failure.
 *
 */
GFXSubMesh* gfx_mesh_push(GFXMesh* mesh, unsigned char drawCalls, unsigned char sources);

/**
 * Appends a shared submesh to the end of the mesh.
 *
 * @param share Submesh to share.
 * @return Zero on failure.
 *
 */
int gfx_mesh_push_share(GFXMesh* mesh, GFXSubMesh* share);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_MESH_H
