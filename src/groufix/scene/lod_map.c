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
#include "groufix/containers/vector.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
static void _gfx_lod_map_get_boundaries(

		GFX_LodMap*        map,
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

		GFX_LodMap*         map,
		size_t              begin,
		size_t              end,
		void*               data,
		GFXVectorIterator*  found)
{
	/* Find the data and count */
	GFXVectorIterator it = gfx_vector_at(&map->data, begin);

	while(begin != end)
	{
		if(!memcmp(it, data, map->map.compSize)) break;

		++begin;
		it = gfx_vector_next(&map->data, it);
	}
	*found = it;

	return begin;
}

/******************************************************/
void _gfx_lod_map_init(

		GFX_LodMap*  map,
		GFXLodFlags  flags,
		size_t       dataSize,
		size_t       compSize)
{
	map->map.flags = flags;
	map->map.levels = 0;
	map->map.compSize = (compSize > dataSize) ? dataSize : compSize;

	gfx_vector_init(&map->data, dataSize);
	gfx_vector_init(&map->levels, sizeof(size_t));
}

/******************************************************/
void _gfx_lod_map_clear(

		GFX_LodMap* map)
{
	map->map.levels = 0;
	gfx_vector_clear(&map->data);
	gfx_vector_clear(&map->levels);
}

/******************************************************/
GFXLodMap* gfx_lod_map_create(

		GFXLodFlags  flags,
		size_t       dataSize,
		size_t       compSize)
{
	/* Allocate new map */
	GFX_LodMap* map = calloc(1, sizeof(GFX_LodMap));
	if(!map) return NULL;

	_gfx_lod_map_init(map, flags, dataSize, compSize);

	return (GFXLodMap*)map;
}

/******************************************************/
void gfx_lod_map_free(

		GFXLodMap* map)
{
	if(map)
	{
		_gfx_lod_map_clear((GFX_LodMap*)map);
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

	GFX_LodMap* internal = (GFX_LodMap*)map;

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
static int _gfx_lod_map_remove_at(

		GFX_LodMap*        map,
		GFXVectorIterator  level,
		size_t             index)
{
	/* Get boundaries */
	size_t begin;
	size_t end;
	_gfx_lod_map_get_boundaries(
		map,
		level,
		&begin,
		&end
	);

	size_t size = end - begin;
	if(index >= size) return 0;

	/* Erase the data */
	gfx_vector_erase_at(&map->data, begin + index);
	if(size == 1)
	{
		level = gfx_vector_erase(&map->levels, level);
		--map->map.levels;
	}

	/* Decrease upper bounds */
	while(level != map->levels.end)
	{
		--(*(size_t*)level);
		level = gfx_vector_next(&map->levels, level);
	}

	return 1;
}

/******************************************************/
int gfx_lod_map_remove(

		GFXLodMap*  map,
		size_t      level,
		void*       data)
{
	/* Check if erasable and level bounds */
	if(!(map->flags & GFX_LOD_ERASABLE) || level >= map->levels) return 0;

	GFX_LodMap* internal = (GFX_LodMap*)map;
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

	/* Find the data and remove it */
	GFXVectorIterator found;
	size_t index = _gfx_lod_map_find_data(
		internal,
		begin,
		end,
		data,
		&found
	);

	return _gfx_lod_map_remove_at(internal, levIt, index - begin);
}

/******************************************************/
int gfx_lod_map_remove_at(

		GFXLodMap*  map,
		size_t      level,
		size_t      index)
{
	/* Check if erasable and level bounds */
	if(!(map->flags & GFX_LOD_ERASABLE) || level >= map->levels) return 0;

	/* Get level and remove */
	GFX_LodMap* internal = (GFX_LodMap*)map;
	GFXVectorIterator levIt = gfx_vector_at(
		&internal->levels,
		level
	);

	return _gfx_lod_map_remove_at(internal, levIt, index);
}

/******************************************************/
int gfx_lod_map_has(

		GFXLodMap*  map,
		size_t      level,
		void*       data)
{
	if(level >= map->levels) return 0;

	GFX_LodMap* internal = (GFX_LodMap*)map;
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
size_t gfx_lod_map_count(

		GFXLodMap*  map,
		size_t      levels)
{
	levels = (levels > map->levels) ? map->levels : levels;
	if(!levels) return 0;

	return *(size_t*)gfx_vector_at(&((GFX_LodMap*)map)->levels, levels - 1);
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

	GFX_LodMap* internal = (GFX_LodMap*)map;
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

/******************************************************/
void* gfx_lod_map_get_all(

		GFXLodMap*  map,
		size_t*     num)
{
	if(!map->levels)
	{
		*num = 0;
		return NULL;
	}

	GFX_LodMap* internal = (GFX_LodMap*)map;
	*num = *(size_t*)gfx_vector_at(
		&internal->levels,
		map->levels - 1
	);

	return internal->data.begin;
}
