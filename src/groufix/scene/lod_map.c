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

#include "groufix/scene/lod.h"
#include "groufix/containers/vector.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Internal LOD map */
struct GFX_Map
{
	/* Super class */
	GFXLodMap map;

	/* Hidden data */
	GFXVector data;   /* Stores elements of dataSize bytes */
	GFXVector levels; /* Stores size_t, upper bound of the level */
};

/******************************************************/
static void _gfx_lod_map_get_boundaries(

		struct GFX_Map*    map,
		GFXVectorIterator  level,
		size_t*            begin,
		size_t*            end)
{
	*end = *(size_t*)level;

	if(level == map->levels.begin) *begin = 0;
	else *begin = *(size_t*)gfx_vector_previous(&map->levels, level);
}

/******************************************************/
static size_t _gfx_lod_map_find_data(

		struct GFX_Map*     map,
		size_t              begin,
		size_t              end,
		void*               data,
		GFXVectorIterator*  found)
{
	/* Find the data and count */
	GFXVectorIterator it = gfx_vector_at(&map->data, begin);

	while(begin != end)
	{
		if(!memcmp(it, data, map->data.elementSize)) break;

		++begin;
		it = gfx_vector_next(&map->data, it);
	}
	*found = it;

	return begin;
}

/******************************************************/
GFXLodMap* gfx_lod_map_create(

		size_t dataSize)
{
	/* Allocate new map */
	struct GFX_Map* map = calloc(1, sizeof(struct GFX_Map));
	if(!map) return NULL;

	gfx_vector_init(&map->data, dataSize);
	gfx_vector_init(&map->levels, sizeof(size_t));

	return (GFXLodMap*)map;
}

/******************************************************/
void gfx_lod_map_free(

		GFXLodMap* map)
{
	if(map)
	{
		struct GFX_Map* internal = (struct GFX_Map*)map;

		gfx_vector_clear(&internal->data);
		gfx_vector_clear(&internal->levels);

		free(map);
	}
}

/******************************************************/
int gfx_lod_map_add(

		GFXLodMap*  map,
		size_t      level,
		void*       data)
{
	if(level > map->levels) return 0;

	struct GFX_Map* internal = (struct GFX_Map*)map;

	/* Get level iterator */
	GFXVectorIterator levIt;

	if(level == map->levels)
	{
		/* Insert the level if it doesn't exist yet */
		size_t upper = gfx_vector_get_size(&internal->data);
		levIt = gfx_vector_insert(
			&internal->levels,
			&upper,
			internal->levels.end
		);

		if(levIt == internal->levels.end) return 0;

		++map->levels;
	}
	else levIt = gfx_vector_at(&internal->levels, level);

	/* Get boundaries */
	size_t begin;
	size_t end;
	_gfx_lod_map_get_boundaries(
		internal,
		levIt,
		&begin,
		&end
	);

	/* Find the data */
	GFXVectorIterator found;
	size_t index = _gfx_lod_map_find_data(
		internal,
		begin,
		end,
		data,
		&found
	);

	if(index == end)
	{
		/* Insert the data */
		if(gfx_vector_insert(&internal->data, data, found) == internal->data.end)
		{
			if(begin == end)
			{
				gfx_vector_erase(&internal->levels, levIt);
				--map->levels;
			}
			return 0;
		}

		/* Increase upper bounds */
		while(levIt != internal->levels.end)
		{
			++(*(size_t*)levIt);
			levIt = gfx_vector_next(&internal->levels, levIt);
		}
	}

	return 1;
}

/******************************************************/
int gfx_lod_map_remove(

		GFXLodMap*  map,
		size_t      level,
		void*       data)
{
	if(level >= map->levels) return 0;

	struct GFX_Map* internal = (struct GFX_Map*)map;
	GFXVectorIterator levIt = gfx_vector_at(
		&internal->levels,
		level
	);

	/* Get boundaries */
	size_t begin;
	size_t end;
	_gfx_lod_map_get_boundaries(
		internal,
		levIt,
		&begin,
		&end
	);

	/* Find the data */
	GFXVectorIterator found;
	size_t index = _gfx_lod_map_find_data(
		internal,
		begin,
		end,
		data,
		&found
	);

	if(index == end) return 0;

	/* Erase the data */
	gfx_vector_erase(&internal->data, found);
	if(end - begin == 1)
	{
		levIt = gfx_vector_erase(&internal->levels, levIt);
		--map->levels;
	}

	/* Decrease upper bounds */
	while(levIt != internal->levels.end)
	{
		--(*(size_t*)levIt);
		levIt = gfx_vector_next(&internal->levels, levIt);
	}

	return 1;
}

/******************************************************/
int gfx_lod_map_has(

		GFXLodMap*  map,
		size_t      level,
		void*       data)
{
	if(level >= map->levels) return 0;

	struct GFX_Map* internal = (struct GFX_Map*)map;
	GFXVectorIterator levIt = gfx_vector_at(
		&internal->levels,
		level
	);

	/* Get boundaries */
	size_t begin;
	size_t end;
	_gfx_lod_map_get_boundaries(
		internal,
		levIt,
		&begin,
		&end
	);

	/* Find the data */
	GFXVectorIterator found;
	size_t index = _gfx_lod_map_find_data(
		internal,
		begin,
		end,
		data,
		&found
	);

	return (index != end);
}

/******************************************************/
void* gfx_lod_map_get(

		GFXLodMap*  map,
		size_t      level,
		size_t*     num)
{
	if(level >= map->levels)
	{
		*num = 0;
		return NULL;
	}

	struct GFX_Map* internal = (struct GFX_Map*)map;
	GFXVectorIterator levIt = gfx_vector_at(
		&internal->levels,
		level
	);

	/* Get boundaries */
	size_t begin;
	size_t end;
	_gfx_lod_map_get_boundaries(
		internal,
		levIt,
		&begin,
		&end
	);

	*num = end - begin;

	return gfx_vector_at(&internal->data, begin);
}
