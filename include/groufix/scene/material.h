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

#ifndef GFX_SCENE_MATERIAL_H
#define GFX_SCENE_MATERIAL_H

#include "groufix/scene/lod.h"
#include "groufix/core/shading.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Material (collection of property maps)
 *******************************************************/

/** Material */
typedef struct GFXMaterial
{
	GFXLodMap lodMap; /* Super class */

} GFXMaterial;


/** Property map list */
typedef void* GFXPropertyMapList;


/**
 * Creates a new material.
 *
 * @return NULL on failure.
 *
 */
GFX_API GFXMaterial* gfx_material_create(void);

/**
 * Makes sure the material is freed properly.
 *
 */
GFX_API void gfx_material_free(

		GFXMaterial* material);

/**
 * Creates a new property map and maps it to a given level of detail.
 *
 * @param level      Level of detail to map to (must be <= material->levels).
 * @param programMap Program map to set inputs for.
 * @param properties Fixed number of property indices associated with this map.
 * @return The new property map on success, NULL on failure.
 *
 */
GFX_API GFXPropertyMap* gfx_material_add(

		GFXMaterial*    material,
		unsigned int    level,
		GFXProgramMap*  programMap,
		unsigned char   properties);

/**
 * Returns an abstract list of property maps of a given level of detail.
 *
 * @param num Returns the number of property maps in the returned list.
 * @return List of property maps.
 *
 * Note: as soon as a property map is added the list is invalidated.
 *
 */
GFX_API GFXPropertyMapList gfx_material_get(

		GFXMaterial*   material,
		unsigned int   level,
		unsigned int*  num);

/**
 * Returns an abstract list of property maps of all levels.
 *
 * Note: as soon as a property map is added the list is invalidated.
 *
 */
GFX_API GFXPropertyMapList gfx_material_get_all(

		GFXMaterial*   material,
		unsigned int*  num);

/**
 * Index into a list of property maps, retrieving the number of used copies.
 *
 */
GFX_API unsigned int gfx_property_map_list_copies_at(

		GFXPropertyMapList  list,
		unsigned int        index);

/**
 * Index into a list of property maps, retrieving the property map.
 *
 */
GFX_API GFXPropertyMap* gfx_property_map_list_at(

		GFXPropertyMapList  list,
		unsigned int        index);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_MATERIAL_H
