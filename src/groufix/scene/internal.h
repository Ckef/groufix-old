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

#ifndef GFX_SCENE_INTERNAL_H
#define GFX_SCENE_INTERNAL_H

#include "groufix/scene/batch.h"
#include "groufix/containers/vector.h"

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
	GFXVector levels; /* Stores size_t, upper bound of the level */

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
 * Batch references at mesh and material
 *******************************************************/

/**
 * Inserts a new batch at the material.
 *
 * @param mesh Mesh the batch is associated with.
 * @return Material ID, 0 on failure.
 *
 */
size_t _gfx_material_insert_batch(

		GFXMaterial*  material,
		GFXMesh*      mesh);

/**
 * Sets the Mesh ID of a batch at a material.
 *
 * @param materialID Batch ID at the material to set.
 * @param meshID     Mesh ID to set it to.
 *
 * Note: this method should be called when the batch is associated with a new property map.
 * This so the material can internally change a reference to said property map.
 *
 */
void _gfx_material_set_batch(

		GFXMaterial*  material,
		size_t        materialID,
		size_t        meshID);

/**
 * Removes a batch at a material.
 *
 * @param materialID Batch ID at the material to remove.
 *
 * Note: _gfx_mesh_remove_batch must be called first.
 *
 */
void _gfx_material_remove_batch(

		GFXMaterial*  material,
		size_t        materialID);

/**
 * Returns the index of the associated property map as seen in gfx_material_get_all.
 *
 * @return Property map index, out of bounds if it has no associated property map.
 *
 */
size_t _gfx_material_get_batch_map(

		GFXMaterial*  material,
		size_t        materialID);

/**
 * Fetches the ID of a batch at a mesh.
 *
 * @param material   Material the batch is associated with.
 * @param params     Level of detail parameters to use.
 * @param materialID Returns the batch ID at the material, 0 if it does not exist.
 * @return Mesh ID if found, 0 on failure.
 *
 */
size_t _gfx_mesh_get_batch(

		GFXMesh*      mesh,
		GFXMaterial*  material,
		GFXBatchLod   params,
		size_t*       materialID);

/**
 * Sets the Material ID of a batch at a mesh.
 *
 * @param meshID     Batch ID at the mesh to set.
 * @param materialID Material ID to set it to.
 *
 */
void _gfx_mesh_set_batch(

		GFXMesh*  mesh,
		size_t    meshID,
		size_t    materialID);

/**
 * Removes a batch at a mesh.
 *
 * @param meshID Batch ID at the mesh to remove.
 *
 * Note: this must be called before _gfx_material_remove_batch.
 *
 */
void _gfx_mesh_remove_batch(

		GFXMesh*  mesh,
		size_t    meshID);

/**
 * Return the level of detail parameters of a batch at a mesh.
 *
 * @param params Returns the level of detail params, not touched if batch does not exist.
 * @return Zero if the batch does not exist.
 *
 */
int _gfx_mesh_get_batch_lod(

		GFXMesh*      mesh,
		size_t        meshID,
		GFXBatchLod*  params);

/**
 * Returns the type of a batch at a mesh.
 *
 * @return GFX_BATCH_DEFAULT if the batch does not exist.
 *
 */
GFXBatchType _gfx_mesh_get_batch_type(

		GFXMesh*  mesh,
		size_t    meshID);

/**
 * Sets the type of a batch at a mesh.
 *
 */
void _gfx_mesh_set_batch_type(

		GFXMesh*      mesh,
		size_t        meshID,
		GFXBatchType  type);


/********************************************************
 * Bucket unit management of batches at meshes
 *******************************************************/

/**
 * Fetch a bucket handle for a given batch and bucket.
 *
 * @param pipe Bucket to fetch a handle of.
 * @return The handle of the bucket, invalid on failure.
 *
 * Note: if the batch does not have an associated submesh, the handle fails to be created.
 * A handle is only valid as long as no other handle at the mesh is altered.
 *
 */
size_t _gfx_mesh_get_bucket(

		GFXMesh*  mesh,
		size_t    meshID,
		GFXPipe*  pipe);

/**
 * Fetch an existing bucket handle for a given batch and bucket.
 *
 * @return The handle of the bucket, invalid on failure.
 *
 * Note: if the batch does not have an associated submesh, the function fails.
 *
 */
size_t _gfx_mesh_find_bucket(

		GFXMesh*  mesh,
		size_t    meshID,
		GFXPipe*  pipe);

/**
 * Forcefully remove a bucket handle associated with a batch for a given bucket.
 *
 * This will destroy all reserved units.
 *
 */
void _gfx_mesh_remove_bucket(

		GFXMesh*  mesh,
		size_t    meshID,
		GFXPipe*  pipe);

/**
 * Increase the instance counter of a bucket handle.
 *
 * @param bucket The handle of the bucket at the mesh.
 * @return Zero on overflow or if the handle is invalid.
 *
 */
int _gfx_mesh_increase(

		GFXMesh*  mesh,
		size_t    bucket,
		size_t    instances);

/**
 * Decreases the instance counter of a bucket handle.
 *
 * @return Zero if the counter hits 0.
 *
 */
int _gfx_mesh_decrease(

		GFXMesh*  mesh,
		size_t    bucket,
		size_t    instances);

/**
 * Returns the instance counter of a bucket handle.
 *
 */
size_t _gfx_mesh_get(

		GFXMesh*  mesh,
		size_t    bucket);

/**
 * Sets the visible 'counter'.
 *
 * @return Actual set value (clamped by instances).
 *
 */
size_t _gfx_mesh_set_visible(

		GFXMesh*  mesh,
		size_t    bucket,
		size_t    instances);

/**
 * Gets the visible 'counter'.
 *
 */
size_t _gfx_mesh_get_visible(

		GFXMesh*  mesh,
		size_t    bucket);

/**
 * Reserves a given amount of units associated with a bucket handle.
 *
 * @param units Number of units to reserve.
 * @return Array of size_t elements of size units, NULL on failure.
 *
 * Note: The pointer is only valid as long as no handle at the mesh reserves any units.
 *
 */
size_t* _gfx_mesh_reserve(

		GFXMesh*  mesh,
		size_t    meshID,
		size_t    bucket,
		size_t    units);

/**
 * Returns memory reserved for units associated with a bucket handle.
 *
 * @param units Returns the number of units in the returned array.
 * @return Array of size_t elements, reserved to be units.
 *
 */
size_t* _gfx_mesh_get_reserved(

		GFXMesh*  mesh,
		size_t    bucket,
		size_t*   units);


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
 * Adds the bucket and increases a reference counter referencing the bucket.
 *
 * @return Zero on failure or overflow.
 *
 */
int _gfx_submesh_reference_bucket(

		GFXSubMesh*  mesh,
		GFXPipe*     pipe);

/**
 * Decreases the reference counter, removes the submesh from the bucket if zero.
 *
 */
void _gfx_submesh_dereference_bucket(

		GFXSubMesh*  mesh,
		GFXPipe*     pipe);

/**
 * Retrieves a bucket source ID from a submesh.
 *
 * @param pipe  A bucket referenced in the submesh.
 * @param index Index of the source to retrieve.
 * @return The source ID, 0 on failure.
 *
 */
size_t _gfx_submesh_get_bucket_source(

		GFXSubMesh*    mesh,
		GFXPipe*       pipe,
		unsigned char  index);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_INTERNAL_H
