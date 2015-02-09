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

#ifndef GFX_SCENE_INTERNAL_H
#define GFX_SCENE_INTERNAL_H

#include "groufix/containers/vector.h"
#include "groufix/scene/material.h"
#include "groufix/scene/mesh.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Internal LOD map
 *******************************************************/

/* Internal LOD map */
typedef struct GFX_LodMap
{
	/* Super class */
	GFXLodMap map;

	/* Hidden data */
	GFXVector data;   /* Stores elements of dataSize bytes */
	GFXVector levels; /* Stores unsigned int, upper bound of the level */

} GFX_LodMap;


/**
 * Initializes a LOD map.
 *
 * @param flags    Flags to apply to this LOD map.
 * @param dataSize Size of each mapped object in bytes.
 * @param compSize Starting bytes of elements to use to compare said elements.
 *
 */
void _gfx_lod_map_init(

		GFX_LodMap*  map,
		GFXLodFlags  flags,
		size_t       dataSize,
		size_t       compSize);

/**
 * Clears the content of a LOD map.
 *
 */
void _gfx_lod_map_clear(

		GFX_LodMap* map);


/********************************************************
 * Material copy management
 *******************************************************/

/**
 * Requests a sequential number of copies to be reserved at a property map.
 *
 * @param copies Number of copies to reserve.
 * @param offset Returns the starting copy of the reserved range.
 * @return Zero on failure.
 *
 */
int _gfx_property_map_list_insert_copies_at(

		GFXPropertyMapList  list,
		unsigned int        index,
		unsigned int        copies,
		unsigned int*       offset);

/**
 * References previously reserved copies to postpone erasing the reservation.
 *
 * @param offset Offset to free copies at as returned by _gfx_property_map_list_insert_copies_at.
 * @return Zero on failure.
 *
 */
int _gfx_property_map_list_reference_copies_at(

		GFXPropertyMapList  list,
		unsigned int        index,
		unsigned int        offset);

/**
 * Frees a reference of a previously reserved copies.
 *
 * Only once all references are freed will the copies be available for a new reservation.
 *
 */
void _gfx_property_map_list_erase_copies_at(

		GFXPropertyMapList  list,
		unsigned int        index,
		unsigned int        offset);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_INTERNAL_H
