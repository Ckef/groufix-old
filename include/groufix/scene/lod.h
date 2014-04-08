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

/** LodMap */
typedef struct GFXLodMap
{
	size_t levels; /* Number of LODs */

} GFXLodMap;


/**
 * Creates a new LOD map.
 *
 * @return NULL on failure.
 *
 */
GFXLodMap* gfx_lod_map_create(void);

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
 * @return Non-zero if the data is mapped.
 *
 */
int gfx_lod_map_add(

		GFXLodMap*  map,
		size_t      level,
		void*       data);

/**
 * Removes mapped data from a given level of detail.
 *
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
 * Retrieves wheter given data is mapped or not.
 *
 * @param level Level of detail it should be mapped to.
 * @return Non-zero if the data is indeed mapped to the given level.
 *
 */
int gfx_lod_map_has(

		GFXLodMap*  map,
		size_t      level,
		void*       data);

/**
 * Returns an array of data elements of a given level of detail.
 *
 * @param num Returns the number of elements in the returned array.
 * @return Array of num elements.
 *
 * Note: as soon as a data element is added/removed the array pointer is invalidated.
 *
 */
void** gfx_lod_map_get(

		GFXLodMap*  map,
		size_t      level,
		size_t*     num);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_LOD_H
