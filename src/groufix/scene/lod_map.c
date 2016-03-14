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
#include "groufix/scene/internal.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

/******************************************************/
static void _gfx_lod_map_get_boundaries(

		const GFX_LodMap  *map,
		GFXVectorIterator  level,
		unsigned int      *begin,
		unsigned int      *end)
{
	*end = *(unsigned int*)level;

	if(level == map->levels.begin) *begin = 0;
	else *begin = *(unsigned int*)gfx_vector_previous(&map->levels, level);
}

/******************************************************/
static unsigned int _gfx_lod_map_find_data(

		const GFX_LodMap   *map,
		unsigned int        begin,
		unsigned int        end,
		const void         *data,
		GFXVectorIterator  *found)
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
static int _gfx_lod_map_remove_at(

		GFX_LodMap        *map,
		GFXVectorIterator  level,
		unsigned int       index)
{
	/* Get boundaries */
	unsigned int begin;
	unsigned int end;
	_gfx_lod_map_get_boundaries(
		map,
		level,
		&begin,
		&end
	);

	unsigned int size = end - begin;
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
		--(*(unsigned int*)level);
		level = gfx_vector_next(&map->levels, level);
	}

	return 1;
}

/******************************************************/
void _gfx_lod_map_init(

		GFX_LodMap  *map,
		GFXLodFlags  flags,
		size_t       dataSize,
		size_t       compSize)
{
	map->map.flags = flags;
	map->map.levels = 0;
	map->map.compSize = (compSize > dataSize) ? dataSize : compSize;

	gfx_vector_init(&map->data, dataSize);
	gfx_vector_init(&map->levels, sizeof(unsigned int));
}

/******************************************************/
void _gfx_lod_map_clear(

		GFX_LodMap *map)
{
	map->map.levels = 0;
	gfx_vector_clear(&map->data);
	gfx_vector_clear(&map->levels);
}

/******************************************************/
GFXLodMap *gfx_lod_map_create(

		GFXLodFlags  flags,
		size_t       dataSize,
		size_t       compSize)
{
	/* Allocate new map */
	GFX_LodMap *map = malloc(sizeof(GFX_LodMap));
	if(map) _gfx_lod_map_init(map, flags, dataSize, compSize);

	/* Out of memory error */
	else gfx_errors_push(
		GFX_ERROR_OUT_OF_MEMORY,
		"Lod Map could not be allocated."
	);

	return (GFXLodMap*)map;
}

/******************************************************/
void gfx_lod_map_free(

		GFXLodMap *map)
{
	if(map)
	{
		_gfx_lod_map_clear((GFX_LodMap*)map);
		free(map);
	}
}

/******************************************************/
int gfx_lod_map_add(

		GFXLodMap    *map,
		unsigned int  level,
		void         *data)
{
	GFX_LodMap *internal = (GFX_LodMap*)map;

	/* Overflow */
	size_t size = gfx_vector_get_size(&internal->data);
	if(level > map->levels || size == UINT_MAX) return 0;

	/* Get level iterator */
	GFXVectorIterator levIt;

	if(level == map->levels)
	{
		/* Insert the level if it doesn't exist yet */
		unsigned int upper = size;
		levIt = gfx_vector_insert(
			&internal->levels,
			&upper,
			internal->levels.end
		);

		if(levIt == internal->levels.end) return 0;
		++map->levels;
	}
	else
	{
		/* Check single data flag */
		if(map->flags & GFX_LOD_SINGLE_DATA) return 0;
		levIt = gfx_vector_at(&internal->levels, level);
	}

	/* Get boundaries */
	unsigned int begin;
	unsigned int end;
	_gfx_lod_map_get_boundaries(
		internal,
		levIt,
		&begin,
		&end
	);

	/* Insert the data */
	GFXVectorIterator it = gfx_vector_insert_at(
		&internal->data,
		data,
		end
	);

	if(it == internal->data.end)
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
		++(*(unsigned int*)levIt);
		levIt = gfx_vector_next(&internal->levels, levIt);
	}

	return 1;
}

