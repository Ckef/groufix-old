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

#include "groufix/core/errors.h"
#include "groufix/scene/material.h"

/******************************************************/
/* Internal Property Map data */
typedef struct GFX_MapData
{
	GFXPropertyMap*  map;    /* Super class */
	unsigned int     copies; /* Number of copies used by units */

} GFX_MapData;


/******************************************************/
GFXMaterial* gfx_material_create(void)
{
	/* Allocate */
	return (GFXMaterial*)gfx_lod_map_create(
		0,
		sizeof(GFX_MapData),
		sizeof(GFXPropertyMap*)
	);
}

/******************************************************/
void gfx_material_free(

		GFXMaterial* material)
{
	if(material)
	{
		/* Free all property maps */
		unsigned int num;
		GFX_MapData* maps = gfx_lod_map_get_all(
			(GFXLodMap*)material,
			&num
		);

		while(num--) gfx_property_map_free(maps[num].map);

		/* Free */
		gfx_lod_map_free((GFXLodMap*)material);
	}
}

/******************************************************/
GFXPropertyMap* gfx_material_add(

		GFXMaterial*    material,
		unsigned int    level,
		GFXProgramMap*  programMap,
		unsigned char   properties)
{
	/* Create new property map */
	GFX_MapData data;
	data.copies = 0;

	data.map = gfx_property_map_create(programMap, properties);
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
unsigned int gfx_property_map_list_copies_at(

		GFXPropertyMapList  list,
		unsigned int        index)
{
	return ((GFX_MapData*)list)[index].copies;
}

/******************************************************/
GFXPropertyMap* gfx_property_map_list_at(

		GFXPropertyMapList  list,
		unsigned int        index)
{
	return ((GFX_MapData*)list)[index].map;
}
