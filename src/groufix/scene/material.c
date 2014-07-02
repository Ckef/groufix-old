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

#include "groufix/scene/internal.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Internal material */
struct GFX_Material
{
	/* Super class */
	GFX_LodMap map;

	/* Hidden data */
	GFXVector  batches; /* Stores GFX_Batch, index + 1 = material ID of batch */
	GFXVector  groups;  /* Stores GFX_Group, index + 1 = ID */
	GFXVector  units;   /* Stores size_t */
};

/* Internal batch (mesh reference) */
struct GFX_Batch
{
	GFXMesh*      mesh;  /* NULL when empty */
	size_t        meshID;
	GFXBatchType  type;
};

/* Internal unit group */
struct GFX_Group
{
	size_t  batch;     /* Material ID, 0 when empty */
	size_t  instances; /* Number of existing instances */
	size_t  upper;     /* Upper bound in units vector */
};

/* Internal Property Map data */
struct GFX_MapData
{
	GFXPropertyMap*  map;       /* Super class */
	size_t           instances; /* Number or renderable instances per unit (constant) */
	size_t           copies;    /* Number of copies used by units */
};

/******************************************************/
static void _gfx_material_get_unit_bounds(

		struct GFX_Material*  mat,
		struct GFX_Group*     group,
		size_t*               begin,
		size_t*               end)
{
	*end = group->upper;

	if(group != mat->groups.begin)
	{
		group = gfx_vector_previous(&mat->groups, group);
		*begin = group->upper;
	}
	else *begin = 0;
}

/******************************************************/
static struct GFX_MapData* _gfx_material_get_group_map(

		struct GFX_Material*  material,
		struct GFX_Group*     group)
{
	if(!group->batch) return NULL;

	/* Get batch & parameters */
	struct GFX_Batch* batch = gfx_vector_at(
		&material->batches,
		group->batch - 1);

	GFXBatchLod params;
	_gfx_mesh_get_batch_lod(
		batch->mesh,
		batch->meshID,
		&params);

	/* Get submesh */
	size_t num;
	GFXSubMeshList list = gfx_mesh_get(
		batch->mesh,
		params.mesh,
		&num);

	if(params.index >= num) return NULL;

	/* Get material index */
	size_t index = gfx_submesh_list_material_at(
		list,
		params.index);

	/* Get property map */
	struct GFX_MapData* maps = gfx_lod_map_get(
		(GFXLodMap*)material,
		params.material,
		&num);

	return (index >= num) ? NULL : maps + index;
}

/******************************************************/
static size_t _gfx_material_get_group_size(

		struct GFX_Material*  material,
		struct GFX_Group*     group)
{
	if(!group->instances) return 0;

	/* Get the property map associated with the group */
	struct GFX_MapData* map = _gfx_material_get_group_map(material, group);
	if(!map) return 0;

	/* Calculate the number of units */
	/* Divide and round up to get the number of units required */
	if(!map->instances) return 1;
	return (group->instances - 1) / map->instances + 1;
}

/******************************************************/
static int _gfx_material_reserve_units(

		struct GFX_Material*  material,
		struct GFX_Group*     group)
{
	size_t begin;
	size_t end;
	_gfx_material_get_unit_bounds(material, group, &begin, &end);

	size_t units = _gfx_material_get_group_size(material, group);
	long int diff = (long int)units - (long int)(end - begin);

	/* Insert new units */
	if(diff > 0)
	{
		GFXVectorIterator it = gfx_vector_insert_range_at(
			&material->units,
			diff,
			NULL,
			end
		);

		if(it == material->units.end) return 0;

		/* Initialize all to 0 */
		memset(it, 0, (size_t)diff * sizeof(size_t));
	}

	/* Erase units */
	else gfx_vector_erase_range_at(
		&material->units,
		-diff,
		end + diff
	);

	/* Adjust bounds */
	while(group != material->groups.end)
	{
		group->upper += diff;
		group = gfx_vector_next(&material->groups, group);
	}

	return 1;
}

/******************************************************/
static struct GFX_Batch* _gfx_material_insert_mesh(

		struct GFX_Material*  mat,
		GFXMesh*              mesh)
{
	/* Construct new ID */
	struct GFX_Batch new;
	new.mesh   = mesh;
	new.meshID = 0;
	new.type   = GFX_BATCH_DEFAULT;

	/* Try to find an empty batch */
	struct GFX_Batch* empty;
	for(
		empty = mat->batches.begin;
		empty != mat->batches.end;
		empty = gfx_vector_next(&mat->batches, empty))
	{
		if(!empty->mesh) break;
	}

	/* Replace an empty ID */
	if(empty != mat->batches.end)
	{
		*empty = new;
		return empty;
	}

	/* Insert new ID */
	return gfx_vector_insert(
		&mat->batches,
		&new,
		empty
	);
}

/******************************************************/
size_t _gfx_material_insert_batch(

		GFXMaterial*  material,
		GFXMesh*      mesh)
{
	struct GFX_Material* internal = (struct GFX_Material*)material;
	struct GFX_Batch* batch = _gfx_material_insert_mesh(internal, mesh);

	/* Return correct ID */
	return (batch == internal->batches.end) ? 0 :
		gfx_vector_get_index(&internal->batches, batch) + 1;
}

