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

#include "groufix/scene/protocol.h"

#include <limits.h>

/******************************************************/
static inline unsigned int _gfx_batch_get_num_units(

		unsigned int  instances,
		size_t        unitSize)
{
	/* If infinite instances per unit, allocate a single unit */
	if(!unitSize) return 1;

	/* Right, zero instances, good job */
	if(!instances) return 0;

	/* Divide and round up for the minimum number of units */
	return (instances - 1) / unitSize + 1;
}

/******************************************************/
static GFXBucketSource _gfx_batch_get_source(

		GFXBatch*  batch,
		GFXPipe*   bucket)
{
	/* Get LOD parameters */
	GFXBatchLod params;
	_gfx_mesh_get_batch_lod(batch->mesh, batch->meshID, &params);

	/* Get submesh */
	unsigned int num;
	GFXSubMeshList subs = gfx_mesh_get(
		batch->mesh,
		params.mesh,
		&num
	);

	if(params.index >= num) return 0;

	/* Get source */
	return _gfx_submesh_get_bucket_source(
		gfx_submesh_list_at(subs, params.index),
		bucket,
		gfx_submesh_list_source_at(subs, params.index)
	);
}

/******************************************************/
static int _gfx_batch_get_map(

		GFXBatch*         batch,
		GFXPropertyMap**  map,
		size_t*           instances)
{
	/* Get property map index */
	unsigned int index = _gfx_material_get_batch_map(
		batch->material,
		batch->materialID
	);

	/* Get map and retrieve */
	unsigned int num;
	GFXPropertyMapList list = gfx_material_get_all(
		batch->material,
		&num
	);

	if(index >= num) return 0;

	*map = gfx_property_map_list_at(list, index);
	*instances = gfx_property_map_list_instances_at(list, index);

	return 1;
}

/******************************************************/
static void _gfx_batch_set_visible(

		GFXBucket*      bucket,
		GFXBucketUnit*  units,
		size_t          unitSize,
		unsigned int    oldVisible,
		unsigned int    newVisible)
 {
	if(!unitSize)
	{
		/* If infinite instances per unit, adjust the single unit */
		gfx_bucket_set_instances(
			bucket,
			units[0],
			newVisible);

		gfx_bucket_set_visible(
			bucket,
			units[0],
			newVisible);
	}
	else
	{
		unsigned int start = oldVisible / unitSize;
		unsigned int inst = oldVisible % unitSize;

		/* Start adding instances untill we have newVisible instances */
		while(oldVisible < newVisible)
		{
			unsigned int add = newVisible - oldVisible;
			inst = unitSize - inst;
			add = (inst < add) ? inst : add;

			gfx_bucket_set_instances(
				bucket,
				units[start],
				unitSize - inst + add
			);

			gfx_bucket_set_visible(
				bucket,
				units[start],
				1
			);

			oldVisible += add;
			inst = 0;
			++start;
		}

		/* Trim off instances untill we have newVisible instances left */
		start = inst ? start : start - 1;
		inst = inst ? inst : unitSize;

		while(oldVisible > newVisible)
		{
			unsigned int sub = oldVisible - newVisible;

			if(inst <= sub)
			{
				sub = inst;
				gfx_bucket_set_visible(
					bucket,
					units[start],
					0
				);
			}

			else gfx_bucket_set_instances(
				bucket,
				units[start],
				inst - sub
			);

			oldVisible -= sub;
			inst = unitSize;
			--start;
		}
	}
}

/******************************************************/
void _gfx_batch_set_copy(

		GFXBucket*      bucket,
		GFXBatchType    type,
		GFXBucketUnit*  units,
		unsigned int    num,
		unsigned int    copy)
{
	unsigned int add = 0;
	switch(type)
	{
		case GFX_BATCH_SINGLE_DATA :
			add = 0;
			break;

		case GFX_BATCH_MULTIPLE_DATA :
			add = 1;
			break;
	}

	/* Set all the copies */
	unsigned int u;
	for(u = 0; u < num; ++u, copy += add)
		gfx_bucket_set_copy(
			bucket,
			units[u],
			copy
		);
}

