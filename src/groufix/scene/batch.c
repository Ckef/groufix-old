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

#include "groufix/scene/batch.h"
#include "groufix/scene/internal.h"

/******************************************************/
static inline size_t _gfx_batch_get_num_units(

		size_t  instances,
		size_t  unitSize)
{
	/* If infinite instances per unit, allocate a single unit */
	if(!unitSize) return 1;

	/* Divide and round up for the minimum number of units */
	return (instances - 1) / unitSize + 1;
}

/******************************************************/
static size_t _gfx_batch_get_source(

		GFXBatch*  batch,
		GFXPipe*   bucket)
{
	/* Get LOD parameters */
	GFXBatchLod params;
	if(!_gfx_mesh_get_batch_lod(batch->mesh, batch->meshID, &params))
		return 0;

	/* Get submesh */
	size_t num;
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
	size_t index = _gfx_material_get_batch_map(
		batch->material,
		batch->materialID
	);

	/* Get map and retrieve */
	size_t num;
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
int _gfx_batch_rebuild(

		GFXBatch*  batch,
		GFXPipe*   bucket)
{
	/* Get and validate data */
	size_t source = _gfx_batch_get_source(batch, bucket);
	GFXPropertyMap* map;
	size_t temp;

	size_t num = _gfx_mesh_find_bucket(
		batch->mesh,
		batch->meshID,
		bucket);

	size_t* units = _gfx_mesh_get_reserved(
		batch->mesh,
		num,
		&num);

	if(source && _gfx_batch_get_map(batch, &map, &temp))
	{
		/* Iterate through units and rebuild them */
		for(temp = 0; temp < num; ++temp)
		{
			if(!gfx_bucket_rebuild(
				bucket->bucket,
				units[temp],
				source,
				map))
			{
				/* Uh, bail? */
				break;
			}
		}

		/* Success! */
		if(temp >= num) return 1;
	}

	/* Remove all units */
	while(num)
		gfx_bucket_erase(bucket->bucket, units[--num]);

	_gfx_mesh_remove_bucket(
		batch->mesh,
		batch->meshID,
		bucket);

	return 0;
}

/******************************************************/
int gfx_batch_get(

		GFXBatch*     batch,
		GFXMaterial*  material,
		GFXMesh*      mesh,
		GFXBatchLod   params)
{
	/* Get batch at mesh */
	size_t matID;
	size_t meshID = _gfx_mesh_get_batch(
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
	_gfx_mesh_set_batch_type(batch->mesh, batch->meshID, type);
}

/******************************************************/
size_t gfx_batch_get_instances(

		GFXBatch*  batch,
		GFXPipe*   bucket)
{
	/* Get unit group */
	size_t handle = _gfx_mesh_find_bucket(
		batch->mesh,
		batch->meshID,
		bucket
	);

	return _gfx_mesh_get(batch->mesh, handle);
}

/******************************************************/
int gfx_batch_increase(

		GFXBatch*  batch,
		GFXPipe*   bucket,
		size_t     instances)
{
	/* Get bucket and increase */
	size_t handle = _gfx_mesh_get_bucket(
		batch->mesh,
		batch->meshID,
		bucket
	);

	if(!_gfx_mesh_increase(batch->mesh, handle, instances))
		return 0;

	/* Get and validate data */
	size_t source = _gfx_batch_get_source(batch, bucket);
	GFXPropertyMap* map;
	size_t unitSize;

	if(!source || !_gfx_batch_get_map(batch, &map, &unitSize))
	{
		_gfx_mesh_remove_bucket(batch->mesh, batch->meshID, bucket);
		return 0;
	}

	/* Calculate the number of current and wanted units */
	size_t start;
	_gfx_mesh_get_reserved(batch->mesh, handle, &start);

	size_t end = _gfx_batch_get_num_units(
		_gfx_mesh_get(batch->mesh, handle),
		unitSize
	);

	/* Reserve extra ones */
	size_t* units = _gfx_mesh_reserve(
		batch->mesh,
		batch->meshID,
		handle,
		end
	);

	if(units)
	{
		/* iterate through units and add them to the bucket */
		/* Also set the base instances */
		size_t i;
		for(i = start; i < end; ++i)
		{
			units[i] = gfx_bucket_insert(
				bucket->bucket,
				source,
				map,
				0, 0
			);

			/* Bail! Fire! */
			if(!units[i]) break;

			gfx_bucket_set_instance_base(
				bucket->bucket,
				units[i],
				unitSize * i
			);
		}

		/* Woop Woop, Victory! */
		if(i >= end) return 1;

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

	/* Well nevermind */
	_gfx_mesh_decrease(batch->mesh, handle, instances);

	return 0;
}

/******************************************************/
int gfx_batch_decrease(

		GFXBatch*  batch,
		GFXPipe*   bucket,
		size_t     instances)
{
	/* Get and validate units */
	size_t handle = _gfx_mesh_find_bucket(
		batch->mesh,
		batch->meshID,
		bucket);

	size_t end;
	size_t* units = _gfx_mesh_get_reserved(
		batch->mesh,
		handle,
		&end);

	if(units)
	{
		/* Decrease */
		if(_gfx_mesh_decrease(
			batch->mesh,
			handle,
			instances))
		{
			/* Get associated data */
			GFXPropertyMap* map;
			size_t unitSize;
			_gfx_batch_get_map(batch, &map, &unitSize);

			/* Get the number of remaining instances */
			/* Calculate the max number of instances of the last remaining unit */
			size_t last = _gfx_mesh_get(
				batch->mesh,
				handle);
			size_t start = _gfx_batch_get_num_units(
				last,
				unitSize);

			if(unitSize) last %= unitSize;

			/* Fix the last remaining unit */
			if(last)
			{
				unitSize = gfx_bucket_get_instances(
					bucket->bucket,
					units[start - 1]);

				gfx_bucket_set_instances(
					bucket->bucket,
					units[start - 1],
					(last < unitSize) ? last : unitSize);
			}

			/* Iterate through units and erase them */
			while(end > start)
				gfx_bucket_erase(bucket->bucket, units[--end]);

			/* Reserve fewer units */
			_gfx_mesh_reserve(
				batch->mesh,
				batch->meshID,
				handle,
				start
			);

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

	return 0;
}

/******************************************************/
size_t gfx_batch_set_visible(

		GFXBatch*  batch,
		GFXPipe*   bucket,
		size_t     instances)
{
	/* Get and validate units */
	size_t handle = _gfx_mesh_find_bucket(
		batch->mesh,
		batch->meshID,
		bucket);

	size_t num;
	size_t* units = _gfx_mesh_get_reserved(
		batch->mesh,
		handle,
		&num);

	if(!num) return 0;

	/* Clamp number of instances */
	size_t temp = _gfx_mesh_get(batch->mesh, handle);
	instances = (temp < instances) ? temp : instances;

	/* Get associated data */
	GFXPropertyMap* map;
	size_t unitSize;
	_gfx_batch_get_map(batch, &map, &unitSize);

	/* Iterate and set visibility */
	size_t ret = instances;
	for(temp = 0; temp < num; ++temp)
	{
		unitSize = (unitSize < instances) ? unitSize : instances;
		instances -= unitSize;

		gfx_bucket_set_instances(
			bucket->bucket,
			units[temp],
			unitSize);

		gfx_bucket_set_visible(
			bucket->bucket,
			units[temp],
			unitSize);
	}

	return ret;
}
