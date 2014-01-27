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

#include "groufix/containers/vector.h"
#include "groufix/memory/internal.h"
#include "groufix/shading/internal.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Internal property map */
struct GFX_Map
{
	/* Super Class */
	GFXPropertyMap map;

	/* Hidden data */
	GLuint         handle;   /* OpenGL program handle */
	unsigned char  buffers;  /* Number of buffer properties */
	unsigned char  samplers; /* Number of sampler properties */
	GFXVector      values;   /* Values of all properties */
};

/* Internal property */
struct GFX_Property
{
	GFXPropertyType  type;
	GLuint           location; /* Block index or uniform location */
	unsigned char    size;     /* In bytes */
	size_t           index;    /* Of value in value vector */
};

/* Internal value property */
struct GFX_Value
{
	unsigned char    count;
	unsigned char    components;
	GFXUnpackedType  type;
};

/* Internal buffer property */
struct GFX_Buffer
{
	GLuint      buffer;
	GLintptr    offset;
	GLsizeiptr  size;
};

/* Internal sampler property */
struct GFX_Sampler
{
	GLuint texture;
	GLuint target;
};

/******************************************************/
static void _gfx_property_set(struct GFX_Map* map, struct GFX_Property* prop, GFX_Extensions* ext)
{
	const void* data = gfx_vector_at(&map->values, prop->index);

	/* Check if it has a value */
	if(prop->size) switch(prop->type)
	{
		/* Bind the uniform vector */
		case GFX_VECTOR_PROPERTY :
		{
			const struct GFX_Value* val = (const struct GFX_Value*)data;
			const void* value = (const void*)(val + 1);

			switch(val->type)
			{
				case GFX_FLOAT : switch(val->components)
				{
					case 1 : ext->Uniform1fv(prop->location, val->count, value); break;
					case 2 : ext->Uniform2fv(prop->location, val->count, value); break;
					case 3 : ext->Uniform3fv(prop->location, val->count, value); break;
					case 4 : ext->Uniform4fv(prop->location, val->count, value); break;
				}
				break;

				case GFX_INT : switch(val->components)
				{
					case 1 : ext->Uniform1iv(prop->location, val->count, value); break;
					case 2 : ext->Uniform2iv(prop->location, val->count, value); break;
					case 3 : ext->Uniform3iv(prop->location, val->count, value); break;
					case 4 : ext->Uniform4iv(prop->location, val->count, value); break;
				}
				break;

				case GFX_UNSIGNED_INT : switch(val->components)
				{
					case 1 : ext->Uniform1uiv(prop->location, val->count, value); break;
					case 2 : ext->Uniform2uiv(prop->location, val->count, value); break;
					case 3 : ext->Uniform3uiv(prop->location, val->count, value); break;
					case 4 : ext->Uniform4uiv(prop->location, val->count, value); break;
				}
				break;

				/* Herp */
				default : break;
			}

			break;
		}

		/* Bind the uniform matrix */
		case GFX_MATRIX_PROPERTY :
		{
			const struct GFX_Value* val = (const struct GFX_Value*)data;
			const void* value = (const void*)(val + 1);

			switch(val->type)
			{
				case GFX_FLOAT : switch(val->components)
				{
					case 4  : ext->UniformMatrix2fv(prop->location, val->count, GL_FALSE, value); break;
					case 9  : ext->UniformMatrix3fv(prop->location, val->count, GL_FALSE, value); break;
					case 16 : ext->UniformMatrix4fv(prop->location, val->count, GL_FALSE, value); break;
				}
				break;

				/* Derp */
				default : break;
			}

			break;
		}

		/* Bind uniform block to uniform buffer index */
		case GFX_BUFFER_PROPERTY :
		{
			const struct GFX_Buffer* buff = (const struct GFX_Buffer*)data;
			size_t index = _gfx_binder_bind_uniform_buffer(buff->buffer, buff->offset, buff->size, 1, ext);

			ext->UniformBlockBinding(map->handle, prop->location, index);

			break;
		}

		/* Set sampler uniform to texture unit */
		case GFX_SAMPLER_PROPERTY :
		{
			const struct GFX_Sampler* samp = (const struct GFX_Sampler*)data;
			GLint unit = _gfx_binder_bind_texture(samp->texture, samp->target, 1, ext);

			ext->Uniform1iv(prop->location, 1, &unit);

			break;
		}
	}
}

