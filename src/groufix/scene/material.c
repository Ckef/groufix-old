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

#include "groufix/scene/material.h"

/******************************************************/
GFXMaterial* gfx_material_create(void)
{
	return (GFXMaterial*)gfx_lod_map_create();
}

/******************************************************/
void gfx_material_free(GFXMaterial* material)
{
	if(material)
	{
		/* Iterate over all levels */
		size_t levels = material->lodMap.levels;

		while(levels)
		{
			/* Free all property maps in it */
			size_t num;
			GFXPropertyMap** maps = gfx_material_get(material, --levels, &num);

			while(num) gfx_property_map_free(maps[--num]);
		}

		gfx_lod_map_free((GFXLodMap*)material);
	}
}

/******************************************************/
GFXPropertyMap* gfx_material_add(GFXMaterial* material, size_t level, GFXProgram* program, unsigned char properties)
{
	/* Create new property map and add it to the LOD map */
	GFXPropertyMap* map = gfx_property_map_create(program, properties);
	if(!map) return NULL;

	if(!gfx_lod_map_add((GFXLodMap*)material, level, map))
	{
		gfx_property_map_free(map);
		return NULL;
	}

	return map;
}

/******************************************************/
void gfx_material_remove(GFXMaterial* material, GFXPropertyMap* map)
{
	/* Remove it from all levels */
	size_t levels = material->lodMap.levels;

	while(levels)
	{
		if(gfx_lod_map_remove((GFXLodMap*)material, --levels, map))
			gfx_property_map_free(map);
	}
}