/******************************************************/
int gfx_batch_get(

		GFXBatch*     batch,
		GFXMaterial*  material,
		GFXMesh*      mesh,
		GFXBatchLod   params)
{
	/* Get batch at mesh */
	unsigned int matID;
	unsigned int meshID = _gfx_mesh_get_batch(
		mesh,
		material,
		params,
		&matID
	);

	if(!matID)
	{
		/* Insert batch at material if it doesn't exist yet */
		/* Also set the references to each other */
		matID = _gfx_material_insert_batch(material, mesh);
		_gfx_material_set_batch(material, matID, meshID);
		_gfx_mesh_set_batch(mesh, meshID, matID);
	}

	/* Remove on failure */
	if(!matID || !meshID)
	{
		_gfx_mesh_remove_batch(mesh, meshID);
		_gfx_material_remove_batch(material, matID);

		return 0;
	}

	/* Initialize batch struct */
	batch->material   = material;
	batch->materialID = matID;
	batch->mesh       = mesh;
	batch->meshID     = meshID;

	return 1;
}

/******************************************************/
void gfx_batch_erase(

		GFXBatch* batch)
{
	_gfx_mesh_remove_batch(batch->mesh, batch->meshID);
	_gfx_material_remove_batch(batch->material, batch->materialID);
}

/******************************************************/
unsigned int gfx_batch_get_property_map(

		GFXBatch* batch)
{
	return _gfx_material_get_batch_map(batch->material, batch->materialID);
}

/******************************************************/
unsigned int gfx_batch_get_submesh(

		GFXBatch* batch)
{
	/* Get LOD parameters */
	GFXBatchLod params;
	_gfx_mesh_get_batch_lod(batch->mesh, batch->meshID, &params);

	/* Bound check */
	unsigned int num;
	gfx_mesh_get(batch->mesh, params.mesh, &num);

	return params.index >= num ?
		UINT_MAX :
		gfx_lod_map_count((GFXLodMap*)batch->mesh, params.mesh) +
		params.index;
}

/******************************************************/
GFXBatchLod gfx_batch_get_lod(

		GFXBatch* batch)
{
	GFXBatchLod params;
	_gfx_mesh_get_batch_lod(batch->mesh, batch->meshID, &params);

	return params;
}

/******************************************************/
GFXBatchType gfx_batch_get_type(

		GFXBatch* batch)
{
	return _gfx_mesh_get_batch_type(batch->mesh, batch->meshID);
}

/******************************************************/
void gfx_batch_set_type(

		GFXBatch*     batch,
		GFXBatchType  type)
{
	GFXBatchType current =
		_gfx_mesh_get_batch_type(batch->mesh, batch->meshID);

	/* Only process if different */
	if(type != current)
	{
		_gfx_mesh_set_batch_type(
			batch->mesh,
			batch->meshID,
			type,
			_gfx_material_get_batch_copy(batch->material, batch->materialID));

		unsigned int units = _gfx_mesh_get_batch_units(
			batch->mesh,
			batch->meshID);

		/* Fix number of copies */
		if(units--) switch(type)
		{
			case GFX_BATCH_SINGLE_DATA :
				_gfx_material_decrease_copies(
					batch->material,
					batch->materialID,
					units
				);
				break;

			case GFX_BATCH_MULTIPLE_DATA :
				_gfx_material_increase_copies(
					batch->material,
					batch->materialID,
					units
				);
				break;
		}
	}
}

