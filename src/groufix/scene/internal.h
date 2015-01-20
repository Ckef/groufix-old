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
 * Material management
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
 * Frees previously reserved copies so they can be reserved by someone else.
 *
 * @param offset Offset to free copies at as returned by _gfx_property_map_list_insert_copies_at.
 *
 */
void _gfx_property_map_list_erase_copies_at(

		GFXPropertyMapList  list,
		unsigned int        index,
		unsigned int        offset);


/********************************************************
 * SubMesh management
 *******************************************************/

/**
 * Creates a new submesh.
 *
 * @param drawCalls Fixed number of draw calls associated with the layout.
 * @param sources   Fixed number of sources to create.
 * @return NULL on failure.
 *
 */
GFXSubMesh* _gfx_submesh_create(

		unsigned char  drawCalls,
		unsigned char  sources);

/**
 * References a submesh to postpone its destruction.
 *
 * @return Zero on overflow.
 *
 */
int _gfx_submesh_reference(

		GFXSubMesh* mesh);

/**
 * Makes sure the submesh is freed properly.
 *
 * Decreases the reference counter before freeing,
 * only freeing if the counter hits 0.
 *
 */
void _gfx_submesh_free(

		GFXSubMesh* mesh);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_INTERNAL_H
