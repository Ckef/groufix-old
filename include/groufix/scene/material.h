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
	size_t            num;          /* Number of elements in propertyMaps */
	GFXPropertyMap**  propertyMaps; /* Array containing the property maps */

} GFXMaterial;


/**
 * Initializes a material.
 *
 */
void gfx_material_init(GFXMaterial* material);

/**
 * Clears content of a material.
 *
 */
void gfx_material_clear(GFXMaterial* material);

/**
 * Appends a newly created property map to the end of the material.
 *
 * @param program    Program to set inputs for.
 * @param properties Fixed number of property indices associated with this map.
 * @return The new property map on success, NULL on failure.
 *
 */
GFXPropertyMap* gfx_material_push(GFXMaterial* material, GFXProgram* program, unsigned char properties);

/**
 * Removes a property map from the material.
 *
 * @param index Index of the property map in material->propertyMaps.
 *
 */
void gfx_material_remove(GFXMaterial* material, size_t index);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_MATERIAL_H
