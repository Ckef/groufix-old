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

#ifndef GFX_SCENE_INTERNAL_H
#define GFX_SCENE_INTERNAL_H

#include "groufix/scene/material.h"
#include "groufix/scene/mesh.h"
#include "groufix/containers/vector.h"

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
	GFXVector levels; /* Stores size_t, upper bound of the level */

} GFX_LodMap;


/**
 * Initializes a LOD map.
 *
 * @param dataSize Size of each mapped object in bytes.
 * @param compSize Starting bytes of elements to use to compare said elements.
 *
 */
void _gfx_lod_map_init(

		GFX_LodMap*  map,
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
 * Ads a bucket unit to the material.
 *
 * @param level Level of detail to add to (must be <= material->levels).
 * @param index Index of the property map within the level as seen in the return of gfx_material_get.
 * @param pipe  Bucket to add it to.
 * @param src   Source of the bucket to use for this unit.
 * @return The ID of the unit, 0 on failure.
 *
 */
size_t _gfx_material_add_bucket_unit(

		GFXMaterial*   material,
		size_t         level,
		size_t         index,
		GFXPipe*       pipe,
		size_t         src,
		GFXBatchState  state,
		int            visible);

/**
 * Retrieves a number of bucket units from the material.
 *
 * @param num Returns the number of returned units.
 * @return Abstract array containing num units.
 *
 * As soon as any units are added or removed, the returned pointer is invalidated.
 * Note: index is not bound checked.
 *
 */
void* _gfx_material_get_bucket_units(

		GFXMaterial*  material,
		size_t        level,
		size_t        index,
		GFXPipe*      pipe,
		size_t*       num);

/**
 * Finds units with a given source in an array returned by _gfx_material_get_bucket_units.
 *
 * @param units  Array returned by _gfx_material_get_bucket_units.
 * @param search Number of elements beginning at the start of the array to include in the search.
 * @param src    Source to search for.
 * @param num    Number of found units starting at the returned index.
 * @return       Index into the array the found units start at.
 *
 * Note: search is not bound checked.
 *
 */
size_t _gfx_material_find_bucket_units(

		void*    units,
		size_t   search,
		size_t   src,
		size_t*  num);

/**
 * Indexes into the array returned by _gfx_material_get_bucket_units.
 *
 * @param array Abstract array to index into.
 * @param unit  Index into the array (bounds not checked).
 * @return The ID of the unit.
 *
 */
size_t _gfx_material_bucket_units_at(

		void*   array,
		size_t  unit);

/**
 * Removes multiple bucket units from the material.
 *
 * @param unit Index into the array returned by _gfx_material_get_bucket_units.
 * @param num  Number of units to erase starting at the given index.
 *
 * Note: index is not bound checked.
 *
 */
void _gfx_material_remove_bucket_units(

		GFXMaterial*  material,
		size_t        level,
		size_t        index,
		GFXPipe*      pipe,
		size_t        unit,
		size_t        num);


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
 * Decreases the reference counter before freeing.
 *
 */
void _gfx_submesh_free(

		GFXSubMesh* mesh);

/**
 * Adds the bucket and increases a reference counter referencing the bucket.
 *
 * @return Zero on failure or overflow.
 *
 */
int _gfx_submesh_reference_bucket(

		GFXSubMesh*  mesh,
		GFXPipe*     pipe);

/**
 * Decreases the reference counter, removes the submesh from the bucket if zero.
 *
 */
void _gfx_submesh_remove_bucket(

		GFXSubMesh*  mesh,
		GFXPipe*     pipe);

/**
 * Retrieves the bucket source ID from a submesh.
 *
 * @param index Index of the submesh source (must be < mesh->sources).
 * @param pipe  A bucket references in the submesh.
 * @return The ID of the source, 0 on failure.
 *
 */
size_t _gfx_submesh_get_bucket_source(

		GFXSubMesh*    mesh,
		GFXPipe*       pipe,
		unsigned char  index);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_INTERNAL_H
