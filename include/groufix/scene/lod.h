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

#ifndef GFX_SCENE_LOD_H
#define GFX_SCENE_LOD_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * LOD Map (map given objects to a level of detail)
 *******************************************************/

/** LOD Flags */
typedef enum GFXLodFlags
{
	GFX_LOD_ERASABLE  = 0x01

} GFXLodFlags;


/** LOD Map */
typedef struct GFXLodMap
{
	GFXLodFlags  flags;    /* Fixed flags */
	size_t       levels;   /* Number of LODs */
	size_t       compSize; /* Number of bytes to use for comparisons */

} GFXLodMap;


/**
 * Creates a new LOD map.
 *
 * @param flags    Flags to apply to this LOD map.
 * @param dataSize Size of each mapped object in bytes.
 * @param compSize Starting bytes of elements to use to compare said elements.
 * @return NULL on failure.
 *
 */
GFXLodMap* gfx_lod_map_create(

		GFXLodFlags  flags,
		size_t       dataSize,
		size_t       compSize);

/**
 * Makes sure the LOD map is freed properly.
 *
 */
void gfx_lod_map_free(

		GFXLodMap* map);

/**
 * Maps data to a given level of detail.
 *
 * @param level Level of detail to map to (must be <= map->levels).
 * @param data  Data of dataSize (see gfx_lod_map_create) bytes, copied into the map.
 * @return Non-zero if the data is mapped.
 *
 * The data is appended to the end of the level (as seen in the return of gfx_lod_map_get).
 *
 */
int gfx_lod_map_add(

		GFXLodMap*  map,
		size_t      level,
		void*       data);

/**
 * Removes mapped data from a given level of detail.
 *
 * @param data Data of compSize bytes to compare against.
 * @return Non-zero if it was found and removed.
 *
 * If this operation causes a level to be empty,
 * all levels above it will fall down one level.
 *
 */
int gfx_lod_map_remove(

		GFXLodMap*  map,
		size_t      level,
		void*       data);

/**
 * Removes mapped data from a given level of detail.
 *
 * @param index Index of the data within the level as seen in the return of gfx_lod_map_get.
 * @return Non-zero if it was removed.
 *
 * If this operation causes a level to be empty,
 * all levels above it will fall down one level.
 *
 */
int gfx_lod_map_remove_at(

		GFXLodMap*  map,
		size_t      level,
		size_t      index);

/**
 * Retrieves wheter given data is mapped or not.
 *
 * @param level Level of detail it should be mapped to.
 * @param data  Data of compSize bytes to compare against.
 * @return Non-zero if the data is indeed mapped to the given level.
 *
 */
int gfx_lod_map_has(

		GFXLodMap*  map,
		size_t      level,
		void*       data);

/**
 * Counts the number of data elements in a number of levels.
 *
 * @param levels Number of levels, starting at 0, to count the elements of.
 *
 */
size_t gfx_lod_map_count(

		GFXLodMap*  map,
		size_t      levels);

/**
 * Returns an array of data elements of dataSize bytes of a given level of detail.
 *
 * @param num Returns the number of elements in the returned array.
 * @return Array of num elements.
 *
 * Note: as soon as a data element is added/removed the array pointer is invalidated.
 *
 */
void* gfx_lod_map_get(

		GFXLodMap*  map,
		size_t      level,
		size_t*     num);

/**
 * Returns an array of data elements of dataSize bytes of all levels.
 *
 */
void* gfx_lod_map_get_all(

		GFXLodMap*  map,
		size_t*     num);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_LOD_H
