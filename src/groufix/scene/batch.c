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

#include "groufix/scene/protocol.h"

#include <limits.h>
#include <string.h>

/******************************************************/
static inline unsigned int _gfx_batch_get_num_units(

		unsigned int  instances,
		size_t        unitSize)
{
	/* If infinite instances per unit, allocate a single unit */
	if(!unitSize) return 1;

	/* Divide and round up for the minimum number of units */
	return instances ? (instances - 1) / unitSize + 1 : 0;
}

/******************************************************/
static GFXBucketSource _gfx_batch_get_source(

		GFXBatch*  batch,
		GFXPipe*   bucket)
{
	/* Get parameters */
	const GFX_BatchData* data =
		_gfx_submesh_get_batch_data(batch->submesh, batch->submeshID);

	/* Get source */
	return _gfx_submesh_get_bucket_source(
		batch->submesh,
		bucket,
		data->params.source
	);
}

/******************************************************/
static void _gfx_batch_get_map(

		GFXBatch*         batch,
		GFXPropertyMap**  map,
		size_t*           instances)
{
	/* Get property map index */
	unsigned int index = _gfx_material_get_batch_map(
		batch->material,
		batch->materialID);

	/* Get map and retrieve */
	unsigned int num;
	GFXPropertyMapList list = gfx_material_get_all(
		batch->material,
		&num);

	*map = gfx_property_map_list_at(list, index);
	*instances = gfx_property_map_list_instances_at(list, index);
}

/******************************************************/
void _gfx_batch_set_unit_data(

		GFXBucket*            bucket,
		GFXBucketUnit*        units,
		const GFX_BatchData*  data,
		unsigned int          num,
		unsigned int          copy,
		unsigned int          first)
{
	unsigned int add =
		(data->flags & GFX_BATCH_MULTIPLE_DATA) ? 1 : 0;

	GFXBatchState state =
		(data->flags & GFX_BATCH_INITIAL_ZERO_STATE ? 0 : data->variant) +
		data->variant * first;

	/* Set all the copies and states */
	unsigned int u;
	for(u = 0; u < num; ++u, copy += add, state += data->variant)
	{
		gfx_bucket_set_copy(bucket, units[u], copy);
		gfx_bucket_set_state(bucket, units[u], state | data->base);
	}
}

/******************************************************/
int gfx_batch_get(

		GFXBatch*       batch,
		GFXMaterial*    material,
		GFXSubMesh*     submesh,
		GFXBatchParams  params)
{
	struct GFX_BatchData data;

	memset(&data, 0, sizeof(struct GFX_BatchData));
	data.params = params;

	/* Get batch at mesh */
	unsigned int matID;
	unsigned int meshID = _gfx_submesh_get_batch(
		submesh,
		material,
		&data,
		&matID
	);

	/* Process batches */
	int success = 1;

	if(!matID)
	{
		/* Insert batch at material if it doesn't exist yet */
		/* Also set the references to each other */
		matID = _gfx_material_insert_batch(material, submesh);

		_gfx_submesh_set_batch(submesh, meshID, matID);
		success = _gfx_material_set_batch(material, matID, meshID);
	}

	/* Remove on failure */
	if(!matID || !meshID || !success)
	{
		_gfx_submesh_remove_batch(submesh, meshID);
		_gfx_material_remove_batch(material, matID);

		return 0;
	}

	/* Initialize batch struct */
	batch->material   = material;
	batch->materialID = matID;
	batch->submesh    = submesh;
	batch->submeshID  = meshID;

	return 1;
}

/******************************************************/
void gfx_batch_erase(

		GFXBatch* batch)
{
	_gfx_submesh_remove_batch(batch->submesh, batch->submeshID);
	_gfx_material_remove_batch(batch->material, batch->materialID);
}

