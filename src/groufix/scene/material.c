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

#include "groufix/scene/material.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
static int _gfx_material_alloc_property_map(GFXMaterial* mat)
{
	/* Allocate the pointer */
	GFXPropertyMap** ptr = realloc(mat->propertyMaps, sizeof(GFXPropertyMap*) * (mat->num + 1));
	if(!ptr) return 0;

	mat->propertyMaps = ptr;
	++mat->num;

	return 1;
}

/******************************************************/
static void _gfx_material_free_property_map(GFXMaterial* mat)
{
	if(!(--mat->num))
	{
		free(mat->propertyMaps);
		mat->propertyMaps = NULL;
	}
	else mat->propertyMaps = realloc(mat->propertyMaps, sizeof(GFXPropertyMap*) * mat->num);
}

/******************************************************/
static void _gfx_material_remove_property_map(GFXMaterial* mat, size_t index)
{
	/* Move memory and free the last property map */
	memmove(
		mat->propertyMaps + index,
		mat->propertyMaps + index + 1,
		sizeof(GFXPropertyMap*) * (mat->num - index - 1));

	_gfx_material_free_property_map(mat);
}

/******************************************************/
void gfx_material_init(GFXMaterial* material)
{
	memset(material, 0, sizeof(GFXMaterial));
}

/******************************************************/
void gfx_material_clear(GFXMaterial* material)
{
	/* Free all property maps */
	size_t p;
	for(p = 0; p < material->num; ++p)
		gfx_property_map_free(material->propertyMaps[p]);

	free(material->propertyMaps);
	gfx_material_init(material);
}

/******************************************************/
GFXPropertyMap* gfx_material_push(GFXMaterial* material, GFXProgram* program, unsigned char properties)
{
	if(!_gfx_material_alloc_property_map(material)) return NULL;

	/* Create new property map */
	GFXPropertyMap* map = gfx_property_map_create(program, properties);
	if(!map)
	{
		_gfx_material_free_property_map(material);
		return NULL;
	}
	material->propertyMaps[material->num - 1] = map;

	return map;
}

/******************************************************/
void gfx_material_remove(GFXMaterial* material, size_t index)
{
	if(index < material->num)
	{
		gfx_property_map_free(material->propertyMaps[index]);
		_gfx_material_remove_property_map(material, index);
	}
}