/******************************************************/
void _gfx_property_map_use(GFXPropertyMap* map, GFX_Extensions* ext)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;

	/* Prevent binding it twice */
	if(ext->program != internal->handle)
	{
		ext->program = internal->handle;
		ext->UseProgram(internal->handle);
	}

	/* Secondly, prevent setting properties twice */
	if(_gfx_program_target(map->program, map))
	{
		struct GFX_Property* prop;
		unsigned char properties = map->properties;

		for(prop = (struct GFX_Property*)(internal + 1); properties--; ++prop)
			_gfx_property_set(internal, prop, ext);
	}
}

/******************************************************/
static inline struct GFX_Property* _gfx_property_map_get_at(struct GFX_Map* map, unsigned char index)
{
	/* Check index */
	if(index >= map->map.properties) return NULL;

	return ((struct GFX_Property*)(map + 1)) + index;
}

/******************************************************/
static inline void _gfx_property_init(struct GFX_Property* prop)
{
	prop->type     = GFX_VECTOR_PROPERTY; /* Anything but a sampler or buffer property */
	prop->location = GL_INVALID_INDEX;
	prop->size     = 0;
	prop->index    = 0;
}

/******************************************************/
static void _gfx_property_erase(struct GFX_Map* map, struct GFX_Property* prop)
{
	if(prop->size)
	{
		/* Check if any buffers or samplers are being removed */
		if(prop->type == GFX_BUFFER_PROPERTY) --map->buffers;
		else if(prop->type == GFX_SAMPLER_PROPERTY) --map->samplers;

		/* Erase from value vector */
		gfx_vector_erase_range_at(&map->values, prop->size, prop->index);

		prop->size = 0;
		prop->index = 0;
	}
}

/******************************************************/
static int _gfx_property_enable(struct GFX_Map* map, struct GFX_Property* prop, size_t size, const GFX_Extensions* ext)
{
	/* Nothing to enable */
	if(prop->location == GL_INVALID_INDEX || !size) return 0;

	/* First erase if different size */
	if(prop->size != size) _gfx_property_erase(map, prop);

	/* Allocate the value */
	if(!prop->size)
	{
		char buffDiff = 0;
		char sampDiff = 0;

		/* Check limits */
		if(prop->type == GFX_BUFFER_PROPERTY)
		{
			buffDiff = 1;
			if(map->buffers + buffDiff > ext->limits[GFX_LIM_MAX_BUFFER_PROPERTIES]) return 0;
		}
		else if(prop->type == GFX_SAMPLER_PROPERTY)
		{
			sampDiff = 1;
			if(map->samplers + sampDiff > ext->limits[GFX_LIM_MAX_SAMPLER_PROPERTIES]) return 0;
		}

		/* Insert into value vector */
		size_t index = gfx_vector_get_size(&map->values);
		if(gfx_vector_insert_range(&map->values, size, NULL, map->values.end) == map->values.end) return 0;

		prop->size = size;
		prop->index = index;

		map->buffers += buffDiff;
		map->samplers += sampDiff;
	}

	return 1;
}

/******************************************************/
static inline void _gfx_property_disable(struct GFX_Map* map, struct GFX_Property* prop)
{
	_gfx_property_erase(map, prop);
	_gfx_property_init(prop);
}

/******************************************************/
GFXPropertyMap* gfx_property_map_create(GFXProgram* program, unsigned char properties)
{
	/* Create new property map, append properties to end of struct */
	size_t size = sizeof(struct GFX_Map) + properties * sizeof(struct GFX_Property);

	struct GFX_Map* map = calloc(1, size);
	if(!map) return NULL;

	map->map.program = program;
	map->map.properties = properties;
	map->handle = _gfx_program_get_handle(program);

	/* Initialize all properties */
	struct GFX_Property* prop;

	for(prop = (struct GFX_Property*)(map + 1); properties--; ++prop)
		_gfx_property_init(prop);

	gfx_vector_init(&map->values, 1);

	return (GFXPropertyMap*)map;
}

/******************************************************/
void gfx_property_map_free(GFXPropertyMap* map)
{
	if(map)
	{
		struct GFX_Map* internal = (struct GFX_Map*)map;

		/* Make sure to untarget its program */
		_gfx_program_untarget(map->program, map);

		/* Erase all properties */
		struct GFX_Property* prop;

		for(prop = (struct GFX_Property*)(internal + 1); map->properties--; ++prop)
			_gfx_property_erase(internal, prop);

		gfx_vector_clear(&internal->values);
		free(map);
	}
}

