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

#include "groufix/internal.h"

#include <stdlib.h>

/******************************************************/
struct GFX_Internal_Property
{
	GFXPropertyType  type;
	GLuint           location; /* Block index or uniform location */
};

/******************************************************/
GFXPropertyMap* gfx_property_map_create(GFXProgram* program, unsigned char properties)
{
	/* Create new property map, append properties to end of struct */
	size_t size = sizeof(GFXPropertyMap) + properties * sizeof(struct GFX_Internal_Property);

	GFXPropertyMap* map = malloc(size);
	if(!map) return NULL;

	map->program = program;
	map->properties = properties;

	/* Initialize all properties */
	struct GFX_Internal_Property* prop;
	for(prop = (struct GFX_Internal_Property*)(map + 1); properties--; ++prop)
	{
		prop->location = GL_INVALID_INDEX;
	}

	return map;
}

/******************************************************/
void gfx_property_map_free(GFXPropertyMap* map)
{
	free(map);
}

/******************************************************/
int gfx_property_map_set(GFXPropertyMap* map, unsigned char index, GFXPropertyType type, const char* name)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window || index >= map->properties) return 0;

	/* Get location */
	GLuint location = GL_INVALID_INDEX;
	GLuint prog = _gfx_program_get_handle(map->program);

	switch(type)
	{
		case GFX_VECTOR_PROPERTY :
		case GFX_MATRIX_PROPERTY :
		case GFX_SAMPLER_PROPERTY :
		{
			GLint loc = window->extensions.GetUniformLocation(prog, name);
			location = (loc < 0) ? GL_INVALID_INDEX : loc;
			break;
		}
		case GFX_BUFFER_PROPERTY :
		{
			location = window->extensions.GetUniformBlockIndex(prog, name);
			break;
		}
	}

	/* Set property */
	struct GFX_Internal_Property* prop = ((struct GFX_Internal_Property*)(map + 1)) + index;
	prop->type = type;
	prop->location = location;

	return (location != GL_INVALID_INDEX) ? 1 : 0;
}

/******************************************************/
int gfx_property_map_get(GFXPropertyMap* map, unsigned char index, GFXPropertyType* type)
{
	/* Check index */
	if(index >= map->properties) return 0;

	/* Get property */
	struct GFX_Internal_Property* prop = ((struct GFX_Internal_Property*)(map + 1)) + index;
	int assigned = (prop->location != GL_INVALID_INDEX) ? 1 : 0;

	if(assigned && type) *type = prop->type;

	return assigned;
}

/******************************************************/
int gfx_property_map_set_value(GFXPropertyMap* map, unsigned char index, GFXProperty value)
{
	return 0;
}


/******************************************************/
int gfx_property_map_set_buffer(GFXPropertyMap* map, unsigned char index, const GFXBuffer* buffer, size_t offset, size_t size)
{
	return 0;
}
