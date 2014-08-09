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

#ifndef GFX_SCENE_PROTOCOL_H
#define GFX_SCENE_PROTOCOL_H

#include "groufix/scene/batch.h"
#include "groufix/scene/internal.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Unit manipulation of batch
 *******************************************************/

/**
 * Sets the copy of all given units according to given flags.
 *
 * @param flags Flags to set the copies accordingly to.
 * @param num   Number of copies in units.
 * @param copy  Starting copy to use.
 *
 */
void _gfx_batch_set_copy(

		GFXBucket*      bucket,
		GFXBatchFlags   flags,
		GFXBucketUnit*  units,
		unsigned int    num,
		unsigned int    copy);


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
unsigned int _gfx_material_insert_batch(

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
		unsigned int  materialID,
		unsigned int  meshID);

/**
 * Removes a batch at a material.
 *
 * @param materialID Batch ID at the material to remove.
 *
 * Note: _gfx_mesh_remove_batch must be called first.
 * This call does not remove the copies at the property map!
 *
 */
void _gfx_material_remove_batch(

		GFXMaterial*  material,
		unsigned int  materialID);

/**
 * Fetches the ID of a batch at a mesh.
 *
 * @param material   Material the batch is associated with.
 * @param params     Level of detail parameters to use.
 * @param materialID Returns the batch ID at the material, 0 if it does not exist.
 * @return Mesh ID if found, 0 on failure.
 *
 */
unsigned int _gfx_mesh_get_batch(

		GFXMesh*       mesh,
		GFXMaterial*   material,
		GFXBatchLod    params,
		unsigned int*  materialID);

/**
 * Sets the Material ID of a batch at a mesh.
 *
 * @param meshID     Batch ID at the mesh to set.
 * @param materialID Material ID to set it to.
 *
 */
void _gfx_mesh_set_batch(

		GFXMesh*      mesh,
		unsigned int  meshID,
		unsigned int  materialID);

/**
 * Removes a batch at a mesh.
 *
 * @param meshID Batch ID at the mesh to remove.
 *
 * Note: this must be called before _gfx_material_remove_batch.
 * This call will call gfx_batch_decrease with UINT_MAX at each bucket.
 *
 */
void _gfx_mesh_remove_batch(

		GFXMesh*      mesh,
		unsigned int  meshID);


/********************************************************
 * Batch properties at material
 *******************************************************/

/**
 * Returns the index of the associated property map as seen in gfx_material_get_all.
 *
 * @return Property map index, out of bounds if it has no associated property map.
 *
 */
unsigned int _gfx_material_get_batch_map(

		GFXMaterial*  material,
		unsigned int  materialID);

/**
 * Returns the first copy used in the associated property map of a batch.
 *
 */
unsigned int _gfx_material_get_batch_copy(

		GFXMaterial*  material,
		unsigned int  materialID);

/**
 * Increase the number of copies used by a given batch.
 *
 * @return Zero on failure or if copies is 0.
 *
 * Note: it moves copy data within the property map and
 * calls _gfx_mesh_set_batch_copy for all altered batches (except the given one).
 *
 */
int _gfx_material_increase_copies(

		GFXMaterial*  material,
		unsigned int  materialID,
		unsigned int  copies);

/**
 * Decrease the number of copies used by a given batch.
 *
 * @return Zero on failure or if copies is 0.
 *
 * Note: it moves copy data within the property map and
 * calls _gfx_mesh_set_batch_copy for all altered batches (except the given one).
 *
 */
int _gfx_material_decrease_copies(

		GFXMaterial*  material,
		unsigned int  materialID,
		unsigned int  copies);


/********************************************************
 * Batch properties at mesh
 *******************************************************/

/**
 * Returns the maximum number of units reserved by any bucket within a batch.
 *
 */
unsigned int _gfx_mesh_get_batch_units(

		GFXMesh*      mesh,
		unsigned int  meshID);

/**
 * Return the level of detail parameters of a batch at a mesh.
 *
 * @param params Returns the level of detail params, not touched if batch does not exist.
 *
 */
void _gfx_mesh_get_batch_lod(

		GFXMesh*      mesh,
		unsigned int  meshID,
		GFXBatchLod*  params);

/**
 * Returns the flags of a batch at a mesh.
 *
 * @return 0 if the batch does not exist.
 *
 */