/******************************************************/
unsigned int gfx_lod_map_has(

		const GFXLodMap  *map,
		unsigned int      level,
		const void       *data)
{
	if(level >= map->levels) return 0;

	const GFX_LodMap *internal =
		(const GFX_LodMap*)map;
	GFXVectorIterator it = gfx_vector_at(
		&internal->levels,
		level);

	/* Get boundaries */
	unsigned int begin;
	unsigned int end;
	_gfx_lod_map_get_boundaries(
		internal,
		it,
		&begin,
		&end
	);

	/* Find all instances */
	unsigned int count;
	for(count = 0; begin != end; ++count)
	{
		begin = _gfx_lod_map_find_data(
			internal,
			begin,
			end,
			data,
			&it
		);
	}

	return count;
}

/******************************************************/
unsigned int gfx_lod_map_remove(

		GFXLodMap    *map,
		unsigned int  level,
		const void   *data)
{
	/* Check if erasable and level bounds */
	if(!(map->flags & GFX_LOD_ERASABLE) || level >= map->levels) return 0;

	GFX_LodMap *internal = (GFX_LodMap*)map;
	GFXVectorIterator levIt = gfx_vector_at(
		&internal->levels,
		level
	);

	/* Get boundaries */
	unsigned int begin;
	unsigned int end;
	_gfx_lod_map_get_boundaries(
		internal,
		levIt,
		&begin,
		&end
	);

	/* Find all instances */
	unsigned int count = 0;
	unsigned int i;

	for(i = begin; i != end; ++count)
	{
		GFXVectorIterator found;
		i = _gfx_lod_map_find_data(
			internal,
			begin,
			end,
			data,
			&found
		);

		if(i != end)
		{
			/* Remove it */
			_gfx_lod_map_remove_at(internal, levIt, i - begin);
			--end;
		}
	}

	return count;
}

/******************************************************/
int gfx_lod_map_remove_at(

		GFXLodMap    *map,
		unsigned int  level,
		unsigned int  index)
{
	/* Check if erasable and level bounds */
	if(!(map->flags & GFX_LOD_ERASABLE) || level >= map->levels) return 0;

	/* Get level and remove */
	GFX_LodMap *internal = (GFX_LodMap*)map;
	GFXVectorIterator levIt = gfx_vector_at(
		&internal->levels,
		level
	);

	return _gfx_lod_map_remove_at(internal, levIt, index);
}

/******************************************************/
unsigned int gfx_lod_map_count(

		const GFXLodMap  *map,
		unsigned int      levels)
{
	levels = (levels > map->levels) ? map->levels : levels;
	if(!levels) return 0;

	return *(unsigned int*)gfx_vector_at(
		&((const GFX_LodMap*)map)->levels,
		levels - 1
	);
}

/******************************************************/
void *gfx_lod_map_get(

		const GFXLodMap  *map,
		unsigned int      level,
		unsigned int     *num)
{
	if(level >= map->levels)
	{
		*num = 0;
		return NULL;
	}

	const GFX_LodMap *internal = (const GFX_LodMap*)map;

	/* Get boundaries */
	unsigned int begin;
	unsigned int end;
	_gfx_lod_map_get_boundaries(
		internal,
		gfx_vector_at(&internal->levels, level),
		&begin,
		&end
	);

	*num = end - begin;

	return gfx_vector_at(&internal->data, begin);
}

/******************************************************/
void *gfx_lod_map_get_all(

		const GFXLodMap  *map,
		unsigned int     *num)
{
	if(!map->levels)
	{
		*num = 0;
		return NULL;
	}

	const GFX_LodMap *internal =
		(const GFX_LodMap*)map;
	*num = *(unsigned int*)gfx_vector_at(
		&internal->levels,
		map->levels - 1);

	return internal->data.begin;
}
