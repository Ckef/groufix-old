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

#include "groufix/scene/lod.h"
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
int gfx_submesh_set_source(

		GFXSubMesh*      mesh,
		unsigned char    index,
		GFXVertexSource  source);

/**
 * Returns a source of the submesh.
 *
 * @param index Index of the submesh source.
 * @return The source values, all 0 if it does not exist.
 *
 */
GFXVertexSource gfx_submesh_get_source(

		GFXSubMesh*    mesh,
		unsigned char  index);

/**
 * Creates an automatic buffer.
 *
 * @param target Storage type the buffer is targeted for.
 * @return ID to identify the buffer (0 on failure).
 *
 */
size_t gfx_submesh_add_buffer(

		GFXSubMesh*      mesh,
		GFXBufferTarget  target,
		size_t           size,
		const void*      data);

/**
 * Uses a vertex buffer for a given attribue.
 *
 * @param index  Index of the attribute to set the buffer of.
 * @param buffer Buffer ID to use for this attribute.
 * @param offset Byte offset within the buffer to start reading at.
 * @return Zero on failure (in which case the attribute is disabled).
 *
 */
int gfx_submesh_set_attribute_buffer(

		GFXSubMesh*   mesh,
		unsigned int  index,
		size_t        buffer,
		size_t        offset);

/**
 * Uses an index buffer for the submesh.
 *
 * @param buffer Buffer ID to use for this draw call.
 * @param offset Byte offset within the buffer to start reading at.
 * @return Zero on failure (in which case the index buffer is disabled).
 *
 */
int gfx_submesh_set_index_buffer(

		GFXSubMesh*  mesh,
		size_t       buffer,
		size_t       offset);


/********************************************************
 * Mesh (collection of submeshes)
 *******************************************************/

/** Mesh */
typedef struct GFXMesh
{
	GFXLodMap lodMap; /* Super class */

} GFXMesh;


/** Submesh list */
typedef void* GFXSubMeshList;


/**
 * Creates a new mesh.
 *
 * @return NULL on failure.
 *
 */
GFXMesh* gfx_mesh_create(void);

/**
 * Makes sure the mesh is freed properly.
 *
 */
void gfx_mesh_free(

		GFXMesh* mesh);

/**
 * Creates a new submesh and maps it to a given level of detail.
 *
 * @param level     Level of detail to map to (must be <= mesh->levels).
 * @param drawCalls Fixed number of draw calls associated with the layout.
 * @param sources   Fixed number of sources to create.
 * @return The new submesh on success, NULL on failure.
 *
 */
GFXSubMesh* gfx_mesh_add(

		GFXMesh*       mesh,
		size_t         level,
		unsigned char  drawCalls,
		unsigned char  sources);

/**
 * Shares a submesh and maps it to a given level of detail.
 *
 * @param level Level of detail to map to (must be <= mesh->levels).
 * @param share Submesh to share.
 * @return Zero on failure.
 *
 */
int gfx_mesh_add_share(

		GFXMesh*     mesh,
		size_t       level,
		GFXSubMesh*  share);

/**
 * Removes a submesh from the mesh.
 *
 * This might or might not remove LODs.
 * If this happens, higher levels will fall down one level.
 *
 */
void gfx_mesh_remove(

		GFXMesh*     mesh,
		GFXSubMesh*  sub);

/**
 * Returns an abstract list of submeshes of a given level of detail.
 *
 * @param num Returns the number of submeshes in the returned list.
 * @return List of submeshes.
 *
 * Note: as soon as a submesh is added/removed the list is invalidated.
 *
 */
GFXSubMeshList gfx_mesh_get(

		GFXMesh*  mesh,
		size_t    level,
		size_t*   num);

/**
 * Index into a list of submeshes.
 *
 * @param list List of submeshes returned by gfx_mesh_get.
 *
 * Note: you can only retrieve a submesh with index < num returnd by gfx_mesh_get.
 *
 */
GFXSubMesh* gfx_submesh_list_at(

		GFXSubMeshList  list,
		size_t          index);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_MESH_H