/******************************************************/
GFXBatchParams gfx_batch_get_params(

		GFXBatch* batch)
{
	const GFX_BatchData* data = _gfx_submesh_get_batch_data(
		batch->submesh,
		batch->submeshID);

	if(!data)
	{
		GFXBatchParams params;
		memset(&params, 0, sizeof(GFXBatchParams));

		return params;
	}

	return data->params;
}

/******************************************************/
unsigned int gfx_batch_get_property_map(

		GFXBatch* batch)
{
	return _gfx_material_get_batch_map(
		batch->material,
		batch->materialID);
}

/******************************************************/
unsigned int gfx_batch_get_copies(

		GFXBatch*      batch,
		unsigned int*  num)
{
	const GFX_BatchData* data = _gfx_submesh_get_batch_data(
		batch->submesh,
		batch->submeshID);

	if(!data)
	{
		*num = 0;
		return 0;
	}

	*num = _gfx_submesh_get_batch_units(batch->submesh, batch->submeshID);
	*num = (data->flags & GFX_BATCH_MULTIPLE_DATA) ? *num : (*num ? 1 : 0);

	return _gfx_material_get_batch_copy(
		batch->material,
		batch->materialID);
}

/******************************************************/
GFXBatchFlags gfx_batch_get_flags(

		GFXBatch* batch)
{
	const GFX_BatchData* data = _gfx_submesh_get_batch_data(
		batch->submesh,
		batch->submeshID);

	if(!data) return 0;
	return data->flags;
}

/******************************************************/
void gfx_batch_get_state(

		GFXBatch*       batch,
		GFXBatchState*  base,
		GFXBatchState*  variant)
{
	const GFX_BatchData* data = _gfx_submesh_get_batch_data(
		batch->submesh,
		batch->submeshID);

	if(!data)
	{
		*base = 0;
		*variant = 0;
	}

	*base = data->base;
	*variant = data->variant;
}

/******************************************************/
void gfx_batch_set_flags(

		GFXBatch*      batch,
		GFXBatchFlags  flags)
{
	const GFX_BatchData* data = _gfx_submesh_get_batch_data(
		batch->submesh,
		batch->submeshID
	);

	/* Get difference and set */
	GFXBatchFlags diff = flags ^ data->flags;

	_gfx_submesh_set_batch_data(
		batch->submesh,
		batch->submeshID,
		flags,
		data->base,
		data->variant,
		_gfx_material_get_batch_copy(batch->material, batch->materialID)
	);

	/* Fix number of copies */
	unsigned int units = _gfx_submesh_get_batch_units(
		batch->submesh,
		batch->submeshID
	);

	if((diff & GFX_BATCH_MULTIPLE_DATA) && units--)
	{
		if(flags & GFX_BATCH_MULTIPLE_DATA) _gfx_material_increase_batch_copies(
			batch->material, batch->materialID, units);

		else _gfx_material_decrease_batch_copies(
			batch->material, batch->materialID, units);
	}
}

/******************************************************/
void gfx_batch_set_state(

		GFXBatch*      batch,
		GFXBatchState  base,
		GFXBatchState  variant)
{
	const GFX_BatchData* data = _gfx_submesh_get_batch_data(
		batch->submesh,
		batch->submeshID
	);

	_gfx_submesh_set_batch_data(
		batch->submesh,
		batch->submeshID,
		data->flags,
		base,
		variant,
		_gfx_material_get_batch_copy(batch->material, batch->materialID)
	);
}

/******************************************************/
unsigned int gfx_batch_get_instances(

		GFXBatch*  batch,
		GFXPipe*   bucket)
{
	/* Get units handle */
	unsigned int handle;
	if(!_gfx_submesh_find_units(
		batch->submesh,
		batch->submeshID,
		bucket,
		&handle))
	{
		return 0;
	}

	return _gfx_submesh_get_instances(batch->submesh, handle);
}