GFXBatchFlags _gfx_mesh_get_batch_flags(

		GFXMesh*      mesh,
		unsigned int  meshID);

/**
 * Sets the flags of a batch at a mesh.
 *
 * In addition, it will also call _gfx_mesh_set_batch_copy for the given batch.
 *
 */
void _gfx_mesh_set_batch_flags(

		GFXMesh*       mesh,
		unsigned int   meshID,
		GFXBatchFlags  flags,
		unsigned int   copy);

/**
 * Calls _gfx_batch_set_copy for all associated buckets of a batch.
 *
 * This call will set the copy of all units for all buckets.
 *
 * Note: undefined behaviour if the batch does not exist!
 *
 */
void _gfx_mesh_set_batch_copy(

		GFXMesh*      mesh,
		unsigned int  meshID,
		unsigned int  copy);


/********************************************************
 * Bucket unit management of batches at meshes
 *******************************************************/

/**
 * Fetch a bucket handle for a given batch and bucket.
 *
 * @param pipe   Bucket to fetch a handle of.
 * @param handle Handle of the bucket.
 * @return Zero on failure, the handle is untouched.
 *
 * Note: if the batch does not have an associated submesh, the handle fails to be created.
 * A handle is only valid as long as no other handle at the mesh is altered.
 *
 */
int _gfx_mesh_get_bucket(

		GFXMesh*       mesh,
		unsigned int   meshID,
		GFXPipe*       pipe,
		unsigned int*  handle);

/**
 * Fetch an existing bucket handle for a given batch and bucket.
 *
 * @param handle The handle of the bucket.
 * @return Zero on failure, the handle is untouched.
 *
 * Note: if the batch does not have an associated submesh, the function fails.
 *
 */
int _gfx_mesh_find_bucket(

		GFXMesh*       mesh,
		unsigned int   meshID,
		GFXPipe*       pipe,
		unsigned int*  handle);

/**
 * Forcefully remove a bucket handle associated with a batch for a given bucket.
 *
 * This will destroy all reserved units.
 *
 */
void _gfx_mesh_remove_bucket(

		GFXMesh*      mesh,
		unsigned int  meshID,
		GFXPipe*      pipe);

/**
 * Reserves a given amount of units associated with a bucket handle.
 *
 * @param meshID Batch ID at the mesh the bucket is associated with, undefined behaviour if not correct.
 * @param bucket The handle of the bucket at the mesh, undefined behaviour if invalid.
 * @param units  Number of units to reserve.
 * @return Array of size_t elements of size units, NULL on failure.
 *
 * Note: The pointer is only valid as long as no handle at the mesh reserves any units.
 *
 */
GFXBucketUnit* _gfx_mesh_reserve(

		GFXMesh*      mesh,
		unsigned int  meshID,
		unsigned int  bucket,
		unsigned int  units);

/**
 * Returns memory reserved for units associated with a bucket handle.
 *
 * @param units Returns the number of units in the returned array.
 * @return Array of size_t elements, reserved to be units.
 *
 */
GFXBucketUnit* _gfx_mesh_get_reserved(

		GFXMesh*       mesh,
		unsigned int   bucket,
		unsigned int*  units);

/**
 * Increase the instance counter of a bucket handle.
 *
 * @return Zero on overflow.
 *
 */
int _gfx_mesh_increase_instances(

		GFXMesh*      mesh,
		unsigned int  bucket,
		unsigned int  instances);

/**
 * Decreases the instance counter of a bucket handle.
 *
 * @return Zero if the counter hits 0.
 *
 */
int _gfx_mesh_decrease_instances(

		GFXMesh*      mesh,
		unsigned int  bucket,
		unsigned int  instances);

/**
 * Returns the instance counter of a bucket handle.
 *
 */
unsigned int _gfx_mesh_get_instances(

		GFXMesh*      mesh,
		unsigned int  bucket);

/**
 * Sets the visible 'counter'.
 *
 * @return Actual set value (clamped by instances).
 *
 */
unsigned int _gfx_mesh_set_visible(

		GFXMesh*      mesh,
		unsigned int  bucket,
		unsigned int  instances);

/**
 * Gets the visible 'counter'.
 *
 */
unsigned int _gfx_mesh_get_visible(

		GFXMesh*      mesh,
		unsigned int  bucket);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_PROTOCOL_H
