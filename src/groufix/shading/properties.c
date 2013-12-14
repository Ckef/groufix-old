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
/* Internal property map */
struct GFX_Internal_Map
{
	/* Super Class */
	GFXPropertyMap map;

	/* OpenGL program handle */
	GLuint program;
};

/* Internal property */
struct GFX_Internal_Property
{
	GFXPropertyType  type;
	GLuint           location; /* Block index or uniform location */
	void*            value;
};

/******************************************************/
static inline struct GFX_Internal_Property* _gfx_property_map_get_at(struct GFX_Internal_Map* map, unsigned char index)
{
	/* Check index */
	if(index >= map->map.properties) return NULL;

	return ((struct GFX_Internal_Property*)(map + 1)) + index;
}

/******************************************************/
GFXPropertyMap* gfx_property_map_create(GFXProgram* program, unsigned char properties)
{
	/* Create new property map, append properties to end of struct */
	size_t size = sizeof(struct GFX_Internal_Map) + properties * sizeof(struct GFX_Internal_Property);

	struct GFX_Internal_Map* map = calloc(1, size);
	if(!map) return NULL;

	map->program = _gfx_program_get_handle(program);
	map->map.properties = properties;

	/* Initialize all properties */
	struct GFX_Internal_Property* prop;

	for(prop = (struct GFX_Internal_Property*)(map + 1); properties--; ++prop)
		prop->location = GL_INVALID_INDEX;

	return (GFXPropertyMap*)map;
}

/******************************************************/
void gfx_property_map_free(GFXPropertyMap* map)
{
	if(map)
	{
		struct GFX_Internal_Map* internal = (struct GFX_Internal_Map*)map;

		/* Free all properties */
		struct GFX_Internal_Property* prop;

		for(prop = (struct GFX_Internal_Property*)(internal + 1); map->properties--; ++prop)
			free(prop->value);

		free(map);
	}
}

/******************************************************/
int gfx_property_map_set(GFXPropertyMap* map, unsigned char index, GFXPropertyType type, const char* name)
{
	/* Get current window, context and property */
	GFX_Internal_Window* window = _gfx_window_get_current();

	struct GFX_Internal_Map* internal = (struct GFX_Internal_Map*)map;
	struct GFX_Internal_Property* prop = _gfx_property_map_get_at(internal, index);

	if(!window || !prop) return 0;

	/* Get location */
	GLuint loc = GL_INVALID_INDEX;
	switch(type)
	{
		case GFX_VECTOR_PROPERTY :
		case GFX_MATRIX_PROPERTY :
		case GFX_SAMPLER_PROPERTY :
		{
			GLint l = window->extensions.GetUniformLocation(internal->program, name);
			loc = (l < 0) ? loc : l;
			break;
		}
		case GFX_BUFFER_PROPERTY :
		{
			loc = window->extensions.GetUniformBlockIndex(internal->program, name);
			break;
		}
	}

	/* Validate index */
	if(loc == GL_INVALID_INDEX) return 0;

	prop->type = type;
	prop->location = loc;

	/* Disable previous property with equal location */
	struct GFX_Internal_Property* it;
	unsigned char properties = map->properties;

	for(it = (struct GFX_Internal_Property*)(internal + 1); properties--; ++it)
		if(it->location == prop->location && it != prop)
		{
			it->location = GL_INVALID_INDEX;

			free(it->value);
			it->value = NULL;

			break;
		}

	return 1;
}

/******************************************************/
int gfx_property_map_get(GFXPropertyMap* map, unsigned char index, GFXPropertyType* type)
{
	struct GFX_Internal_Map* internal = (struct GFX_Internal_Map*)map;

	/* Get property */
	struct GFX_Internal_Property* prop = _gfx_property_map_get_at(internal, index);
	if(!prop) return 0;

	int assigned = (prop->location != GL_INVALID_INDEX) ? 1 : 0;
	if(assigned && type) *type = prop->type;

	return assigned;
}

/******************************************************/
int gfx_property_map_set_value(GFXPropertyMap* map, unsigned char index, GFXProperty value)
{
	struct GFX_Internal_Map* internal = (struct GFX_Internal_Map*)map;

	/* Get property and check type */
	struct GFX_Internal_Property* prop = _gfx_property_map_get_at(internal, index);
	if(!prop) return 0;

	if(prop->type != GFX_VECTOR_PROPERTY && prop->type != GFX_MATRIX_PROPERTY) return 0;

	return 0;
}

/******************************************************/
int gfx_property_map_set_buffer(GFXPropertyMap* map, unsigned char index, const GFXBuffer* buffer, size_t offset, size_t size)
{
	struct GFX_Internal_Map* internal = (struct GFX_Internal_Map*)map;

	/* Get property and check type */
	struct GFX_Internal_Property* prop = _gfx_property_map_get_at(internal, index);
	if(!prop) return 0;

	if(prop->type != GFX_BUFFER_PROPERTY) return 0;

	return 0;
}

/******************************************************/
int gfx_property_map_set_sampler(GFXPropertyMap* map, unsigned char index, const GFXTexture* texture)
{
	struct GFX_Internal_Map* internal = (struct GFX_Internal_Map*)map;

	/* Get property and check type */
	struct GFX_Internal_Property* prop = _gfx_property_map_get_at(internal, index);
	if(!prop) return 0;

	if(prop->type != GFX_SAMPLER_PROPERTY) return 0;

	return 0;
}