/******************************************************/
unsigned int gfx_batch_get_visible(

		GFXBatch*  batch,
		GFXPipe*   bucket)
{
	/* Get units handle */
	unsigned int handle;
	if(!_gfx_submesh_find_units(
		batch->submesh,
		batch->submeshID,
		bucket,
		&handle))
	{
		return 0;
	}

	return _gfx_submesh_get_visible(batch->submesh, handle);
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
			bucket, units[0], newVisible);
		gfx_bucket_set_visible(
			bucket, units[0], newVisible);
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
				bucket, units[start], unitSize - inst + add);
			gfx_bucket_set_visible(
				bucket, units[start], 1);

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

			if(inst <= sub) gfx_bucket_set_visible(
				bucket, units[start], 0), sub = inst;
			else gfx_bucket_set_instances(
				bucket, units[start], inst - sub);

			oldVisible -= sub;
			inst = unitSize;
			--start;
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
	/* Get all data */
	unsigned int copy = _gfx_material_get_batch_copy(
		batch->material, batch->materialID);
	const GFX_BatchData* data = _gfx_submesh_get_batch_data(
		batch->submesh, batch->submeshID);

	if(data->flags & GFX_BATCH_MULTIPLE_DATA)
	{
		/* If multiple data, use added units */
		unsigned int copies =
			_gfx_submesh_get_batch_units(batch->submesh, batch->submeshID);
		currentCopies =
			copies - currentCopies;

		copy += first;
	}
	else
	{
		/* If single data, check if there exist current copies */
		currentCopies = currentCopies ? 0 : 1;
	}

	/* Set copies and state of itself */
	units += first;
	num -= first;
	_gfx_batch_set_unit_data(bucket, units, data, num, copy, first);

	/* Increase batch copies */
	_gfx_material_increase_batch_copies(
		batch->material,
		batch->materialID,
		currentCopies
	);
}

/******************************************************/
static void _gfx_batch_decrease_copies(

		GFXBatch*     batch,
		unsigned int  currentCopies)
{
	const GFX_BatchData* data =
		_gfx_submesh_get_batch_data(batch->submesh, batch->submeshID);

	/* If multiple data, use number of erased units */
	if(data->flags & GFX_BATCH_MULTIPLE_DATA) currentCopies -=
		_gfx_submesh_get_batch_units(batch->submesh, batch->submeshID);

	/* If single data, check if any units are left */
	else currentCopies =
		_gfx_submesh_get_batch_units(batch->submesh, batch->submeshID) ? 0 : 1;

	_gfx_material_decrease_batch_copies(
		batch->material,
		batch->materialID,
		currentCopies
	);
}

/******************************************************/
static int _gfx_batch_increase_instances(

		GFXBatch*     batch,
		GFXPipe*      bucket,
		unsigned int  handle,
		unsigned int  instances,
		unsigned int  currentCopies)
{
	/* Get data */
	GFXPropertyMap* map;
	size_t unitSize;
	unsigned int start;
	_gfx_batch_get_map(batch, &map, &unitSize);

	_gfx_submesh_get_reserved(
		batch->submesh,
		handle,
		&start);

	/* Get number of units and reserve */
	unsigned int end = _gfx_batch_get_num_units(
		instances,
		unitSize);

	GFXBucketUnit* units = _gfx_submesh_reserve(
		batch->submesh,
		batch->submeshID,
		handle,
		end);

	if(units)
	{
		GFXBucketSource source =
			_gfx_batch_get_source(batch, bucket);

		/* Iterate through units and add them to the bucket */
		/* Also set the base instances */
		unsigned int u;
		for(u = start; u < end; ++u)
		{
			units[u] = gfx_bucket_insert(
				bucket->bucket, source, map, 0, 0);
			gfx_bucket_set_instance_base(
				bucket->bucket, units[u], unitSize * u);

			/* Bail, Fire! */
			if(!units[u]) break;
		}

		/* Victory! */
		if(u >= end)
		{
			_gfx_batch_increase_copies(
				batch, bucket->bucket, units, end, start, currentCopies);

			return 1;
		}

		/* Well then, destroy units again and unreserve */
		while(u > start)
			gfx_bucket_erase(bucket->bucket, units[--u]);

		_gfx_submesh_reserve(
			batch->submesh,
			batch->submeshID,
			handle,
			start
		);
	}

	/* Well nevermind */
	if(!start) _gfx_submesh_remove_units(
		batch->submesh,
		batch->submeshID,
		bucket);

	return 0;
}