/******************************************************/
static void _gfx_batch_increase_copies(

		GFXBatch*       batch,
		GFXBucket*      bucket,
		GFXBucketUnit*  units,
		unsigned int    num,
		unsigned int    first,
		unsigned int    currentCopies)
{
	GFXBatchType type =
		_gfx_mesh_get_batch_type(batch->mesh, batch->meshID);
	unsigned int copy =
		_gfx_material_get_batch_copy(batch->material, batch->materialID);

	switch(type)
	{
		/* If single data, check if there exist current copies */
		case GFX_BATCH_SINGLE_DATA :
			currentCopies = currentCopies ? 0 : 1;
			break;

		/* If multiple data, use added units */
		case GFX_BATCH_MULTIPLE_DATA :
			currentCopies = _gfx_mesh_get_batch_units(
				batch->mesh,
				batch->meshID) - currentCopies;

			copy += first;
			break;
	}

	/* Increase batch copies and set copies of itself */
	_gfx_material_increase_copies(
		batch->material,
		batch->materialID,
		currentCopies);

	_gfx_batch_set_copy(
		bucket,
		type,
		units + first,
		num - first,
		copy);
}

/******************************************************/
int gfx_batch_increase(

		GFXBatch*     batch,
		GFXPipe*      bucket,
		unsigned int  instances)
{
	/* Get bucket and increase */
	unsigned int handle = _gfx_mesh_get_bucket(
		batch->mesh,
		batch->meshID,
		bucket
	);

	if(!_gfx_mesh_increase_instances(batch->mesh, handle, instances))
		return 0;

	/* Get and validate data */
	/* Calculate the number of wanted units */
	GFXPropertyMap* map;
	size_t unitSize;
	unsigned int start;

	GFXBucketSource source =
		_gfx_batch_get_source(batch, bucket);
	_gfx_mesh_get_reserved(
		batch->mesh, handle, &start);

	if(source && _gfx_batch_get_map(batch, &map, &unitSize))
	{
		unsigned int copies =
			_gfx_mesh_get_batch_units(batch->mesh, batch->meshID);

		/* Get current number of units and reserve extra ones */
		unsigned int end = _gfx_batch_get_num_units(
			_gfx_mesh_get_instances(batch->mesh, handle), unitSize);
		GFXBucketUnit* units = _gfx_mesh_reserve(
			batch->mesh, batch->meshID, handle, end);

		if(end && units)
		{
			/* iterate through units and add them to the bucket */
			/* Also set the base instances */
			unsigned int i;
			for(i = start; i < end; ++i)
			{
				units[i] = gfx_bucket_insert(
					bucket->bucket, source, map, 0, 0);
				gfx_bucket_set_instance_base(
					bucket->bucket, units[i], unitSize * i);

				/* Bail! Fire! */
				if(!units[i]) break;
			}

			/* Woop Woop, Victory! */
			if(i >= end)
			{
				/* Increase copies */
				_gfx_batch_increase_copies(
					batch, bucket->bucket,
					units, end, start,
					copies
				);
				return 1;
			}

			/* Well then, destroy units again and unreserve */
			while(i > start)
				gfx_bucket_erase(bucket->bucket, units[--i]);

			_gfx_mesh_reserve(
				batch->mesh,
				batch->meshID,
				handle,
				start
			);
		}
	}

	/* Well nevermind */
	if(start) _gfx_mesh_decrease_instances(
		batch->mesh,
		handle,
		instances);

	else _gfx_mesh_remove_bucket(
		batch->mesh,
		batch->meshID,
		bucket);

	return 0;
}

/******************************************************/
static void _gfx_batch_decrease_copies(

		GFXBatch*     batch,
		unsigned int  currentCopies)
{
	GFXBatchType type =
		_gfx_mesh_get_batch_type(batch->mesh, batch->meshID);

	switch(type)
	{
		/* If single data, check if any units are left */
		case GFX_BATCH_SINGLE_DATA :
			currentCopies = _gfx_mesh_get_batch_units(
				batch->mesh,
				batch->meshID) ? 0 : 1;

			break;

		/* If multiple data, use number of erased units */
		case GFX_BATCH_MULTIPLE_DATA :
			currentCopies -= _gfx_mesh_get_batch_units(
				batch->mesh,
				batch->meshID
			);
			break;
	}

	_gfx_material_decrease_copies(
		batch->material,
		batch->materialID,
		currentCopies
	);
}

