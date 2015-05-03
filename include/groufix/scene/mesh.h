/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#ifndef GFX_SCENE_MESH_H
#define GFX_SCENE_MESH_H

#include "groufix/core/pipeline.h"
#include "groufix/scene/lod.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Mesh (vertex layouts & buffer data abstraction)
 *******************************************************/

/** Vertex source list */
typedef void* GFXVertexSourceList;


/** Mesh layout */
typedef unsigned int GFXMeshLayout;


/** Mesh buffer */
typedef unsigned int GFXMeshBuffer;


/** Mesh */
typedef struct GFXMesh
{
	GFXLodMap lodMap; /* Super class */

} GFXMesh;


/**
 * Creates a new mesh.
 *
 * @return NULL on failure.
 *
 */
GFX_API GFXMesh* gfx_mesh_create(void);

/**
 * Makes sure the mesh is freed properly.
 *
 */
GFX_API void gfx_mesh_free(

		GFXMesh* mesh);

/**
 * Creates a new vertex layout associated with the mesh.
 *
 * @param drawCalls Fixed number of draw calls associated with this layout.
 * @return ID to identify the layout (0 on failure).
 *
 */
GFX_API GFXMeshLayout gfx_mesh_add_layout(

		GFXMesh*       mesh,
		unsigned char  drawCalls);

/**
 * Retrieves the vertex layout from a mesh.
 *
 * @param layout Vertex layout ID to fetch.
 * @return Associated vertex layout.
 *
 */
GFX_API GFXVertexLayout* gfx_mesh_get_layout(

		const GFXMesh*  mesh,
		GFXMeshLayout   layout);

/**
 * Creates an automatic buffer.
 *
 * @return ID to identify the buffer (0 on failure).
 *
 */
GFX_API GFXMeshBuffer gfx_mesh_add_buffer(

		GFXMesh*     mesh,
		size_t       size,
		const void*  data);

/**
 * Uses a vertex buffer for a given attribue of a layout.
 *
 * @param layout Vertex layout ID to add the buffer to.
 * @param buffer Buffer ID to use for this attribute.
 * @param index  Index of the vertex buffer.
 * @param offset Byte offset within the buffer to start reading at.
 * @param stride Byte offset between consecutive attributes (must be <= GFX_LIM_MAX_VERTEX_STRIDE).
 * @return Zero on failure.
 *
 */
GFX_API int gfx_mesh_set_vertex_buffer(

		const GFXMesh*  mesh,
		GFXMeshLayout   layout,
		GFXMeshBuffer   buffer,
		unsigned int    index,
		size_t          offset,
		size_t          stride);

/**
 * Uses an index buffer for a given layout.
 *
 * @param layout Vertex layout ID to set the buffer for.
 * @param buffer Buffer ID to use for this layout.
 * @param offset Byte offset within the buffer to start reading at.
 *
 */
GFX_API void gfx_mesh_set_index_buffer(

		const GFXMesh*  mesh,
		GFXMeshLayout   layout,
		GFXMeshBuffer   buffer,
		size_t          offset);

/**
 * Creates a new vertex source and appends it to a given level of detail.
 *
 * @param level     Level of detail to map to (must be <= mesh->levels).
 * @param layout    Vertex layout ID to use for the source.
 * @param drawIndex Draw index into the layout to use.
 * @return Zero on failure.
 *
 */
GFX_API int gfx_mesh_add(

		GFXMesh*       mesh,
		unsigned int   level,
		GFXMeshLayout  layout,
		unsigned char  drawIndex);

/**
 * Returns an abstract list of vertex sources of a given level of detail.
 *
 * @param num Returns the number of vertex sources in the returned list.
 * @return List of vertex sources.
 *
 * Note: as soon as a vertex source is added the list is invalidated.
 *
 */
GFX_API GFXVertexSourceList gfx_mesh_get(

		const GFXMesh*  mesh,
		unsigned int    level,
		unsigned int*   num);

/**
 * Returns an abstract list of vertex sources of all levels.
 *
 * Note: as soon as a vertex source is added the list is invalidated.
 *
 */
GFX_API GFXVertexSourceList gfx_mesh_get_all(

		const GFXMesh*  mesh,
		unsigned int*   num);

/**
 * Index into a list of vertex sources, retrieving the associated layout ID.
 *
 * @param list List of vertex sources returned by gfx_mesh_get or gfx_mesh_get_all.
 *
 */
GFX_API GFXMeshLayout gfx_vertex_source_list_layout_at(

		const GFXVertexSourceList  list,
		unsigned int               index);

/**
 * Index into a list of vertex sources, retrieving the draw index.
 *
 */
GFX_API unsigned char gfx_vertex_source_list_at(

		const GFXVertexSourceList  list,
		unsigned int               index);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_MESH_H
