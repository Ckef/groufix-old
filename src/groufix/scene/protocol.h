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

/** Batch metadata */
typedef struct GFX_BatchData
{
	GFXBatchParams  params;
	GFXBatchFlags   flags;
	GFXBatchState   base;
	GFXBatchState   variant;

} GFX_BatchData;


/**
 * Sets the copy of all given units according to given flags.
 *
 * @param num  Number of copies in units.
 * @param copy Starting copy to use.
 *
 */
void _gfx_batch_set_unit_copies(

		GFXBucket*            bucket,
		GFXBucketUnit*        units,
		const GFX_BatchData*  data,
		unsigned int          num,
		unsigned int          copy);

/**
 * Sets the state of all given units according to given flags.
 *
 * @param first Displacement index to use for calculating the states.
 *
 */
void _gfx_batch_set_unit_states(

		GFXBucket*            bucket,
		GFXBucketUnit*        units,
		const GFX_BatchData*  data,
		unsigned int          num,
		unsigned int          first);


/********************************************************
 * Batch references at material and submesh
 *******************************************************/

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
 * calls _gfx_submesh_set_unit_copies for all altered batches (except the given one).
 *
 */
int _gfx_material_increase_batch_copies(

		GFXMaterial*  material,
		unsigned int  materialID,
		unsigned int  copies);

/**
 * Decrease the number of copies used by a given batch.
 *
 * @return Zero on failure or if copies is 0.
 *
 * Note: it moves copy data within the property map and
 * calls _gfx_submesh_set_unit_copies for all altered batches (except the given one).
 *
 */
int _gfx_material_decrease_batch_copies(

		GFXMaterial*  material,
		unsigned int  materialID,
		unsigned int  copies);


/********************************************************
 * Batch properties at submesh
 *******************************************************/

/**
 * Returns the maximum number of units reserved by any bucket within a batch.
 *
 */
unsigned int _gfx_submesh_get_batch_units(

		GFXSubMesh*   mesh,
		unsigned int  submeshID);

/**
 * Return the metadata of a batch at a submesh.
 *
 * @return Batch metadata, NULL if batch does not exist.
 *
 */
const GFX_BatchData* _gfx_submesh_get_batch_data(

		GFXSubMesh*   mesh,
		unsigned int  submeshID);

/**
 * Sets the metadata of a batch at a submesh.
 *
 * In addition, it will also call _gfx_submesh_set_unit_copies and
 * _gfx_submesh_set_unit_states for the given batch, if necessar.
 *
 * copy is ignored if flags is equal to the current flags.
 *
 */
void _gfx_submesh_set_batch_data(

		GFXSubMesh*    mesh,
		unsigned int   submeshID,
		GFXBatchFlags  flags,
		GFXBatchState  base,
		GFXBatchState  variant,
		unsigned int   copy);

/**
 * Calls _gfx_batch_set_unit_copies for all associated buckets of a batch.
 *
 * This call will set the copy of all units for all buckets.
 *
 * Note: undefined behaviour if the batch does not exist!
 *
 */
void _gfx_submesh_set_unit_copies(

		GFXSubMesh*   mesh,
		unsigned int  submeshID,
		unsigned int  copy);

/**
 * Calls _gfx_batch_set_unit_states for all associated buckets of a batch.
 *
 * This call will set the state of all units for all buckets with the stored states.
 *
 * Note: undefined behaviour if the batch does not exist!
 *
 */
void _gfx_submesh_set_unit_states(

		GFXSubMesh*   mesh,
		unsigned int  submeshID);


/********************************************************
 * Bucket unit management of batches at meshes
 *******************************************************/

/**
 * Fetch an existing unit handle for a given batch and bucket.
 *
 * @param pipe   Bucket to insert.
 * @param handle Handle of the units associated with the bucket.
 * @return Zero on failure, the handle is untouched.
 *
 * A handle is only valid as long as no other handle at the submesh is altered.
 *
 */
int _gfx_submesh_find_units(

		GFXSubMesh*    mesh,
		unsigned int   submeshID,
		GFXPipe*       pipe,
		unsigned int*  handle);

/**
 * Insert a unit handle for a given batch and bucket.
 *
 * @return Zero on failure, the handle is untouched.
 *
 * Note: if a handle for a given bucket already exists, behaviour is undefined.
 * Therefor, always use _gfx_submesh_find_units first!
 *
 */
int _gfx_submesh_insert_units(

		GFXSubMesh*    mesh,
		unsigned int   submeshID,
		GFXPipe*       pipe,
		unsigned int*  handle);

/**
 * Forcefully remove a unit handle associated with a batch for a given bucket.
 *
 * This will destroy all reserved units.
 *
 */
void _gfx_submesh_remove_units(

		GFXSubMesh*   mesh,
		unsigned int  submeshID,
		GFXPipe*      pipe);

/**
 * Reserves a given amount of units associated with a unit handle.
 *
 * @param units  Number of units to reserve.
 * @return Array of size_t elements of size units, NULL on failure.
 *
 * Note: The pointer is only valid as long as no handle at the submesh reserves any units.
 *
 */
GFXBucketUnit* _gfx_submesh_reserve(

		GFXSubMesh*   mesh,
		unsigned int  submeshID,
		unsigned int  handle,
		unsigned int  units);

/**
 * Returns memory reserved for units associated with a unit handle.
 *
 * @param units Returns the number of units in the returned array.
 * @return Array of size_t elements, reserved to be units.
 *
 */
GFXBucketUnit* _gfx_submesh_get_reserved(

		GFXSubMesh*    mesh,
		unsigned int   handle,
		unsigned int*  units);

/**
 * Sets the instance counter of a unit handle.
 *
 */
void _gfx_submesh_set_instances(

		GFXSubMesh*   mesh,
		unsigned int  handle,
		unsigned int  instances);

/**
 * Returns the instance counter of a unit handle.
 *
 */
unsigned int _gfx_submesh_get_instances(

		GFXSubMesh*   mesh,
		unsigned int  handle);

/**
 * Sets the visible 'counter'.
 *
 * @return Actual set value (clamped by instances).
 *
 */
unsigned int _gfx_submesh_set_visible(

		GFXSubMesh*   mesh,
		unsigned int  handle,
		unsigned int  instances);

/**
 * Gets the visible 'counter'.
 *
 */
unsigned int _gfx_submesh_get_visible(

		GFXSubMesh*   mesh,
		unsigned int  handle);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_PROTOCOL_H