/******************************************************/
static void _gfx_batch_decrease_instances(

		GFXBatch*     batch,
		GFXPipe*      bucket,
		unsigned int  handle,
		unsigned int  instances,
		unsigned int  currentCopies)
{
	/* Get data */
	unsigned int end;
	GFXBucketUnit* units = _gfx_submesh_get_reserved(
		batch->submesh,
		handle,
		&end);

	if(instances)
	{
		/* Get more data! */
		GFXPropertyMap* map;
		size_t unitSize;
		_gfx_batch_get_map(batch, &map, &unitSize);

		/* Get number of remaining units and erase the rest */
		unsigned int start = _gfx_batch_get_num_units(
			instances,
			unitSize);

		while(end > start) gfx_bucket_erase(
			bucket->bucket,
			units[--end]
		);

		/* Get number of visible units, set visibility and reserve */
		/* Just pretend all remaining units are visible at this point */
		unsigned int visible = _gfx_submesh_get_visible(
			batch->submesh,
			handle);

		_gfx_batch_set_visible(
			bucket->bucket,
			units,
			unitSize,
			instances,
			instances < visible ? instances : visible);

		_gfx_submesh_reserve(
			batch->submesh,
			batch->submeshID,
			handle,
			start);
	}
	else
	{
		/* Erase all units */
		while(end) gfx_bucket_erase(
			bucket->bucket,
			units[--end]
		);

		/* Remove the bucket */
		_gfx_submesh_remove_units(
			batch->submesh,
			batch->submeshID,
			bucket
		);
	}

	/* Decrease copies */
	_gfx_batch_decrease_copies(batch, currentCopies);
}

/******************************************************/
int gfx_batch_set_instances(

		GFXBatch*     batch,
		GFXPipe*      bucket,
		unsigned int  instances)
{
	/* Get units handle */
	unsigned int handle;
	if(!_gfx_submesh_find_units(
		batch->submesh,
		batch->submeshID,
		bucket,
		&handle))
	{
		if(!instances) return 1;

		if(!_gfx_submesh_insert_units(
			batch->submesh,
			batch->submeshID,
			bucket,
			&handle))
		{
			return 0;
		}
	}

	/* Get current number of instances */
	unsigned int current = _gfx_submesh_get_instances(
		batch->submesh,
		handle);

	unsigned int copies = _gfx_submesh_get_batch_units(
		batch->submesh,
		batch->submeshID);

	if(instances > current)
	{
		/* Increase */
		if(!_gfx_batch_increase_instances(
			batch, bucket, handle, instances, copies)) return 0;
	}

	else if(instances < current)
	{
		/* Decrease */
		_gfx_batch_decrease_instances(
			batch, bucket, handle, instances, copies);
	}

	/* Aaand set instances */
	if(instances) _gfx_submesh_set_instances(
		batch->submesh,
		handle,
		instances
	);

	return 1;
}

/******************************************************/
unsigned int gfx_batch_set_visible(

		GFXBatch*     batch,
		GFXPipe*      bucket,
		unsigned int  instances)
{
	/* Get and validate units */
	unsigned int handle;
	unsigned int num;

	if(!_gfx_submesh_find_units(
		batch->submesh,
		batch->submeshID,
		bucket,
		&handle))
	{
		return 0;
	}

	GFXBucketUnit* units = _gfx_submesh_get_reserved(
		batch->submesh,
		handle,
		&num
	);

	/* Get current and set number of visible instances */
	unsigned int old =
		_gfx_submesh_get_visible(batch->submesh, handle);
	instances =
		_gfx_submesh_set_visible(batch->submesh, handle, instances);

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
