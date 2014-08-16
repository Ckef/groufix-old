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
#include "groufix/core/errors.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#define GFX_INT_INVALID_BATCH_MAP  UINT_MAX

/******************************************************/
/* Internal material */
struct GFX_Material
{
	GFX_LodMap  map;     /* Super class */
	GFXVector   batches; /* Stores GFX_Batch, index + 1 = material ID of batch */
};

/* Internal batch (mesh reference) */
struct GFX_Batch
{
	GFXSubMesh*   submesh;   /* NULL when empty */
	unsigned int  submeshID;
	unsigned int  map;       /* Direct index of the property map of this batch, out of bounds when empty */
	unsigned int  copy;      /* First copy at the property map in use by this batch */
};

/* Internal Property Map data */
struct GFX_MapData
{
	GFXPropertyMap*  map;       /* Super class */
	size_t           instances; /* Number or renderable instances per unit (constant) */
	unsigned int     copies;    /* Number of copies used by units */
};

/******************************************************/
static struct GFX_MapData* _gfx_material_increase_copies(

		struct GFX_Material*  material,
		unsigned int          materialID,
		long int              diff,
		unsigned int*         upper)
{
	/* Bound check */
	size_t max = gfx_vector_get_size(&material->batches);
	if(!materialID || materialID > max || !diff) return NULL;

	/* Get batch */
	struct GFX_Batch* batch =
		gfx_vector_at(&material->batches, materialID - 1);

	/* Check if the property map exists */
	unsigned int num;
	struct GFX_MapData* data = gfx_lod_map_get_all(
		(GFXLodMap*)material,
		&num
	);

	if(batch->map >= num) return NULL;

	/* Adjust number of copies used */
	data[batch->map].copies += diff;
	*upper = data[batch->map].copies;

	/* Iterate through all batches */
	struct GFX_Batch* it;
	for(
		it = material->batches.begin;
		it != material->batches.end;
		it = gfx_vector_next(&material->batches, it))
	{
		if(
			it != batch &&
			it->map == batch->map &&
			it->copy >= batch->copy)
		{
			/* Increase their starting copy */
			/* Also retrieve the batch following the given batch */
			/* Do this by simply computing the lowest following copy */
			it->copy += diff;
			*upper = (it->copy < *upper) ?
				it->copy : *upper;

			/* And adjust all units */
			_gfx_submesh_set_unit_copies(
				it->submesh,
				it->submeshID,
				it->copy
			);
		}
	}

	return data + batch->map;
}

/******************************************************/
static struct GFX_Batch* _gfx_material_insert_mesh(

		struct GFX_Material*  mat,
		GFXSubMesh*           mesh)
{
	/* Construct new ID */
	struct GFX_Batch new;
	new.submesh   = mesh;
	new.submeshID = 0;
	new.map       = GFX_INT_INVALID_BATCH_MAP;
	new.copy      = 0;

	/* Try to find an empty batch */
	struct GFX_Batch* empty;
	for(
		empty = mat->batches.begin;
		empty != mat->batches.end;
		empty = gfx_vector_next(&mat->batches, empty))
	{
		if(!empty->submesh) break;
	}

	/* Replace an empty ID */
	if(empty != mat->batches.end)
	{
		*empty = new;
		return empty;
	}

	/* Overflow */
	if(gfx_vector_get_size(&mat->batches) == UINT_MAX)
		return mat->batches.end;

	/* Insert new ID */
	return gfx_vector_insert(
		&mat->batches,
		&new,
		empty
	);
}

/******************************************************/
static int _gfx_material_update_batch_map(

		struct GFX_Material*  mat,
		struct GFX_Batch*     batch)
{
	/* Set to default (no map) */
	batch->map = GFX_INT_INVALID_BATCH_MAP;

	/* Get parameters */
	const GFX_BatchData* data =
		_gfx_submesh_get_batch_data(batch->submesh, batch->submeshID);

	/* Get map index */
	unsigned int index =
		gfx_lod_map_count((GFXLodMap*)mat, data->params.level) +
		data->params.index;

