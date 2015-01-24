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
 * SubMesh (vertex layout & buffer data abstraction)
 *******************************************************/

/** SubMesh */
typedef struct GFXSubMesh
{
	GFXLodMap lodMap; /* Super class */

} GFXSubMesh;


/** Vertex source list */
typedef void* GFXVertexSourceList;


/** SubMesh layout */
typedef unsigned int GFXSubMeshLayout;


/** SubMesh buffer */
typedef unsigned int GFXSubMeshBuffer;


/**
 * References a bucket at a submesh.
 *
 * @param bucket Bucket to reference.
 * @return Zero on failure.
 *
 * Note: it must be removed equally many times to free all
 * references to a bucket.
 *
 */
GFX_API int _gfx_submesh_add_bucket(

		GFXSubMesh*  mesh,
		GFXBucket*   bucket);

/**
 * Fetches the source ID at a bucket of a source at a submesh.
 *
 * @param index Index of the source as seen in gfx_submesh_get_all.
 * @return Source ID at the bucket.
 *
 */
GFX_API GFXBucketSource _gfx_submesh_get_bucket_source(

		GFXSubMesh*   mesh,
		GFXBucket*    bucket,
		unsigned int  index);

/**
 * Frees a single reference of a bucket at a submesh.
 *
 * @return Non-zero if a reference was freed.
 *
 */
GFX_API int _gfx_submesh_remove_bucket(

		GFXSubMesh*  mesh,
		GFXBucket*   bucket);

/**
 * Creates a new vertex layout associated with the submesh.
 *
 * @param drawCalls Fixed number of draw calls associated with this layout.
 * @return ID to identify the layout (0 on failure).
 *
 */
GFX_API GFXSubMeshLayout gfx_submesh_add_layout(

		GFXSubMesh*    mesh,
		unsigned char  drawCalls);

/**
 * Retrieves the vertex layout from a submesh.
 *
 * @param layout Vertex layout ID to fetch.
 * @return Associated vertex layout.
 *
 */
GFX_API GFXVertexLayout* gfx_submesh_get_layout(

		GFXSubMesh*       mesh,
		GFXSubMeshLayout  layout);

/**
 * Creates an automatic buffer.
 *
 * @param target Storage type the buffer is targeted for.
 * @return ID to identify the buffer (0 on failure).
 *
 */
GFX_API GFXSubMeshBuffer gfx_submesh_add_buffer(

		GFXSubMesh*      mesh,
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
GFX_API int gfx_submesh_set_vertex_buffer(

		GFXSubMesh*       mesh,
		GFXSubMeshLayout  layout,
		GFXSubMeshBuffer  buffer,
		unsigned int      index,
		size_t            offset,
		size_t            stride);

/**
 * Uses an index buffer for a given layout.
 *
 * @param layout Vertex layout ID to set the buffer for.
 * @param buffer Buffer ID to use for this layout.
 * @param offset Byte offset within the buffer to start reading at.
 *
 */
GFX_API void gfx_submesh_set_index_buffer(

		GFXSubMesh*       mesh,
		GFXSubMeshLayout  layout,
		GFXSubMeshBuffer  buffer,
		size_t            offset);

/**
 * Creates a new vertex source and maps it to a given level of detail.
 *
 * @param level  Level of detail to map to (must be <= mesh->levels).
 * @param source Vertex source to use.
 * @param layout Vertex layout ID to use for the source.
 * @return Zero on failure.
 *
 */
GFX_API int gfx_submesh_add(

		GFXSubMesh*       mesh,
		unsigned int      level,
		GFXVertexSource   source,
		GFXSubMeshLayout  layout);

/**
 * Returns an abstract list of vertex sources of a given level of detail.
 *
 * @param num Returns the number of vertex sources in the returned list.
 * @return List of vertex sources.
 *
 * Note: as soon as a vertex source is added the list is invalidated.
 *
 */
GFX_API GFXVertexSourceList gfx_submesh_get(

		GFXSubMesh*    mesh,
		unsigned int   level,
		unsigned int*  num);

/**
 * Returns an abstract list of vertex sources of all levels.
 *
 * Note: as soon as a vertex source is added the list is invalidated.
 *
 */
GFX_API GFXVertexSourceList gfx_submesh_get_all(

		GFXSubMesh*    mesh,
		unsigned int*  num);

/**
 * Index into a list of vertex sources, retrieving the associated layout ID.
 *
 * @param list List of vertex sources returned by gfx_submesh_get or gfx_submesh_get_all.
 *
 */
GFX_API GFXSubMeshLayout gfx_vertex_source_list_layout_at(

		GFXVertexSourceList  list,
		unsigned int         index);

/**
 * Index into a list of vertex sources, retrieving the vertex source.
 *
 */
GFX_API GFXVertexSource gfx_vertex_source_list_at(

		GFXVertexSourceList  list,
		unsigned int         index);


/********************************************************
 * Mesh (collection of submeshes)
 *******************************************************/

/** Mesh */
typedef struct GFXMesh
{
	unsigned char subMeshes; /* Number of submeshes */

} GFXMesh;


/**
 * Creates a new mesh.
 *
 * @param subMeshes Fixed number of submeshes associated with this layout
 * @return NULL on failure.
 *
 */
GFX_API GFXMesh* gfx_mesh_create(

		unsigned char subMeshes);

/**
 * Makes sure the mesh is freed properly.
 *
 */
GFX_API void gfx_mesh_free(

		GFXMesh* mesh);

/**
 * Creates a new submesh and replaces the submesh (if any) at a given index.
 *
 * @param index Index to insert the submesh at.
 * @return The new submesh on success, NULL on failure.
 *
 * Note: the old submesh is freed if no mesh references it anymore.
 *
 */
GFX_API GFXSubMesh* gfx_mesh_add(

		GFXMesh*       mesh,
		unsigned char  index);

/**
 * Shares a submesh and replaces the submesh (if any) at a given index.
 *
 * @param share Submesh to share, or NULL to only remove the current submesh.
 * @return Zero on failure.
 *
 * Note: A submesh can be added multiple times to the same mesh.
 *
 */
GFX_API int gfx_mesh_add_share(

		GFXMesh*       mesh,
		unsigned char  index,
		GFXSubMesh*    share);

/**
 * Returns a submesh at a given index.
 *
 * @param index Index to retrieve the submesh of.
 * @return Mapped submesh, can be NULL.
 *
 */
GFX_API GFXSubMesh* gfx_mesh_get(

		GFXMesh*       mesh,
		unsigned char  index);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_MESH_H
