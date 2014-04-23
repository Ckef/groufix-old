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

#include "groufix/scene.h"
#include "groufix/scene/internal.h"

#include <stdlib.h>

/******************************************************/
/* Internal material */
struct GFX_Material
{
	/* Super class */
	GFX_LodMap map;

	/* Hidden data */
	GFXVector  units;   /* Stores GFX_Unit */
	GFXVector  buckets; /* Stores (GFXPipe* + size_t (upper bound in units) * propertyMaps) */
};

/* Internal unit */
struct GFX_Unit
{
	size_t  src;  /* Sort key */
	size_t  unit; /* Actual bucket unit */
};

/******************************************************/
static inline size_t _gfx_material_num_property_maps(

		GFXLodMap* map)
{
	return gfx_lod_map_count(map, map->levels);
}

/******************************************************/
static void _gfx_material_get_boundaries(

		struct GFX_Material*  material,
		GFXVectorIterator     bucket,
		size_t                index,
		size_t*               begin,
		size_t*               end)
{
	size_t* it = ((size_t*)(((GFXPipe**)bucket) + 1)) + index;
	*end = *it;

	/* Get previous upper bound as begin */
	if(index) *begin = *(it - 1);

	/* Get start as begin */
	else if(bucket == material->buckets.begin) *begin = 0;

	/* Get upper bound of previous buffer as begin */
	else *begin = *(((size_t*)bucket) - 1);
}

/******************************************************/
static GFXVectorIterator _gfx_material_find_bucket(

		struct GFX_Material*  material,
		GFXPipe*              pipe)
{
	/* Get bucket iterator size */
	GFXVectorIterator it;
	int bucketSize =
		sizeof(GFXPipe*) +
		sizeof(size_t) *
		_gfx_material_num_property_maps((GFXLodMap*)material);

	/* Iterate and find */
	for(
		it = material->buckets.begin;
		it != material->buckets.end;
		it = gfx_vector_advance(&material->buckets, it, bucketSize))
	{
		if(*(GFXPipe**)it == pipe) break;
	}

	return it;
}

/******************************************************/
GFXMaterial* gfx_material_create(void)
{
	/* Allocate material */
	struct GFX_Material* mat = calloc(1, sizeof(struct GFX_Material));
	if(!mat) return NULL;

	/* Initialize */
	_gfx_lod_map_init(
		(GFX_LodMap*)mat,
		sizeof(GFXPropertyMap*),
		sizeof(GFXPropertyMap*)
	);

	gfx_vector_init(&mat->units, sizeof(struct GFX_Unit));
	gfx_vector_init(&mat->buckets, 1);

	return (GFXMaterial*)mat;
}

/******************************************************/
void gfx_material_free(

		GFXMaterial* material)
{
	if(material)
	{
		struct GFX_Material* internal = (struct GFX_Material*)material;

		/* Iterate over all levels */
		size_t levels = material->lodMap.levels;

		while(levels)
		{
			/* Free all property maps in it */
			size_t num;
			GFXPropertyMap** data = gfx_lod_map_get(
				(GFXLodMap*)material,
				--levels,
				&num
			);

			while(num) gfx_property_map_free(data[--num]);
		}

		gfx_vector_clear(&internal->units);
		gfx_vector_clear(&internal->buckets);
		_gfx_lod_map_clear((GFX_LodMap*)material);

		free(material);
	}
}

/******************************************************/
GFXPropertyMap* gfx_material_add(

		GFXMaterial*   material,
		size_t         level,
		GFXProgram*    program,
		unsigned char  properties)
{
	/* Create new property map and add it to the LOD map */
	GFXPropertyMap* map = gfx_property_map_create(program, properties);
	if(!map) return NULL;

	if(!gfx_lod_map_add((GFXLodMap*)material, level, &map))
	{
		gfx_property_map_free(map);
		return NULL;
	}

	return map;
}

/******************************************************/
int gfx_material_remove(

		GFXMaterial*     material,
		GFXPropertyMap*  map)
{
	/* Find level and remove it */
	size_t levels = material->lodMap.levels;

	while(levels)
	{
		if(gfx_lod_map_remove((GFXLodMap*)material, --levels, &map))
		{
			gfx_property_map_free(map);
			return 1;
		}
	}
	return 0;
}

/******************************************************/
int gfx_material_remove_at(

		GFXMaterial*  material,
		size_t        level,
		size_t        index)
{
	/* First get the property map */
	size_t num;
	GFXPropertyMap** data = gfx_lod_map_get(
		(GFXLodMap*)material,
		level,
		&num
	);

	if(index >= num) return 0;

	/* Try to remove it */
	GFXPropertyMap* map = data[index];

	if(gfx_lod_map_remove_at((GFXLodMap*)material, level, index))
	{
		gfx_property_map_free(map);
		return 1;
	}
	return 0;
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
GFXPropertyMap* gfx_property_map_list_at(

		GFXPropertyMapList  list,
		size_t              index)
{
	return ((GFXPropertyMap**)list)[index];
}
