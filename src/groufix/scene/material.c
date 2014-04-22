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
	GFXVector  units;
	GFXVector  buckets;
};

/******************************************************/
GFXMaterial* gfx_material_create(void)
{
	/* Allocate material */
	struct GFX_Material* mat = calloc(1, sizeof(struct GFX_Material));
	if(!mat) return NULL;

	_gfx_lod_map_init(
		(GFX_LodMap*)mat,
		sizeof(GFXPropertyMap*),
		sizeof(GFXPropertyMap*)
	);

	return (GFXMaterial*)mat;
}

/******************************************************/
void gfx_material_free(

		GFXMaterial* material)
{
	if(material)
	{
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
void gfx_material_remove(

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
			break;
		}
	}
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