	if(index < gfx_lod_map_count((GFXLodMap*)mat, data->params.level + 1))
	{
		batch->map = index;
		return 1;
	}

	return 0;
}

/******************************************************/
unsigned int _gfx_material_insert_batch(

		GFXMaterial*  material,
		GFXSubMesh*   mesh)
{
	struct GFX_Material* internal =
		(struct GFX_Material*)material;
	struct GFX_Batch* batch =
		_gfx_material_insert_mesh(internal, mesh);

	/* Return correct ID */
	return (batch == internal->batches.end) ? 0 :
		gfx_vector_get_index(&internal->batches, batch) + 1;
}

/******************************************************/
int _gfx_material_set_batch(

		GFXMaterial*  material,
		unsigned int  materialID,
		unsigned int  submeshID)
{
	/* Bound check */
	struct GFX_Material* internal =
		(struct GFX_Material*)material;
	size_t max =
		gfx_vector_get_size(&internal->batches);

	if(materialID && materialID <= max)
	{
		/* Get batch and set, also update the map index */
		struct GFX_Batch* batch =
			gfx_vector_at(&internal->batches, materialID - 1);

		if(batch->submesh)
		{
			batch->submeshID = submeshID;
			return _gfx_material_update_batch_map(internal, batch);
		}
	}

	return 0;
}

/******************************************************/
void _gfx_material_remove_batch(

		GFXMaterial*  material,
		unsigned int  materialID)
{
	/* Bound check */
	struct GFX_Material* internal =
		(struct GFX_Material*)material;
	size_t size =
		gfx_vector_get_size(&internal->batches);

	if(materialID && materialID <= size)
	{
		/* Get batch and mark as empty */
		struct GFX_Batch* batch =
			gfx_vector_at(&internal->batches, materialID - 1);

		batch->submesh = NULL;
		batch->map = GFX_INT_INVALID_BATCH_MAP;

		/* Remove trailing empty batches */
		size_t num;
		struct GFX_Batch* beg = internal->batches.end;

		for(num = 0; num < size; ++num)
		{
			struct GFX_Batch* prev =
				gfx_vector_previous(&internal->batches, beg);

			if(prev->submesh) break;
			beg = prev;
		}
		gfx_vector_erase_range(&internal->batches, num, beg);
	}
}

/******************************************************/
unsigned int _gfx_material_get_batch_map(

		GFXMaterial*  material,
		unsigned int  materialID)
{
	/* Bound check */
	struct GFX_Material* internal =
		(struct GFX_Material*)material;
	size_t max =
		gfx_vector_get_size(&internal->batches);

	if(!materialID || materialID > max)
		return GFX_INT_INVALID_BATCH_MAP;

	/* Get batch */
	struct GFX_Batch* batch =
		gfx_vector_at(&internal->batches, materialID - 1);

	return batch->map;
}

/******************************************************/
unsigned int _gfx_material_get_batch_copy(

		GFXMaterial*  material,
		unsigned int  materialID)
{
	/* Get batch */
	struct GFX_Material* internal =
		(struct GFX_Material*)material;
	size_t max =
		gfx_vector_get_size(&internal->batches);

	if(!materialID || materialID > max)
		return 0;

	/* Get batch */
	struct GFX_Batch* batch =
		gfx_vector_at(&internal->batches, materialID - 1);

	return batch->copy;
}

/******************************************************/
int _gfx_material_increase_batch_copies(

		GFXMaterial*  material,
		unsigned int  materialID,
		unsigned int  copies)
{
	struct GFX_Material* internal =
		(struct GFX_Material*)material;

	/* Increase */
	unsigned int upper;
	struct GFX_MapData* data = _gfx_material_increase_copies(
		internal,
		materialID,
		copies,
		&upper
	);

	if(!data) return 0;

	/* Expand property map if needed */
	if(data->map->copies < data->copies)
		gfx_property_map_expand(
			data->map,
			data->copies - data->map->copies
		);

	/* Move copies to higher copies */
	/* Start at the upper bound */
	unsigned int dest;
	for(dest = data->copies; dest > upper; --dest)
		gfx_property_map_move(
			data->map,
			dest - 1,
			dest - 1 - copies
		);

	return 1;
}

