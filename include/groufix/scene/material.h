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
 * @return The new property map on success, NULL on failure.
 *
 */
GFXPropertyMap* gfx_material_add(

		GFXMaterial*   material,
		size_t         level,
		GFXProgram*    program,
		unsigned char  properties);

/**
 * Removes a property map from a material.
 *
 * @return Non-zero if it was found and removed.
 *
 * If this operation causes a level to be empty,
 * all levels above it will fall down one level.
 *
 */
int gfx_material_remove(

		GFXMaterial*     material,
		GFXPropertyMap*  map);

/**
 * Removes a property map from a given level of detail of a material.
 *
 * @param index Index of the property map within the level as seen in the return of gfx_material_get.
 * @return Non-zero if it was removed.
 *
 * If this operation causes a level to be empty,
 * all levels above it will fall down one level.
 *
 */
int gfx_material_remove_at(

		GFXMaterial*  material,
		size_t        level,
		size_t        index);

/**
 * Returns an abstract list of property maps of a given level of detail.
 *
 * @param num Returns the number of property maps in the returned list.
 * @return List of property maps.
 *
 * Note: as soon as a property map is added/removed the list is invalidated.
 *
 */
GFXPropertyMapList gfx_material_get(

		GFXMaterial*  material,
		size_t        level,
		size_t*       num);

/**
 * Index into a list of property maps.
 *
 * @param list List of property maps returned by gfx_material_get.
 *
 * Note: you can only retrieve a property map with index < num returned by gfx_material_get.
 *
 */
GFXPropertyMap* gfx_property_map_list_at(

		GFXPropertyMapList  list,
		size_t              index);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_MATERIAL_H
