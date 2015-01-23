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
GFX_API int gfx_submesh_set_source(

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
GFX_API GFXVertexSource gfx_submesh_get_source(

		GFXSubMesh*    mesh,
		unsigned char  index);

/**
 * References a bucket at a submesh.
 *
 * @param bucket Bucket to reference.
 * @return Zero on overflow.
 *
 * Note: it must be removed equally many times to free all
 * references to a bucket.
 *
 */
GFX_API int gfx_submesh_add_bucket(

		GFXSubMesh*  mesh,
		GFXBucket*   bucket);

/**
 * Fetches the source ID at a bucket of a source at a submesh.
 *
 * @param index Index of the submesh source to fetch.
 * @return Soure ID at the bucket.
 *
 */
GFX_API GFXBucketSource gfx_submesh_get_bucket_source(

		GFXSubMesh*    mesh,
		GFXBucket*     bucket,
		unsigned char  index);

/**
 * Frees a single reference of a bucket at a submesh.
 *
 * @return Non-zero if a reference was freed.
 *
 */
GFX_API int gfx_submesh_remove_bucket(

		GFXSubMesh*  mesh,
		GFXBucket*   bucket);

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
 * Uses a vertex buffer for a given attribue.
 *
 * @param index  Index of the vertex buffer.
 * @param buffer Buffer ID to use for this attribute.
 * @param offset Byte offset within the buffer to start reading at.
 * @param stride Byte offset between consecutive attributes (must be <= GFX_LIM_MAX_VERTEX_STRIDE).
 * @return Zero on failure (in which case the attribute is disabled).
 *
 */
GFX_API int gfx_submesh_set_vertex_buffer(

		GFXSubMesh*       mesh,
		unsigned int      index,
		GFXSubMeshBuffer  buffer,
		size_t            offset,
		size_t            stride);

/**
 * Uses an index buffer for the submesh.
 *
 * @param buffer Buffer ID to use for this draw call.
 * @param offset Byte offset within the buffer to start reading at.
 * @return Zero on failure (in which case the index buffer is disabled).
 *
 */
GFX_API int gfx_submesh_set_index_buffer(

		GFXSubMesh*       mesh,
		GFXSubMeshBuffer  buffer,
		size_t            offset);


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
 * @param index     Index to insert the submesh at.
 * @param drawCalls Fixed number of draw calls associated with the layout.
 * @param sources   Fixed number of sources to create.
 * @return The new submesh on success, NULL on failure.
 *
 * Note: the old submesh is freed if no mesh references it anymore.
 *
 */
GFX_API GFXSubMesh* gfx_mesh_add(

		GFXMesh*       mesh,
		unsigned char  index,
		unsigned char  drawCalls,
		unsigned char  sources);

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
