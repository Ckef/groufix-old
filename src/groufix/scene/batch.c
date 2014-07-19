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
static int _gfx_batch_get_data(

		GFXBatch*         batch,
		GFXPipe*          bucket,
		size_t*           source,
		GFXPropertyMap**  map,
		size_t*           unitSize)
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
	*source = _gfx_submesh_get_bucket_source(
		gfx_submesh_list_at(subs, params.index),
		bucket,
		gfx_submesh_list_source_at(subs, params.index)
	);

	/* Get property map */
	GFXPropertyMapList maps = gfx_material_get(
		batch->material,
		params.material,
		&num
	);

	size_t index = gfx_submesh_list_material_at(subs, params.index);
	if(index >= num) return 0;

	/* Get property map and instances per unit */
	*map = gfx_property_map_list_at(maps, index);
	*unitSize = gfx_property_map_list_instances_at(maps, index);

	return 1;
}

/******************************************************/
static void _gfx_batch_decrease(

		GFXBatch*  batch,
		GFXPipe*   bucket,
		size_t     groupID,
		size_t     instances)
{
	if(!_gfx_material_decrease(
		batch->material,
		groupID,
		instances))
	{
		_gfx_mesh_remove_group(
			batch->mesh,
			batch->meshID,
			bucket
		);
	}
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
	size_t groupID = _gfx_mesh_get_group(
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
	/* Get units and current instance counter */
	size_t groupID = _gfx_mesh_get_group(
		batch->mesh,
		batch->meshID,
		bucket);

	size_t start = _gfx_material_get(
		batch->material,
		groupID);

	/* Increase instances */
	if(!_gfx_material_increase(
		batch->material,
		groupID,
		instances))
	{
		return 0;
	}

	/* Fetch data associated with the units */
	size_t source;
	GFXPropertyMap* map;
	size_t unitSize;

	if(_gfx_batch_get_data(
		batch,
		bucket,
		&source,
		&map,
		&unitSize))
	{
		/* Calculate the number of units and reserve them */
		size_t num = _gfx_material_get(batch->material, groupID);
		num = _gfx_batch_get_num_units(num, unitSize);

		size_t* units = _gfx_material_reserve(
			batch->material,
			groupID,
			num);

		if(units)
		{
			/* Iterate through units and add them to the bucket */
			while(start < num) units[start++] =
				gfx_bucket_insert(
					bucket->bucket,
					source,
					map,
					0, 0
				);

			return 1;
		}
	}

	/* Nevermind those instances */
	_gfx_batch_decrease(batch, bucket, groupID, instances);

	return 0;
}

/******************************************************/
void gfx_batch_decrease(

		GFXBatch*  batch,
		GFXPipe*   bucket,
		size_t     instances)
{
	/* Get units */
	size_t groupID = _gfx_mesh_get_group(
		batch->mesh,
		batch->meshID,
		bucket
	);

	/* Fetch data associated with the units */
	size_t temp;
	GFXPropertyMap* map;
	size_t unitSize;

	if(_gfx_batch_get_data(
		batch,
		bucket,
		&temp,
		&map,
		&unitSize))
	{
		/* Get the number of remaining instances */
		/* Calculate the max number of instances of the last remaining unit */
		size_t start = _gfx_material_get(batch->material, groupID);
		start = (instances > start) ? 0 : start - instances;

		size_t last = start;
		if(unitSize) last = start % unitSize;

		/* Calculate the number of remaining units */
		start = _gfx_batch_get_num_units(start, unitSize);

		size_t num;
		size_t* units = _gfx_material_get_reserved(
			batch->material,
			groupID,
			&num);

		if(last)
		{
			/* Fix the last remaining unit */
			temp = gfx_bucket_get_instances(
				bucket->bucket,
				units[start - 1]);

			gfx_bucket_set_instances(
				bucket->bucket,
				units[start - 1],
				last < temp ? last : temp);
		}

		/* Iterate through units and erase them */
		while(start < num) gfx_bucket_erase(
			bucket->bucket,
			units[start++]
		);
	}

	/* Decrease instances */
	_gfx_batch_decrease(batch, bucket, groupID, instances);
}

/******************************************************/
size_t gfx_batch_set_visible(

		GFXBatch*  batch,
		GFXPipe*   bucket,
		size_t     instances)
{
	/* Get units */
	size_t groupID = _gfx_mesh_get_group(
		batch->mesh,
		batch->meshID,
		bucket
	);

	/* Fetch data associated with the units */
	size_t temp;
	GFXPropertyMap* map;
	size_t unitSize;

	if(!_gfx_batch_get_data(
		batch,
		bucket,
		&temp,
		&map,
		&unitSize))
	{
		return 0;
	}

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
	size_t ret = instances;

	/* Iterate and set visibility */
	for(temp = 0; temp < num; ++temp)
	{
		unitSize = (unitSize < instances) ? unitSize : instances;
		instances -= unitSize;

		gfx_bucket_set_visible(
			bucket->bucket,
			units[temp],
			unitSize);

		gfx_bucket_set_instances(
			bucket->bucket,
			units[temp],
			unitSize);
	}

	return ret;
}