/******************************************************/
void _gfx_material_set_batch(

		GFXMaterial*  material,
		size_t        materialID,
		size_t        meshID)
{
	/* Bound check */
	struct GFX_Material* internal = (struct GFX_Material*)material;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(materialID && materialID <= max)
	{
		/* Get batch and set */
		struct GFX_Batch* batch = gfx_vector_at(
			&internal->batches,
			materialID - 1
		);

		batch->meshID = meshID;
	}
}

/******************************************************/
void _gfx_material_remove_batch(

		GFXMaterial*  material,
		size_t        materialID)
{
	/* Bound check */
	struct GFX_Material* internal = (struct GFX_Material*)material;
	size_t size = gfx_vector_get_size(&internal->batches);

	if(materialID && materialID <= size)
	{
		/* Get batch and mark as empty */
		struct GFX_Batch* batch = gfx_vector_at(
			&internal->batches,
			materialID - 1
		);

		batch->type = GFX_BATCH_DEFAULT;
		batch->mesh = NULL;

		/* Remove trailing empty batches */
		size_t num;
		struct GFX_Batch* beg = internal->batches.end;

		for(num = 0; num < size; ++num)
		{
			struct GFX_Batch* prev = gfx_vector_previous(&internal->batches, beg);
			if(prev->mesh) break;

			beg = prev;
		}
		gfx_vector_erase_range(&internal->batches, num, beg);
	}
}

/******************************************************/
GFXBatchType _gfx_material_get_batch_type(

		GFXMaterial*  material,
		size_t        materialID)
{
	/* Bound check */
	struct GFX_Material* internal = (struct GFX_Material*)material;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(!materialID || materialID > max) return GFX_BATCH_DEFAULT;

	/* Get batch */
	struct GFX_Batch* batch = gfx_vector_at(
		&internal->batches,
		materialID - 1
	);

	return batch->type;
}

/******************************************************/
void _gfx_material_set_batch_type(

		GFXMaterial*  material,
		size_t        materialID,
		GFXBatchType  type)
{
	/* Bound check */
	struct GFX_Material* internal = (struct GFX_Material*)material;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(materialID && materialID <= max)
	{
		/* Get batch and set */
		struct GFX_Batch* batch = gfx_vector_at(
			&internal->batches,
			materialID - 1
		);

		if(batch->mesh) batch->type = type;
	}
}

/******************************************************/
size_t _gfx_material_insert_group(

		GFXMaterial*  material,
		size_t        materialID)
{
	/* Derp */
	if(!materialID) return 0;

	/* Try to find an empty group */
	struct GFX_Material* internal = (struct GFX_Material*)material;
	struct GFX_Group* it;

	for(
		it = internal->groups.begin;
		it != internal->groups.end;
		it = gfx_vector_next(&internal->groups, it))
	{
		if(!(it->batch)) break;
	}

	/* Construct new group and insert */
	struct GFX_Group group;
	group.batch = materialID;
	group.instances = 0;

	if(it != internal->groups.end)
	{
		/* Replace an empty group */
		group.upper = it->upper;
		*it = group;
	}
	else
	{
		/* Insert a new group */
		group.upper = gfx_vector_get_size(&internal->units);
		it = gfx_vector_insert(&internal->groups, &group, it);
	}

	return (it == internal->groups.end) ? 0 :
		gfx_vector_get_index(&internal->groups, it) + 1;
}

/******************************************************/
void _gfx_material_remove_group(

		GFXMaterial*  material,
		size_t        groupID)
{
	/* Bound check */
	struct GFX_Material* internal = (struct GFX_Material*)material;
	size_t size = gfx_vector_get_size(&internal->groups);

	if(groupID && groupID <= size)
	{
		/* Get group */
		struct GFX_Group* group = gfx_vector_at(
			&internal->groups,
			groupID - 1
		);

		/* Erase units */
		group->instances = 0;
		_gfx_material_reserve_units(internal, group);

		/* Mark as empty and remove trailing empty groups */
		group->batch = 0;

		size_t num;
		struct GFX_Group* beg = internal->groups.end;

		for(num = 0; num < size; ++num)
		{
			struct GFX_Group* prev = gfx_vector_previous(&internal->groups, beg);
			if(prev->batch) break;

			beg = prev;
		}
		gfx_vector_erase_range(&internal->groups, num, beg);
	}
}

/******************************************************/
size_t* _gfx_material_get_group(

		GFXMaterial*  material,
		size_t        groupID,
		size_t*       units)
{
	/* Bound check */
	struct GFX_Material* internal = (struct GFX_Material*)material;
	size_t max = gfx_vector_get_size(&internal->groups);

	if(!groupID || groupID > max)
	{
		*units = 0;
		return NULL;
	}

	/* Get group */
	struct GFX_Group* group = gfx_vector_at(
		&internal->groups,
		groupID - 1
	);

	/* Return correct values */
	size_t begin;
	size_t end;
	_gfx_material_get_unit_bounds(internal, group, &begin, &end);

	*units = end - begin;
	return gfx_vector_at(&internal->units, begin);
}

