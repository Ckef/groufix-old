/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
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
typedef void *GFXVertexSourceList;


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
GFX_API GFXMesh *gfx_mesh_create(void);

/**
 * Makes sure the mesh is freed properly.
 *
 */
GFX_API void gfx_mesh_free(

		GFXMesh *mesh);

/**
 * Creates a new vertex layout associated with the mesh.
 *
 * @param sources Fixed number of vertex sources associated with this layout.
 * @return ID to identify the layout at this mesh (0 on failure).
 *
 */
GFX_API GFXMeshLayout gfx_mesh_add_layout(

		GFXMesh       *mesh,
		unsigned char  sources);

/**
 * Shares a vertex layout associated with the mesh.
 *
 * @param share Vertex Layout to share.
 * @return ID to identify the layout at this mesh (0 on failure).
 *
 */
GFX_API GFXMeshLayout gfx_mesh_share_layout(

		GFXMesh          *mesh,
		GFXVertexLayout  *layout);

/**
 * Retrieves the vertex layout from a mesh.
 *
 * @param layout Vertex layout ID to fetch.
 * @return Associated vertex layout.
 *
 */
GFX_API GFXVertexLayout *gfx_mesh_get_layout(

		const GFXMesh  *mesh,
		GFXMeshLayout   layout);

/**
 * Creates an automatic buffer.
 *
 * @return ID to identify the buffer (0 on failure).
 *
 * Note: the buffer will be aligned to GFX_LARGE_INTEGER so it can be used
 * for any index buffer.
 *
 */
GFX_API GFXMeshBuffer gfx_mesh_add_buffer(

		GFXMesh     *mesh,
		size_t       size,
		const void  *data);

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

		const GFXMesh  *mesh,
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
 * Note: offset must be a multiple of the size of the largest index used within layout.
 * If not, using this layout in a batch will fail.
 *
 */
GFX_API void gfx_mesh_set_index_buffer(

		const GFXMesh  *mesh,
		GFXMeshLayout   layout,
		GFXMeshBuffer   buffer,
		size_t          offset);

/**
 * Creates a new vertex source and appends it to a given level of detail.
 *
 * @param level     Level of detail to map to (must be <= mesh->levels).
 * @param layout    Vertex layout ID to use for the source.
 * @param srcIndex  Source index into the layout to use.
 * @param offset    Offset to add to first.
 * @param count     Number of vertices to use.
 * @return Zero on failure.
 *
 */
GFX_API int gfx_mesh_add(

		GFXMesh       *mesh,
		unsigned int   level,
		GFXMeshLayout  layout,
		unsigned char  srcIndex,
		size_t         offset,
		size_t         count);

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

		const GFXMesh  *mesh,
		unsigned int    level,
		unsigned int   *num);

/**
 * Returns an abstract list of vertex sources of all levels.
 *
 * Note: as soon as a vertex source is added the list is invalidated.
 *
 */
GFX_API GFXVertexSourceList gfx_mesh_get_all(

		const GFXMesh  *mesh,
		unsigned int   *num);

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
 * Index into the list of vertex sources, retrieving the offset added to the source.
 *
 */
GFX_API size_t gfx_vertex_source_list_offset_at(

		const GFXVertexSourceList  list,
		unsigned int               index);

/**
 * Index into the list of vertex sources, retrieving the count of vertices used.
 *
 */
GFX_API size_t gfx_vertex_source_list_count_at(

		const GFXVertexSourceList  list,
		unsigned int               index);

/**
 * Index into a list of vertex sources, retrieving the source index.
 *
 */
GFX_API unsigned char gfx_vertex_source_list_at(

		const GFXVertexSourceList  list,
		unsigned int               index);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_MESH_H
