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
GFXMaterial* gfx_material_create(void);

/**
 * Makes sure the material is freed properly.
 *
 */
void gfx_material_free(

		GFXMaterial* material);

/**
 * Creates a new property map and maps it to a given level of detail.
 *
 * @param level      Level of detail to map to (must be <= material->levels).
 * @param program    Program to set inputs for.
 * @param properties Fixed number of property indices associated with this map.
 * @param instances  Number of instances which can be drawn at once using the property map.
 * @return The new property map on success, NULL on failure.
 *
 * Note: instances can be 0 to signify unlimited instances.
 *
 */
GFXPropertyMap* gfx_material_add(

		GFXMaterial*   material,
		unsigned int   level,
		GFXProgram*    program,
		unsigned char  properties,
		size_t         instances);

/**
 * Returns an abstract list of property maps of a given level of detail.
 *
 * @param num Returns the number of property maps in the returned list.
 * @return List of property maps.
 *
 * Note: as soon as a property map is added the list is invalidated.
 *
 */
GFXPropertyMapList gfx_material_get(

		GFXMaterial*   material,
		unsigned int   level,
		unsigned int*  num);

/**
 * Returns an abstract list of property maps of all levels.
 *
 * Note: as soon as a property map is added the list is invalidated.
 *
 */
GFXPropertyMapList gfx_material_get_all(

		GFXMaterial*   material,
		unsigned int*  num);

/**
 * Index into a list of property maps, retrieving the instances.
 *
 * @param list List of property maps returned by gfx_material_get or gfx_material_get_all.
 * @return Number of instances which can be drawn at once using the property map, 0 for infinite.
 *
 * Note: you can only retrieve a property map with index < number of elements in list.
 *
 */
size_t gfx_property_map_list_instances_at(

		GFXPropertyMapList  list,
		unsigned int        index);

/**
 * Index into a list of property maps, retrieving the number of used copies.
 *
 */
unsigned int gfx_property_map_list_copies_at(

		GFXPropertyMapList  list,
		unsigned int        index);

/**
 * Index into a list of property maps, retrieving the property map.
 *
 */
GFXPropertyMap* gfx_property_map_list_at(

		GFXPropertyMapList  list,
		unsigned int        index);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_MATERIAL_H