/******************************************************/
int gfx_property_map_set(GFXPropertyMap* map, unsigned char index, GFXPropertyType type, const char* name)
{
	/* Get current window, context and property */
	GFX_Window* window = _gfx_window_get_current();

	struct GFX_Map* internal = (struct GFX_Map*)map;
	struct GFX_Property* prop = _gfx_property_map_get_at(internal, index);

	if(!window || !prop) return 0;

	/* Get location */
	GLuint loc = GL_INVALID_INDEX;
	switch(type)
	{
		case GFX_VECTOR_PROPERTY :
		case GFX_MATRIX_PROPERTY :
		case GFX_SAMPLER_PROPERTY :
		{
			GLint l = window->extensions.GetUniformLocation(internal->handle, name);
			loc = (l < 0) ? loc : l;
			break;
		}
		case GFX_BUFFER_PROPERTY :
		{
			loc = window->extensions.GetUniformBlockIndex(internal->handle, name);
			break;
		}
	}

	/* Validate index */
	if(loc == GL_INVALID_INDEX) return 0;

	/* Disable previous property with equal location */
	struct GFX_Property* it;
	unsigned char properties = map->properties;

	for(it = (struct GFX_Property*)(internal + 1); properties--; ++it)
		if(it->location == loc)
		{
			_gfx_property_disable(internal, it);
			break;
		}

	/* Reset property */
	_gfx_property_disable(internal, prop);
	prop->type = type;
	prop->location = loc;

	return 1;
}

/******************************************************/
int gfx_property_map_get(GFXPropertyMap* map, unsigned char index, GFXPropertyType* type)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;

	/* Get property */
	struct GFX_Property* prop = _gfx_property_map_get_at(internal, index);
	if(!prop) return 0;

	int assigned = prop->location != GL_INVALID_INDEX;
	if(assigned && type) *type = prop->type;

	return assigned;
}

/******************************************************/
int gfx_property_map_set_value(GFXPropertyMap* map, unsigned char index, GFXProperty value)
{
	/* Get current window, context and property */
	GFX_Window* window = _gfx_window_get_current();

	struct GFX_Map* internal = (struct GFX_Map*)map;
	struct GFX_Property* prop = _gfx_property_map_get_at(internal, index);

	if(!window || !prop) return 0;

	/* Check type */
	if(prop->type != GFX_VECTOR_PROPERTY && prop->type != GFX_MATRIX_PROPERTY) return 0;

	/* Make sure it is enabled */
	GFXDataType type;
	type.unpacked = value.type;

	value.count = value.count ? value.count : 1;
	size_t size = (size_t)value.components * (size_t)_gfx_sizeof_data_type(type) * (size_t)value.count;

	if(!_gfx_property_enable(
		internal,
		prop,
		sizeof(struct GFX_Value) + size,
		&window->extensions)) return 0;

	/* Set value */
	struct GFX_Value* val = gfx_vector_at(&internal->values, prop->index);

	val->count      = value.count;
	val->components = value.components;
	val->type       = value.type;

	memcpy(val + 1, value.data, size);

	return 1;
}

/******************************************************/
int gfx_property_map_set_buffer(GFXPropertyMap* map, unsigned char index, const GFXBuffer* buffer, size_t offset, size_t size)
{
	/* Get current window, context and property */
	GFX_Window* window = _gfx_window_get_current();

	struct GFX_Map* internal = (struct GFX_Map*)map;
	struct GFX_Property* prop = _gfx_property_map_get_at(internal, index);

	if(!window || !prop) return 0;

	/* Check type */
	if(prop->type != GFX_BUFFER_PROPERTY) return 0;

	/* Make sure it is enabled */
	if(!_gfx_property_enable(
		internal,
		prop,
		sizeof(struct GFX_Buffer),
		&window->extensions)) return 0;

	/* Set buffer */
	struct GFX_Buffer* buff = gfx_vector_at(&internal->values, prop->index);

	buff->buffer = _gfx_buffer_get_handle(buffer);
	buff->offset = offset;
	buff->size = size;

	return 1;
}

/******************************************************/
int gfx_property_map_set_sampler(GFXPropertyMap* map, unsigned char index, const GFXTexture* texture)
{
	/* Get current window, context and property */
	GFX_Window* window = _gfx_window_get_current();

	struct GFX_Map* internal = (struct GFX_Map*)map;
	struct GFX_Property* prop = _gfx_property_map_get_at(internal, index);

	if(!window || !prop) return 0;

	/* Check type */
	if(prop->type != GFX_SAMPLER_PROPERTY) return 0;

	/* Make sure it is enabled */
	if(!_gfx_property_enable(
		internal,
		prop,
		sizeof(struct GFX_Sampler),
		&window->extensions)) return 0;

	/* Set sampler */
	struct GFX_Sampler* samp = gfx_vector_at(&internal->values, prop->index);

	samp->texture = _gfx_texture_get_handle(texture);
	samp->target = _gfx_texture_get_internal_target(texture);

	return 1;
}