/******************************************************/
int gfx_batch_decrease(

		GFXBatch*     batch,
		GFXPipe*      bucket,
		unsigned int  instances)
{
	unsigned int copies =
		_gfx_mesh_get_batch_units(batch->mesh, batch->meshID);

	/* Get and validate units */
	unsigned int end;

	unsigned int handle =
		_gfx_mesh_find_bucket(batch->mesh, batch->meshID, bucket);
	GFXBucketUnit* units =
		_gfx_mesh_get_reserved(batch->mesh, handle, &end);

	if(units)
	{
		if(_gfx_mesh_decrease_instances(batch->mesh, handle, instances))
		{
			/* Get associated data */
			GFXPropertyMap* map;
			size_t unitSize;
			_gfx_batch_get_map(batch, &map, &unitSize);

			/* Get the number of remaining units and erase the rest */
			instances =
				_gfx_mesh_get_instances(batch->mesh, handle);
			unsigned int start =
				_gfx_batch_get_num_units(instances, unitSize);

			while(end > start) gfx_bucket_erase(
				bucket->bucket,
				units[--end]
			);

			/* Get the number of visible units, set visibility and reserve */
			/* Just pretend everything is visible at this point */
			_gfx_batch_set_visible(
				bucket->bucket, units, unitSize, instances,
				_gfx_mesh_get_visible(batch->mesh, handle));
			_gfx_mesh_reserve(
				batch->mesh, batch->meshID, handle, start);

			/* Decrease copies */
			_gfx_batch_decrease_copies(batch, copies);

			return 1;
		}

		/* Erase all units */
		while(end) gfx_bucket_erase(
			bucket->bucket,
			units[--end]
		);
	}

	/* Remove the bucket */
	_gfx_mesh_remove_bucket(
		batch->mesh,
		batch->meshID,
		bucket
	);

	/* Decrease copies */
	_gfx_batch_decrease_copies(batch, copies);

	return 0;
}

/******************************************************/
unsigned int gfx_batch_get_instances(

		GFXBatch*  batch,
		GFXPipe*   bucket)
{
	/* Get bucket handle */
	unsigned int handle = _gfx_mesh_find_bucket(
		batch->mesh,
		batch->meshID,
		bucket
	);

	return _gfx_mesh_get_instances(batch->mesh, handle);
}

/******************************************************/
unsigned int gfx_batch_set_visible(

		GFXBatch*     batch,
		GFXPipe*      bucket,
		unsigned int  instances)
{
	unsigned int num;

	/* Get and validate units */
	unsigned int handle =
		_gfx_mesh_find_bucket(batch->mesh, batch->meshID, bucket);
	GFXBucketUnit* units =
		_gfx_mesh_get_reserved(batch->mesh, handle, &num);

	if(!num) return 0;

	/* Get current and set number of visible instances */
	unsigned int old =
		_gfx_mesh_get_visible(batch->mesh, handle);
	instances =
		_gfx_mesh_set_visible(batch->mesh, handle, instances);

	/* Get associated data */
	GFXPropertyMap* map;
	size_t unitSize;
	_gfx_batch_get_map(batch, &map, &unitSize);

	/* Set visibility */
	_gfx_batch_set_visible(
		bucket->bucket,
		units,
		unitSize,
		old,
		instances
	);

	return instances;
}

/******************************************************/
unsigned int gfx_batch_get_visible(

		GFXBatch*  batch,
		GFXPipe*   bucket)
{
	/* Get bucket handle */
	unsigned int handle = _gfx_mesh_find_bucket(
		batch->mesh,
		batch->meshID,
		bucket
	);

	return _gfx_mesh_get_visible(batch->mesh, handle);
}