/******************************************************/
int _gfx_material_decrease_batch_copies(

		GFXMaterial*  material,
		unsigned int  materialID,
		unsigned int  copies)
{
	struct GFX_Material* internal =
		(struct GFX_Material*)material;

	/* Decrease */
	unsigned int lower;
	struct GFX_MapData* data = _gfx_material_increase_copies(
		internal,
		materialID,
		-(long int)copies,
		&lower
	);

	if(!data) return 0;

	/* Move copies to lower copies */
	/* Start at the lower bound */
	unsigned int lim = data->copies + copies;
	unsigned int src;

	for(src = lower + copies; src < lim; ++src)
		gfx_property_map_move(
			data->map,
			src - copies,
			src
		);

	return 1;
}

/******************************************************/
GFXMaterial* gfx_material_create(void)
{
	/* Allocate material */
	struct GFX_Material* mat = malloc(sizeof(struct GFX_Material));
	if(!mat)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Material could not be allocated."
		);
		return NULL;
	}

	/* Initialize */
	_gfx_lod_map_init(
		(GFX_LodMap*)mat,
		0,
		sizeof(struct GFX_MapData),
		sizeof(GFXPropertyMap*)
	);

	gfx_vector_init(&mat->batches, sizeof(struct GFX_Batch));

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
			if(it->submesh) _gfx_submesh_remove_batch(it->submesh, it->submeshID);
		}

		/* Free all property maps */
		unsigned int num;
		struct GFX_MapData* maps = gfx_lod_map_get_all(
			(GFXLodMap*)material,
			&num
		);

		while(num) gfx_property_map_free(maps[--num].map);

		/* Clear and free */
		gfx_vector_clear(&internal->batches);
		_gfx_lod_map_clear((GFX_LodMap*)internal);

		free(material);
	}
}

/******************************************************/
GFXPropertyMap* gfx_material_add(

		GFXMaterial*   material,
		unsigned int   level,
		GFXProgram*    program,
		unsigned char  properties,
		size_t         instances)
{
	/* Compute index of the map */
	unsigned int ind = gfx_lod_map_count(
		(GFXLodMap*)material,
		level + 1
	);

	/* Create new property map */
	struct GFX_MapData data;
	data.instances = instances;
	data.copies = 0;

	data.map = gfx_property_map_create(program, properties);
	if(!data.map) return NULL;

	/* Add it to the LOD map */
	if(!gfx_lod_map_add((GFXLodMap*)material, level, &data))
	{
		gfx_property_map_free(data.map);
		return NULL;
	}

	/* Iterate through all batches */
	struct GFX_Batch* it;
	struct GFX_Material* internal = (struct GFX_Material*)material;

	for(
		it = internal->batches.begin;
		it != internal->batches.end;
		it = gfx_vector_next(&internal->batches, it))
	{
		/* Increase map index if necessary */
		if(it->submesh && it->map != GFX_INT_INVALID_BATCH_MAP && it->map >= ind)
			++it->map;
	}

	return data.map;
}

/******************************************************/
GFXPropertyMapList gfx_material_get(

		GFXMaterial*   material,
		unsigned int   level,
		unsigned int*  num)
{
	return gfx_lod_map_get((GFXLodMap*)material, level, num);
}

/******************************************************/
GFXPropertyMapList gfx_material_get_all(

		GFXMaterial*   material,
		unsigned int*  num)
{
	return gfx_lod_map_get_all((GFXLodMap*)material, num);
}

/******************************************************/
size_t gfx_property_map_list_instances_at(

		GFXPropertyMapList  list,
		unsigned int        index)
{
	return ((struct GFX_MapData*)list)[index].instances;
}

/******************************************************/
unsigned int gfx_property_map_list_copies_at(

		GFXPropertyMapList  list,
		unsigned int        index)
{
	return ((struct GFX_MapData*)list)[index].copies;
}

/******************************************************/
GFXPropertyMap* gfx_property_map_list_at(

		GFXPropertyMapList  list,
		unsigned int        index)
{
	return ((struct GFX_MapData*)list)[index].map;
}
