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
 * Removes a property map from the material.
 *
 * This might or might not remove LODs.
 * If this happens, higher levels will fall down one level.
 *
 */
void gfx_material_remove(

		GFXMaterial*     material,
		GFXPropertyMap*  map);

/**
 * Returns an array of property maps of a given level of detail.
 *
 * @param num Returns the number of property maps in the returned array.
 * @return Array of num property maps.
 *
 * Note: as soon as a property map is added/removed the array pointer is invalidated.
 *
 */
inline GFXPropertyMap** gfx_material_get(

		GFXMaterial*  material,
		size_t        level,
		size_t*       num)
{
	return (GFXPropertyMap**)gfx_lod_map_get((GFXLodMap*)material, level, num);
}


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_MATERIAL_H