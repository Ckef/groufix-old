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

#ifndef GFX_SCENE_INTERNAL_H
#define GFX_SCENE_INTERNAL_H

#include "groufix/containers/vector.h"
#include "groufix/scene/batch.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Internal LOD map
 *******************************************************/

/* Internal LOD map */
typedef struct GFX_LodMap
{
	/* Super class */
	GFXLodMap map;

	/* Hidden data */
	GFXVector data;   /* Stores elements of dataSize bytes */
	GFXVector levels; /* Stores unsigned int, upper bound of the level */

} GFX_LodMap;


/**
 * Initializes a LOD map.
 *
 * @param flags    Flags to apply to this LOD map.
 * @param dataSize Size of each mapped object in bytes.
 * @param compSize Starting bytes of elements to use to compare said elements.
 *
 */
void _gfx_lod_map_init(

		GFX_LodMap*  map,
		GFXLodFlags  flags,
		size_t       dataSize,
		size_t       compSize);

/**
 * Clears the content of a LOD map.
 *
 */
void _gfx_lod_map_clear(

		GFX_LodMap* map);


/********************************************************
 * SubMesh management
 *******************************************************/

/**
 * Creates a new submesh.
 *
 * @param drawCalls Fixed number of draw calls associated with the layout.
 * @param sources   Fixed number of sources to create.
 * @return NULL on failure.
 *
 */
GFXSubMesh* _gfx_submesh_create(

		unsigned char  drawCalls,
		unsigned char  sources);

/**
 * References a submesh to postpone its destruction.
 *
 * @return Zero on overflow.
 *
 */
int _gfx_submesh_reference(

		GFXSubMesh* mesh);

/**
 * Makes sure the submesh is freed properly.
 *
 * Decreases the reference counter before freeing,
 * only freeing if the counter hits 0.
 *
 */
void _gfx_submesh_free(

		GFXSubMesh* mesh);

/**
 * Retrieves a bucket source ID from a submesh.
 *
 * @param bucket A bucket in the submesh used by any batch.
 * @param index  Index of the source to retrieve.
 * @return The source ID, 0 on failure.
 *
 */
GFXBucketSource _gfx_submesh_get_bucket_source(

		GFXSubMesh*    mesh,
		GFXBucket*     bucket,
		unsigned char  index);


/********************************************************
 * Batch references at material and submesh
 *******************************************************/

/** Batch metadata */
typedef struct GFX_BatchData
{
	GFXBatchParams  params;
	GFXBatchFlags   flags;
	GFXBatchState   base;
	GFXBatchState   variant;

} GFX_BatchData;


/**
 * Inserts a new batch at the material.
 *
 * @param mesh SubMesh the batch is associated with.
 * @return Material ID, 0 on failure.
 *
 */
unsigned int _gfx_material_insert_batch(

		GFXMaterial*  material,
		GFXSubMesh*   mesh);

/**
 * Sets the SubMesh ID of a batch at a material.
 *
 * @param materialID Batch ID at the material to set.
 * @param submeshID  SubMesh ID to set it to.
 * @return Zero if the calculated property map does not exist.
 *
 * This will also calculate the map index of the batch.
 *
 */
int _gfx_material_set_batch(

		GFXMaterial*  material,
		unsigned int  materialID,
		unsigned int  submeshID);

/**
 * Removes a batch at a material.
 *
 * @param materialID Batch ID at the material to remove.
 *
 * Note: _gfx_submesh_remove_batch must be called first.
 * This call does not remove the copies at the property map!
 *
 */
void _gfx_material_remove_batch(

		GFXMaterial*  material,
		unsigned int  materialID);

/**
 * Fetches the ID of a batch at a submesh.
 *
 * @param material   Material the batch is associated with.
 * @param data       Metadata to use.
 * @param materialID Returns the batch ID at the material, 0 if it does not exist.
 * @return SubMesh ID if found, 0 on failure.
 *
 */
unsigned int _gfx_submesh_get_batch(

		GFXSubMesh*           mesh,
		GFXMaterial*          material,
		const GFX_BatchData*  data,
		unsigned int*         materialID);

/**
 * Sets the Material ID of a batch at a submesh.
 *
 * @param submeshID  Batch ID at the submesh to set.
 * @param materialID Material ID to set it to.
 *
 */
void _gfx_submesh_set_batch(

		GFXSubMesh*   mesh,
		unsigned int  submeshID,
		unsigned int  materialID);

/**
 * Removes a batch at a submesh.
 *
 * @param submeshID Batch ID at the submesh to remove.
 *
 * Note: this must be called before _gfx_material_remove_batch.
 * This call will call gfx_batch_set_instances with 0 at each bucket.
 *
 */
void _gfx_submesh_remove_batch(

		GFXSubMesh*   mesh,
		unsigned int  submeshID);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_INTERNAL_H
