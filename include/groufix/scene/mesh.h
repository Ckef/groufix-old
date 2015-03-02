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

#include "groufix/scene/lod.h"
#include "groufix/core/pipeline.h"

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
 * References a bucket at a mesh.
 *
 * @param bucket Bucket to reference.
 * @return Zero on failure.
 *
 * Note: it must be removed equally many times to free all
 * references to a bucket.
 *
 */
GFX_API int _gfx_mesh_add_bucket(

		GFXMesh*    mesh,
		GFXBucket*  bucket);

/**
 * Fetches the source ID at a bucket of a source at a mesh.
 *
 * @param index Index of the source as seen in gfx_mesh_get_all.
 * @return Source ID at the bucket.
 *
 */
GFX_API GFXBucketSource _gfx_mesh_get_bucket_source(

		GFXMesh*      mesh,
		GFXBucket*    bucket,
		unsigned int  index);

/**
 * Frees a single reference of a bucket at a mesh.
 *
 * @return Non-zero if a reference was freed.
 *
 */
GFX_API int _gfx_mesh_remove_bucket(

		GFXMesh*    mesh,
		GFXBucket*  bucket);

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

		GFXMesh*       mesh,
		GFXMeshLayout  layout);

/**
 * Creates an automatic buffer.
 *
 * @param target Storage type the buffer is targeted for.
 * @return ID to identify the buffer (0 on failure).
 *
 */
GFX_API GFXMeshBuffer gfx_mesh_add_buffer(

		GFXMesh*         mesh,
		GFXBufferTarget  target,
		size_t           size,
		const void*      data);

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

		GFXMesh*       mesh,
		GFXMeshLayout  layout,
		GFXMeshBuffer  buffer,
		unsigned int   index,
		size_t         offset,
		size_t         stride);

/**
 * Uses an index buffer for a given layout.
 *
 * @param layout Vertex layout ID to set the buffer for.
 * @param buffer Buffer ID to use for this layout.
 * @param offset Byte offset within the buffer to start reading at.
 *
 */
GFX_API void gfx_mesh_set_index_buffer(

		GFXMesh*       mesh,
		GFXMeshLayout  layout,
		GFXMeshBuffer  buffer,
		size_t         offset);

/**
 * Creates a new vertex source and appends it to a given level of detail.
 *
 * @param level  Level of detail to map to (must be <= mesh->levels).
 * @param source Vertex source to use.
 * @param layout Vertex layout ID to use for the source.
 * @return Zero on failure.
 *
 */
GFX_API int gfx_mesh_add(

		GFXMesh*         mesh,
		unsigned int     level,
		GFXVertexSource  source,
		GFXMeshLayout    layout);

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

		GFXMesh*       mesh,
		unsigned int   level,
		unsigned int*  num);

/**
 * Returns an abstract list of vertex sources of all levels.
 *
 * Note: as soon as a vertex source is added the list is invalidated.
 *
 */
GFX_API GFXVertexSourceList gfx_mesh_get_all(

		GFXMesh*       mesh,
		unsigned int*  num);

/**
 * Index into a list of vertex sources, retrieving the associated layout ID.
 *
 * @param list List of vertex sources returned by gfx_mesh_get or gfx_mesh_get_all.
 *
 */
GFX_API GFXMeshLayout gfx_vertex_source_list_layout_at(

		GFXVertexSourceList  list,
		unsigned int         index);

/**
 * Index into a list of vertex sources, retrieving the vertex source.
 *
 */
GFX_API GFXVertexSource gfx_vertex_source_list_at(

		GFXVertexSourceList  list,
		unsigned int         index);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_MESH_H
