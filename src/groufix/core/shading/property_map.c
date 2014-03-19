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

#include "groufix/core/shading/internal.h"
#include "groufix/core/memory/internal.h"
#include "groufix/containers/vector.h"

#include <stdlib.h>
#include <string.h>

/* Internal property types */
#define GFX_INT_PROPERTY_UNKNOWN  0
#define GFX_INT_PROPERTY_VECTOR   1
#define GFX_INT_PROPERTY_MATRIX   2
#define GFX_INT_PROPERTY_SAMPLER  3
#define GFX_INT_PROPERTY_BLOCK    4

/******************************************************/
/* Internal property map */
struct GFX_Map
{
	/* Super Class */
	GFXPropertyMap map;

	/* Hidden data */
	GLuint         handle;   /* OpenGL program handle */
	GFXVector      values;   /* Values of all properties */

	unsigned char  samplers; /* Number of sampler properties */
	unsigned char  blocks;   /* Number of block properties */
};

/* Internal property */
struct GFX_Property
{
	unsigned char  type;     /* Internal type */
	GLuint         location; /* Block index or uniform location */
	size_t         index;    /* Of value in value vector */
	unsigned char  size;     /* In bytes */
};

/* Internal vector/matrix property */
struct GFX_Value
{
	GFXUnpackedType  type;
	unsigned char    components;
	size_t           count;
};

/* Internal sampler property */
struct GFX_Sampler
{
	GLuint texture;
	GLuint target;
};

/* Internal block property */
struct GFX_Block
{
	GLuint      buffer;
	GLintptr    offset;
	GLsizeiptr  size;
};

