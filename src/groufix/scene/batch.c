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
void _gfx_batch_rebuild(

		GFXBatch*  batch,
		GFXPipe*   bucket)
{
	/* Get units and associated data */
	size_t groupID = _gfx_mesh_find_group(
		batch->mesh,
		batch->meshID,
		bucket);

	size_t index = _gfx_material_get_batch_map(
		batch->material,
		batch->materialID);

	size_t source = _gfx_batch_get_source(batch, bucket);

	/* Validate data */
	size_t num;
	GFXPropertyMapList list = gfx_material_get_all(
		batch->material,
		&num
	);

	if(source && index < num)
	{
		size_t* units = _gfx_material_get_reserved(
			batch->material,
			groupID,
			&num);

		/* Iterate through units and rebuild them */
		while(num)
		{
			if(!gfx_bucket_rebuild(
				bucket->bucket,
				units[--num],
				source,
				gfx_property_map_list_at(list, index)))
			{
				/* Uh, bail? */
				num = 1;
				break;
			}
		}

		/* Success! */
		if(!num) return;
	}

	/* Remove all units */
	size_t* units = _gfx_material_get_reserved(
		batch->material,
		groupID,
		&num);

	while(num) gfx_bucket_erase(
		bucket->bucket,
		units[--num]);

	_gfx_mesh_remove_group(
		batch->mesh,
		batch->meshID,
		bucket);
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
	return _gfx_material_get_batch_type(
		batch->material,
		batch->materialID
	);
}

/******************************************************/
size_t gfx_batch_get_instances(

		GFXBatch*  batch,
		GFXPipe*   bucket)
{
	/* Get unit group */
	size_t groupID = _gfx_mesh_find_group(
		batch->mesh,
		batch->meshID,
		bucket
	);

	return _gfx_material_get(
		batch->material,
		groupID
	);
}

/******************************************************/
void gfx_batch_set_type(

		GFXBatch*     batch,
		GFXBatchType  type)
{
	_gfx_material_set_batch_type(
		batch->material,
		batch->materialID,
		type
	);
}

/******************************************************/
int gfx_batch_increase(

		GFXBatch*  batch,
		GFXPipe*   bucket,
		size_t     instances)
{
	/* Get units and increase */
	size_t groupID = _gfx_mesh_get_group(
		batch->mesh,
		batch->meshID,
		bucket
	);

	if(!_gfx_material_increase(
		batch->material,
		groupID,
		instances))
	{
		return 0;
	}

	/* Get associated data */
	size_t index = _gfx_material_get_batch_map(
		batch->material,
		batch->materialID);

	size_t num;
	GFXPropertyMapList list = gfx_material_get_all(
		batch->material,
		&num);

	size_t source = _gfx_batch_get_source(batch, bucket);

	/* Validate data */
	if(!source || index >= num)
	{
		_gfx_mesh_remove_group(
			batch->mesh,
			batch->meshID,
			bucket
		);
		return 0;
	}

	/* Calculate the number of wanted units */
	num = _gfx_material_get(
		batch->material,
		groupID);
	num = _gfx_batch_get_num_units(
		num,
		gfx_property_map_list_instances_at(list, index));

	/* Get current number of units and reserve extra ones */
	size_t start;
	_gfx_material_get_reserved(
		batch->material,
		groupID,
		&start);

	size_t* units = _gfx_material_reserve(
		batch->material,
		groupID,
		num);

	if(units)
	{
		/* Iterate through units and add them to the bucket */
		/* Also set the base instance */
		size_t i;
		for(i = start; i < num; ++i)
		{
			units[i] = gfx_bucket_insert(
				bucket->bucket,
				source,
				gfx_property_map_list_at(list, index),
				0, 0
			);

			/* Bail! Bail! */
			if(!units[i]) break;

			gfx_bucket_set_instance_base(
				bucket->bucket,
				units[i],
				gfx_property_map_list_instances_at(list, index) * i
			);
		}

		/* Woop Woop, sneaky success! */
		if(i >= num) return 1;

		/* This isn't going well, destroy them again and unreserve */
		while(i > start) gfx_bucket_erase(
			bucket->bucket,
			units[--i]);

		_gfx_material_reserve(
			batch->material,
			groupID,
			start);
	}

	/* Nevermind D: */
	_gfx_material_decrease(
		batch->material,
		groupID,
		instances
	);

	return 0;
}

/******************************************************/
void gfx_batch_decrease(

		GFXBatch*  batch,
		GFXPipe*   bucket,
		size_t     instances)
{
	/* Get units */
	size_t groupID = _gfx_mesh_find_group(
		batch->mesh,
		batch->meshID,
		bucket
	);

	if(groupID)
	{
		/* Get associated data */
		size_t num;
		GFXPropertyMapList list = gfx_material_get_all(
			batch->material,
			&num);

		num = _gfx_material_get_batch_map(
			batch->material,
			batch->materialID);

		size_t unitSize = gfx_property_map_list_instances_at(
			list, num);

		/* Get the number of remaining instances */
		/* Calculate the max number of instances of the last remaining unit */
		size_t start = _gfx_material_get(batch->material, groupID);
		start = (instances > start) ? 0 : start - instances;

		size_t last = start;
		if(unitSize) last = start % unitSize;

		/* Calculate the number of remaining units */
		start = _gfx_batch_get_num_units(start, unitSize);
		size_t* units = _gfx_material_get_reserved(
			batch->material,
			groupID,
			&num
		);

		if(last)
		{
			/* Fix the last remaining unit */
			unitSize = gfx_bucket_get_instances(
				bucket->bucket,
				units[start - 1]);

			gfx_bucket_set_instances(
				bucket->bucket,
				units[start - 1],
				(last < unitSize) ? last : unitSize);
		}

		/* Iterate through units and erase them */
		while(num > start) gfx_bucket_erase(
			bucket->bucket,
			units[--num]
		);

		/* Decrease and reserve less units */
		if(!_gfx_material_decrease(
			batch->material,
			groupID,
			instances))
		{
			_gfx_mesh_remove_group(
				batch->mesh,
				batch->meshID,
				bucket);
		}

		else _gfx_material_reserve(
			batch->material,
			groupID,
			start
		);
	}
}

/******************************************************/
size_t gfx_batch_set_visible(

		GFXBatch*  batch,
		GFXPipe*   bucket,
		size_t     instances)
{
	/* Get units */
	size_t groupID = _gfx_mesh_find_group(
		batch->mesh,
		batch->meshID,
		bucket
	);

	if(!groupID) return 0;

	/* Get associated data */
	size_t temp;
	GFXPropertyMapList list = gfx_material_get_all(
		batch->material,
		&temp);

	temp = _gfx_material_get_batch_map(
		batch->material,
		batch->materialID);

	size_t unitSize = gfx_property_map_list_instances_at(
		list, temp);

	/* Get reserved units */
	size_t num;
	size_t* units = _gfx_material_get_reserved(
		batch->material,
		groupID,
		&num
	);

	/* Clamp number of instances */
	temp = _gfx_material_get(batch->material, groupID);
	instances = (temp < instances) ? temp : instances;

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
