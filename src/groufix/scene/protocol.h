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

#include "groufix/scene/internal.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Unit manipulation of batch
 *******************************************************/

/**
 * Sets the copy and state of all given units according to given flags.
 *
 * @param num   Number of units.
 * @param copy  Starting copy to use.
 * @param first Displacement index to use for calculating the states.
 *
 */
void _gfx_batch_set_unit_data(

		GFXBucket*            bucket,
		GFXBucketUnit*        units,
		const GFX_BatchData*  data,
		unsigned int          num,
		unsigned int          copy,
		unsigned int          first);


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
 * In addition, it will also call _gfx_submesh_set_unit_data for the given batch, if necessary.
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
 * Calss _gfx_batch_set_unit_data for all associated buckets of a batch.
 *
 * This call will set the copy and state of all units for all buckets with the stored states.
 *
 * Note: undefined behaviour if the batch does not exist!
 *
 */
void _gfx_submesh_set_unit_data(

		GFXSubMesh*   mesh,
		unsigned int  submeshID,
		unsigned int  copy);


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