/******************************************************/
static void _gfx_property_set(struct GFX_Map* map, struct GFX_Property* prop, GFX_Extensions* ext)
{
	void* data = gfx_vector_at(&map->values, prop->index);

	/* Check if it has a value */
	if(prop->size) switch(prop->type)
	{
		/* Bind the uniform vector */
		case GFX_INT_PROPERTY_VECTOR :
		{
			struct GFX_Value* val = (struct GFX_Value*)data;
			void* value = val + 1;

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
		case GFX_INT_PROPERTY_MATRIX :
		{
			struct GFX_Value* val = (struct GFX_Value*)data;
			void* value = val + 1;

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

		/* Set sampler uniform to texture unit */
		case GFX_INT_PROPERTY_SAMPLER :
		{
			struct GFX_Sampler* samp = (struct GFX_Sampler*)data;

			int old;
			GLint unit = _gfx_binder_bind_texture(
				samp->texture,
				samp->target,
				1, &old, ext
			);
			if(!old) ext->Uniform1iv(prop->location, 1, &unit);

			break;
		}

		/* Bind uniform block to uniform buffer index */
		case GFX_INT_PROPERTY_BLOCK :
		{
			struct GFX_Block* block = (struct GFX_Block*)data;

			int old;
			size_t index = _gfx_binder_bind_uniform_buffer(
				block->buffer,
				block->offset,
				block->size,
				1, &old, ext
			);
			if(!old) ext->UniformBlockBinding(map->handle, prop->location, index);

			break;
		}

		/* Uuuurr.. */
		default : break;
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
	prop->type     = GFX_INT_PROPERTY_UNKNOWN;
	prop->location = GL_INVALID_INDEX;
	prop->index    = 0;
	prop->size     = 0;
}

/******************************************************/
static void _gfx_property_erase(struct GFX_Map* map, struct GFX_Property* prop)
{
	if(prop->size)
	{
		/* Check if any buffers or samplers are being removed */
		if(prop->type == GFX_INT_PROPERTY_SAMPLER) --map->samplers;
		else if(prop->type == GFX_INT_PROPERTY_BLOCK) --map->blocks;

		/* Adjust value index of all properties */
		struct GFX_Property* it;
		unsigned char properties = map->map.properties;

		for(it = (struct GFX_Property*)(map + 1); properties--; ++it)
			if(it->index > prop->index) it->index -= prop->size;

		/* Erase from value vector */
		gfx_vector_erase_range_at(&map->values, prop->size, prop->index);

		prop->index = 0;
		prop->size = 0;
	}
}

/******************************************************/
static int _gfx_property_enable(struct GFX_Map* map, struct GFX_Property* prop, size_t size, const GFX_Extensions* ext)
{
	/* Nothing to enable */
	if(prop->type == GFX_INT_PROPERTY_UNKNOWN || !size) return 0;

	/* First erase if different size */
	if(prop->size != size) _gfx_property_erase(map, prop);

	/* Allocate the value */
	if(!prop->size)
	{
		char blockDiff = 0;
		char sampDiff = 0;

		/* Check limits */
		if(prop->type == GFX_INT_PROPERTY_SAMPLER)
		{
			sampDiff = 1;
			if(map->samplers + sampDiff > ext->limits[GFX_LIM_MAX_SAMPLER_PROPERTIES]) return 0;
		}
		else if(prop->type == GFX_INT_PROPERTY_BLOCK)
		{
			blockDiff = 1;
			if(map->blocks + blockDiff > ext->limits[GFX_LIM_MAX_BUFFER_PROPERTIES]) return 0;
		}

		/* Insert into value vector */
		size_t index = gfx_vector_get_size(&map->values);
		if(gfx_vector_insert_range(&map->values, size, NULL, map->values.end) == map->values.end) return 0;

		prop->index = index;
		prop->size = size;

		map->samplers += sampDiff;
		map->blocks += blockDiff;
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

		gfx_vector_clear(&internal->values);
		free(map);
	}
}

/******************************************************/
static void _gfx_property_map_forward(struct GFX_Map* map, struct GFX_Property* prop, unsigned char type, GLuint location)
{
	/* Disable previous property with equal location */
	struct GFX_Property* it;
	unsigned char properties = map->map.properties;

	for(it = (struct GFX_Property*)(map + 1); properties--; ++it)
		if(it->type == type && it->location == location)
		{
			_gfx_property_disable(map, it);
			break;
		}

	/* Reset property */
	_gfx_property_disable(map, prop);
	prop->type = type;
	prop->location = location;
}

/******************************************************/
int gfx_property_map_forward(GFXPropertyMap* map, unsigned char index, unsigned short property)
{
	/* Get current window, context and property */
	GFX_Window* window = _gfx_window_get_current();

	struct GFX_Map* internal = (struct GFX_Map*)map;
	struct GFX_Property* forward = _gfx_property_map_get_at(internal, index);

	/* Get property and location */
	GLint location = _gfx_program_get_location(map->program, property);
	const GFXProperty* prop = gfx_program_get_property(map->program, property);

	if(!window || !forward || location < 0 || !prop) return 0;
	if(!prop->components || !prop->count) return 0;

	/* Forward property */
	unsigned char type;
	switch(prop->type)
	{
		case GFX_VECTOR_PROPERTY :
			type = GFX_INT_PROPERTY_VECTOR;
			break;

		case GFX_MATRIX_PROPERTY :
			type = GFX_INT_PROPERTY_MATRIX;
			break;

		case GFX_SAMPLER_PROPERTY :
			type = GFX_INT_PROPERTY_SAMPLER;
			break;

		/* I don't know D: */
		default : return 0;
	}

	_gfx_property_map_forward(internal, forward, type, location);

	/* Allocate data */
	switch(type)
	{
		/* Value property */
		case GFX_INT_PROPERTY_VECTOR :
		case GFX_INT_PROPERTY_MATRIX :
		{
			GFXDataType dataType;
			dataType.unpacked = prop->dataType;

			size_t size = (size_t)prop->components * (size_t)_gfx_sizeof_data_type(dataType) * (size_t)prop->count;

			/* Try to enable it */
			if(!_gfx_property_enable(
				internal,
				forward,
				sizeof(struct GFX_Value) + size,
				&window->extensions))
			{
				_gfx_property_disable(internal, forward);
				return 0;
			}

			/* Set value information */
			struct GFX_Value* val = gfx_vector_at(&internal->values, forward->index);

			val->type       = prop->dataType;
			val->components = prop->components;
			val->count      = prop->count;

			break;
		}

		/* Sampler property */
		case GFX_INT_PROPERTY_SAMPLER :
		{
			/* Try to enable it */
			if(!_gfx_property_enable(
				internal,
				forward,
				sizeof(struct GFX_Sampler),
				&window->extensions))
			{
				_gfx_property_disable(internal, forward);
				return 0;
			}

			/* Set default sampler */
			struct GFX_Sampler* samp = gfx_vector_at(&internal->values, forward->index);

			samp->texture = 0;
			samp->target = 0;

			break;
		}
	}

	return 1;
}

/******************************************************/
int gfx_property_map_forward_named(GFXPropertyMap* map, unsigned char index, const char* name)
{
	/* Fetch property index and forward it */
	unsigned short property = gfx_program_get_named_property(map->program, name);
	if(property >= map->program->properties) return 0;

	return gfx_property_map_forward(map, index, property);
}

/******************************************************/
int gfx_property_map_forward_block(GFXPropertyMap* map, unsigned char index, unsigned short block)
{
	/* Get current window, context and property */
	GFX_Window* window = _gfx_window_get_current();

	struct GFX_Map* internal = (struct GFX_Map*)map;
	struct GFX_Property* forward = _gfx_property_map_get_at(internal, index);

	/* Get block */
	const GFXPropertyBlock* bl = gfx_program_get_property_block(map->program, block);

	if(!window || !forward || !bl) return 0;
	if(!bl->numProperties || !bl->properties) return 0;

	/* Forward property */
	_gfx_property_map_forward(internal, forward, GFX_INT_PROPERTY_BLOCK, block);

	/* Allocate data */
	if(!_gfx_property_enable(
		internal,
		forward,
		sizeof(struct GFX_Block),
		&window->extensions))
	{
		_gfx_property_disable(internal, forward);
		return 0;
	}

	/* Set default buffer */
	struct GFX_Block* buff = gfx_vector_at(&internal->values, forward->index);

	buff->buffer = 0;
	buff->offset = 0;
	buff->size = 0;

	return 1;
}

/******************************************************/
int gfx_property_map_forward_named_block(GFXPropertyMap* map, unsigned char index, const char* name)
{
	/* Fetch block index and forward it */
	unsigned short block = gfx_program_get_named_property_block(map->program, name);
	if(block >= map->program->blocks) return 0;

	return gfx_property_map_forward_block(map, index, block);
}

/******************************************************/
int gfx_property_map_set_value(GFXPropertyMap* map, unsigned char index, const void* value, size_t offset, size_t size)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;
	struct GFX_Property* prop = _gfx_property_map_get_at(internal, index);

	if(!prop) return 0;

	/* Check type */
	if(prop->type != GFX_INT_PROPERTY_VECTOR && prop->type != GFX_INT_PROPERTY_MATRIX) return 0;

	/* Calculate size */
	size_t max = prop->size - sizeof(struct GFX_Value);
	offset = offset > max ? max : offset;

	max -= offset;
	size = (!size || size > max) ? max : size;

	/* Set value */
	struct GFX_Value* val = gfx_vector_at(&internal->values, prop->index);
	memcpy(GFX_PTR_ADD_BYTES(val + 1, offset), value, size);

	return 1;
}

/******************************************************/
int gfx_property_map_set_sampler(GFXPropertyMap* map, unsigned char index, const GFXTexture* texture)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;
	struct GFX_Property* prop = _gfx_property_map_get_at(internal, index);

	if(!prop) return 0;

	/* Check type */
	if(prop->type != GFX_INT_PROPERTY_SAMPLER) return 0;

	/* Set sampler */
	struct GFX_Sampler* samp = gfx_vector_at(&internal->values, prop->index);

	samp->texture = _gfx_texture_get_handle(texture);
	samp->target = _gfx_texture_get_internal_target(texture);

	return 1;
}

/******************************************************/
static int _gfx_property_map_set_block(struct GFX_Map* map, unsigned char index, GLuint buffer, size_t offset, size_t size)
{
	struct GFX_Property* prop = _gfx_property_map_get_at(map, index);
	if(!prop) return 0;

	/* Check type */
	if(prop->type != GFX_INT_PROPERTY_BLOCK) return 0;

	/* Set block */
	struct GFX_Block* buff = gfx_vector_at(&map->values, prop->index);

	buff->buffer = buffer;
	buff->offset = offset;
	buff->size = size;

	return 1;
}

/******************************************************/
int gfx_property_map_set_buffer(GFXPropertyMap* map, unsigned char index, const GFXBuffer* buffer, size_t offset, size_t size)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;
	return _gfx_property_map_set_block(internal, index, _gfx_buffer_get_handle(buffer), offset, size);
}

/******************************************************/
int gfx_property_map_set_shared_buffer(GFXPropertyMap* map, unsigned char index, const GFXSharedBuffer* buffer, size_t offset, size_t size)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;
	return _gfx_property_map_set_block(internal, index, _gfx_shared_buffer_get_handle(buffer), offset + buffer->offset, size);
}

/******************************************************/
int gfx_property_map_swap(GFXPropertyMap* map, unsigned char index1, unsigned char index2)
{
	/* Get properties */
	struct GFX_Map* internal = (struct GFX_Map*)map;
	struct GFX_Property* prop1 = _gfx_property_map_get_at(internal, index1);
	struct GFX_Property* prop2 = _gfx_property_map_get_at(internal, index2);

	if(!prop1 || !prop2) return 0;

	/* Check if they can be swapped */
	if(prop1->size != prop2->size) return 0;

	/* Swap indices to values */
	size_t temp = prop1->index;
	prop1->index = prop2->index;
	prop2->index = temp;

	return 1;
}
