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


/** SubMesh buffer */
typedef unsigned int GFXSubMeshBuffer;


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
GFXSubMeshBuffer gfx_submesh_add_buffer(

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

		GFXSubMesh*       mesh,
		unsigned int      index,
		GFXSubMeshBuffer  buffer,
		size_t            offset);

/**
 * Uses an index buffer for the submesh.
 *
 * @param buffer Buffer ID to use for this draw call.
 * @param offset Byte offset within the buffer to start reading at.
 * @return Zero on failure (in which case the index buffer is disabled).
 *
 */
int gfx_submesh_set_index_buffer(

		GFXSubMesh*       mesh,
		GFXSubMeshBuffer  buffer,
		size_t            offset);


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
		unsigned int   level,
		unsigned char  drawCalls,
		unsigned char  sources);

/**
 * Shares a submesh and maps it to a given level of detail.
 *
 * @param level Level of detail to map to (must be <= mesh->levels).
 * @param share Submesh to share.
 * @return Zero on failure.
 *
 * Note: A submesh can be added multiple times to the same mesh.
 *
 */
int gfx_mesh_add_share(

		GFXMesh*      mesh,
		unsigned int  level,
		GFXSubMesh*   share);

/**
 * Sets the material index to use for all instances of a submesh added to a mesh.
 *
 * @param material Index of the property map within a material level of detail to use.
 * @return Number of found submeshes.
 *
 * Note: index is clamped to the number of property maps available when used.
 *
 * Changing this while the submesh is already in use by a batch can be expensive!
 *
 */
unsigned int gfx_mesh_set_material(

		GFXMesh*      mesh,
		unsigned int  level,
		GFXSubMesh*   sub,
		unsigned int  material);

/**
 * Sets the material index to use for a submesh added to a mesh.
 *
 * @param index Index of the submesh within the level as seen in the return of gfx_mesh_get.
 * @return Zero if the submesh does not exist.
 *
 */
int gfx_mesh_set_material_at(

		GFXMesh*      mesh,
		unsigned int  level,
		unsigned int  index,
		unsigned int  material);

/**
 * Sets the source to sample from all instances of a submesh added to a mesh.
 *
 * @return Number of found submeshes (0 if source >= sub->sources).
 *
 * Changing this while the submesh is already in use by a batch can be expensive!
 *
 */
unsigned int gfx_mesh_set_source(

		GFXMesh*       mesh,
		unsigned int   level,
		GFXSubMesh*    sub,
		unsigned char  source);

/**
 * Sets the source to sample from a submesh added to a mesh.
 *
 * @return Zero if the submesh does not exist (or if source >= sources of the submesh).
 *
 */
int gfx_mesh_set_source_at(

		GFXMesh*       mesh,
		unsigned int   level,
		unsigned int   index,
		unsigned char  source);

/**
 * Returns an abstract list of submeshes of a given level of detail.
 *
 * @param num Returns the number of submeshes in the returned list.
 * @return List of submeshes.
 *
 * Note: as soon as a submesh is added the list is invalidated.
 *
 */
GFXSubMeshList gfx_mesh_get(

		GFXMesh*       mesh,
		unsigned int   level,
		unsigned int*  num);

/**
 * Returns an abstract list of submeshes of all levels.
 *
 * Note: as soon as a submesh is added the list is invalidated.
 *
 */
GFXSubMeshList gfx_mesh_get_all(

		GFXMesh*       mesh,
		unsigned int*  num);

/**
 * Index into a list of submeshes, retrieving the material index.
 *
 * @param list List of submeshes returned by gfx_mesh_get or gfx_mesh_get_all.
 *
 * Note: you can only retrieve a submesh with index < number of elements in list.
 *
 */
unsigned int gfx_submesh_list_material_at(

		GFXSubMeshList  list,
		unsigned int    index);

/**
 * Index into a list of submeshes, retrieving the mesh source.
 *
 */
unsigned char gfx_submesh_list_source_at(

		GFXSubMeshList  list,
		unsigned int    index);

/**
 * Index into a list of submeshes, retrieving the submesh.
 *
 */
GFXSubMesh* gfx_submesh_list_at(

		GFXSubMeshList  list,
		unsigned int    index);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_MESH_H