/******************************************************/
int _gfx_material_increase(

		GFXMaterial*  material,
		size_t        groupID,
		size_t        instances)
{
	/* Bound check */
	struct GFX_Material* internal = (struct GFX_Material*)material;
	size_t max = gfx_vector_get_size(&internal->groups);

	if(!groupID || groupID > max) return 0;

	/* Get group */
	struct GFX_Group* group = gfx_vector_at(
		&internal->groups,
		groupID - 1
	);

	/* Check for overflow */
	if(SIZE_MAX - instances < group->instances) return 0;
	group->instances += instances;

	/* Reserve the correct amount of units */
	if(!_gfx_material_reserve_units(internal, group))
	{
		group->instances -= instances;
		return 0;
	}

	return 1;
}

/******************************************************/
size_t _gfx_material_get(

		GFXMaterial*  material,
		size_t        groupID)
{
	/* Bound check */
	struct GFX_Material* internal = (struct GFX_Material*)material;
	size_t max = gfx_vector_get_size(&internal->groups);

	if(!groupID || groupID > max) return 0;

	/* Get group */
	struct GFX_Group* group = gfx_vector_at(
		&internal->groups,
		groupID - 1
	);

	return group->instances;
}

/******************************************************/
int _gfx_material_decrease(

		GFXMaterial*  material,
		size_t        groupID,
		size_t        instances)
{
	/* Bound check */
	struct GFX_Material* internal = (struct GFX_Material*)material;
	size_t max = gfx_vector_get_size(&internal->groups);

	if(!groupID || groupID > max) return 0;

	/* Get group and decrease */
	struct GFX_Group* group = gfx_vector_at(
		&internal->groups,
		groupID - 1
	);

	group->instances = (instances > group->instances) ? 0 :
		group->instances - instances;

	/* Reserve the correct amount of units */
	_gfx_material_reserve_units(internal, group);

	return group->instances ? 1 : 0;
}

/******************************************************/
GFXMaterial* gfx_material_create(void)
{
	/* Allocate material */
	struct GFX_Material* mat = malloc(sizeof(struct GFX_Material));
	if(!mat) return NULL;

	/* Initialize */
	_gfx_lod_map_init(
		(GFX_LodMap*)mat,
		0,
		sizeof(struct GFX_MapData),
		sizeof(GFXPropertyMap*)
	);

	gfx_vector_init(&mat->batches, sizeof(struct GFX_Batch));
	gfx_vector_init(&mat->groups, sizeof(struct GFX_Group));
	gfx_vector_init(&mat->units, sizeof(size_t));

	return (GFXMaterial*)mat;
}

/******************************************************/
void gfx_material_free(

		GFXMaterial* material)
{
	if(material)
	{
		struct GFX_Material* internal = (struct GFX_Material*)material;

		/* Remove all batches at the meshes */
		struct GFX_Batch* it;
		for(
			it = internal->batches.begin;
			it != internal->batches.end;
			it = gfx_vector_next(&internal->batches, it))
		{
			if(it->mesh) _gfx_mesh_remove_batch(
				it->mesh,
				it->meshID
			);
		}

		/* Free all property maps */
		size_t num;
		struct GFX_MapData* maps = gfx_lod_map_get_all(
			(GFXLodMap*)material,
			&num
		);

		while(num) gfx_property_map_free(maps[--num].map);

		/* Clear and free */
		gfx_vector_clear(&internal->batches);
		gfx_vector_clear(&internal->groups);
		gfx_vector_clear(&internal->units);
		_gfx_lod_map_clear((GFX_LodMap*)internal);

		free(material);
	}
}

/******************************************************/
GFXPropertyMap* gfx_material_add(

		GFXMaterial*   material,
		size_t         level,
		GFXProgram*    program,
		unsigned char  properties,
		size_t         instances)
{
	/* Create new property map */
	struct GFX_MapData data;
	data.instances = instances;
	data.copies    = 0;

	data.map = gfx_property_map_create(program, properties);
	if(!data.map) return NULL;

	/* Add it to the LOD map */
	if(!gfx_lod_map_add((GFXLodMap*)material, level, &data))
	{
		gfx_property_map_free(data.map);
		return NULL;
	}

	return data.map;
}

/******************************************************/
GFXPropertyMapList gfx_material_get(

		GFXMaterial*  material,
		size_t        level,
		size_t*       num)
{
	return gfx_lod_map_get((GFXLodMap*)material, level, num);
}

/******************************************************/
GFXPropertyMapList gfx_material_get_all(

		GFXMaterial*  material,
		size_t*       num)
{
	return gfx_lod_map_get_all((GFXLodMap*)material, num);
}

/******************************************************/
size_t gfx_property_map_list_instances_at(

		GFXPropertyMapList  list,
		size_t              index)
{
	return ((struct GFX_MapData*)list)[index].instances;
}

/******************************************************/
GFXPropertyMap* gfx_property_map_list_at(

		GFXPropertyMapList  list,
		size_t              index)
{
	return ((struct GFX_MapData*)list)[index].map;
}
