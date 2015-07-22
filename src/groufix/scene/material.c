/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/containers/vector.h"
#include "groufix/core/errors.h"
#include "groufix/scene/material.h"

#include <limits.h>
#include <stdlib.h>

/******************************************************/
/* Internal Property Map data */
typedef struct GFX_MapData
{
	GFXPropertyMap*  map;      /* Super class */
	unsigned int     copies;   /* Number of copies used by units */
	GFXVector        segments; /* Takes segments of the copy range */

} GFX_MapData;


/* Internal segment */
typedef struct GFX_Segment
{
	unsigned int ref;    /* Reference count */
	unsigned int offset; /* Sort key */
	unsigned int num;

} GFX_Segment;


/******************************************************/
static int _gfx_material_segment_comp(

		const void*  key,
		const void*  elem)
{
	unsigned int offset = GFX_VOID_TO_UINT(key);
	unsigned int found = ((const GFX_Segment*)elem)->offset;

	if(found < offset) return 1;
	if(found > offset) return -1;

	return 0;
}

/******************************************************/
static inline GFX_Segment* _gfx_material_find_segment(

		const GFX_MapData*  data,
		unsigned int        offset)
{
	return bsearch(
		GFX_UINT_TO_VOID(offset),
		data->segments.begin,
		gfx_vector_get_size(&data->segments),
		sizeof(GFX_Segment),
		_gfx_material_segment_comp
	);
}

/******************************************************/
int _gfx_property_map_list_insert_copies_at(

		GFXPropertyMapList  list,
		unsigned int        index,
		unsigned int        copies,
		unsigned int*       offset)
{
	GFX_Segment new =
	{
		.ref    = 1,
		.offset = 0,
		.num    = copies
	};

	GFX_MapData* data = ((GFX_MapData*)list) + index;

	/* Check for overflow */
	if(UINT_MAX - copies < data->copies)
	{
		gfx_errors_push(
			GFX_ERROR_OVERFLOW,
			"Overflow occurred during copy insertion at a Material."
		);
		return 0;
	}

	/* Iterate through segments and find a big enough empty spot */
	GFXVectorIterator it;
	for(
		it = data->segments.begin;
		it != data->segments.end;
		it = gfx_vector_next(&data->segments, it))
	{
		GFX_Segment* seg = it;
		if(copies > (seg->offset - new.offset))
		{
			new.offset = seg->offset + seg->num;
			continue;
		}

		/* Now try to insert the segment */
		it = gfx_vector_insert(&data->segments, &new, it);
		if(it == data->segments.end) return 0;

		data->copies += copies;
		*offset = new.offset;

		return 1;
	}

	/* Expand the property map if necessary */
	unsigned int expand = new.offset + copies;

	if(data->map->copies >= expand)
		expand = 0;

	else
	{
		expand -= data->map->copies;
		if(!gfx_property_map_expand(data->map, expand))
			return 0;
	}

	/* Insert at end instead */
	it = gfx_vector_insert(
		&data->segments,
		&new,
		data->segments.end
	);

	if(it != data->segments.end)
	{
		data->copies += copies;
		*offset = new.offset;

		return 1;
	}

	/* Shrink property map again.. */
	gfx_property_map_shrink(data->map, expand);

	return 0;
}

/******************************************************/
int _gfx_property_map_list_reference_copies_at(

		GFXPropertyMapList  list,
		unsigned int        index,
		unsigned int        offset)
{
	/* Retrieve segment */
	GFX_MapData* data =
		((GFX_MapData*)list) + index;
	GFX_Segment* it =
		_gfx_material_find_segment(data, offset);

	if(!it) return 0;

	/* Increase reference counter */
	if(!(it->ref + 1))
	{
		/* Overflow error */
		gfx_errors_push(
			GFX_ERROR_OVERFLOW,
			"Overflow occurred during Material copy referencing."
		);
		return 0;
	}

	++it->ref;
	return 1;
}

/******************************************************/
void _gfx_property_map_list_erase_copies_at(

		GFXPropertyMapList  list,
		unsigned int        index,
		unsigned int        offset)
{
	/* Retrieve segment */
	GFX_MapData* data =
		((GFX_MapData*)list) + index;
	GFX_Segment* it =
		_gfx_material_find_segment(data, offset);

	/* Check reference count */
	if(it && !(--it->ref))
	{
		/* Decrease used copies */
		data->copies -= it->num;
		gfx_vector_erase(&data->segments, it);
	}
}

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

		while(num--)
		{
			gfx_property_map_free(maps[num].map);
			gfx_vector_clear(&maps[num].segments);
		}

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

	gfx_vector_init(&data.segments, sizeof(GFX_Segment));

	/* Add it to the LOD map */
	if(!gfx_lod_map_add((GFXLodMap*)material, level, &data))
	{
		gfx_property_map_free(data.map);
		gfx_vector_clear(&data.segments);

		return NULL;
	}

	return data.map;
}

/******************************************************/
GFXPropertyMapList gfx_material_get(

		const GFXMaterial*  material,
		unsigned int        level,
		unsigned int*       num)
{
	return gfx_lod_map_get((const GFXLodMap*)material, level, num);
}

/******************************************************/
GFXPropertyMapList gfx_material_get_all(

		const GFXMaterial*  material,
		unsigned int*       num)
{
	return gfx_lod_map_get_all((const GFXLodMap*)material, num);
}

/******************************************************/
unsigned int gfx_property_map_list_copies_at(

		const GFXPropertyMapList  list,
		unsigned int              index)
{
	return ((const GFX_MapData*)list)[index].copies;
}

/******************************************************/
GFXPropertyMap* gfx_property_map_list_at(

		const GFXPropertyMapList  list,
		unsigned int              index)
{
	return ((const GFX_MapData*)list)[index].map;
}
