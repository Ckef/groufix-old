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

#include <limits.h>
#include <stdlib.h>
#include <string.h>

/* Internal property types */
#define GFX_INT_PROPERTY_UNKNOWN     0x00
#define GFX_INT_PROPERTY_VECTOR      0x01
#define GFX_INT_PROPERTY_VECTOR_PTR  0x02
#define GFX_INT_PROPERTY_MATRIX      0x03
#define GFX_INT_PROPERTY_MATRIX_PTR  0x04
#define GFX_INT_PROPERTY_SAMPLER     0x05
#define GFX_INT_PROPERTY_BLOCK       0x06

#define GFX_INT_PROPERTY_HAS_COPIES  0x08

/******************************************************/
/* Internal function to set a property */
typedef void (*GFX_PropertyFunc)(

	GLuint,
	GLuint,
	void*,
	unsigned int,
	GFX_Extensions*);


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
	unsigned char  type;     /* Internal type and whether this property contains copies or not */
	GLuint         location; /* Block index or uniform location */

	size_t         index;    /* Of values in value vector */
	size_t         size;     /* In bytes of a single copy */
};

/* Internal vector/matrix property */
struct GFX_Value
{
	GFXUnpackedType  type;
	unsigned char    components;
	size_t           count;

	size_t           offset;     /* Instance offset in bytes */
	unsigned int     offsetSize; /* Number of array elements of the instance offset */
};

/* Internal vector/matrix pointer property */
struct GFX_ValuePtr
{
	struct GFX_Value  val; /* Super class */
	const void*       ptr;
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
static inline struct GFX_Property* _gfx_property_map_get_at(

		struct GFX_Map*  map,
		unsigned char    index)
{
	return index >= map->map.properties ?
		NULL : ((struct GFX_Property*)(map + 1)) + index;
}

/******************************************************/
static inline unsigned char _gfx_property_get_type(

		unsigned char type)
{
	return type & ~GFX_INT_PROPERTY_HAS_COPIES;
}

/******************************************************/
static inline size_t _gfx_property_get_total_size(

		struct GFX_Map*       map,
		struct GFX_Property*  prop)
{
	return prop->size * (prop->type & GFX_INT_PROPERTY_HAS_COPIES ? map->map.copies : 1);
}

/******************************************************/
static inline GFXVectorIterator _gfx_property_get_value(

		struct GFX_Map*       map,
		struct GFX_Property*  prop,
		size_t                copy)
{
	return gfx_vector_at(
		&map->values,
		prop->index + (prop->type & GFX_INT_PROPERTY_HAS_COPIES ? copy * prop->size : 0)
	);
}

/******************************************************/
static void _gfx_property_set_unknown(

		GLuint           program,
		GLuint           location,
		void*            data,
		unsigned int     base,
		GFX_Extensions*  ext)
{
	/* Some unknown property type, ignore :D */
}

/******************************************************/
static void _gfx_property_set_vector_val(

		GLuint             location,
		struct GFX_Value*  val,
		const void*        data,
		unsigned int       base,
		GFX_Extensions*    ext)
{
	data = GFX_PTR_ADD_BYTES(data, base * val->offset);
	size_t cnt = val->count - base * val->offsetSize;

	switch(val->type)
	{
		case GFX_FLOAT : switch(val->components)
		{
			case 1 : ext->Uniform1fv(location, cnt, data); break;
			case 2 : ext->Uniform2fv(location, cnt, data); break;
			case 3 : ext->Uniform3fv(location, cnt, data); break;
			case 4 : ext->Uniform4fv(location, cnt, data); break;
		}
		break;

		case GFX_INT : switch(val->components)
		{
			case 1 : ext->Uniform1iv(location, cnt, data); break;
			case 2 : ext->Uniform2iv(location, cnt, data); break;
			case 3 : ext->Uniform3iv(location, cnt, data); break;
			case 4 : ext->Uniform4iv(location, cnt, data); break;
		}
		break;

		case GFX_UNSIGNED_INT : switch(val->components)
		{
			case 1 : ext->Uniform1uiv(location, cnt, data); break;
			case 2 : ext->Uniform2uiv(location, cnt, data); break;
			case 3 : ext->Uniform3uiv(location, cnt, data); break;
			case 4 : ext->Uniform4uiv(location, cnt, data); break;
		}
		break;

		/* Herp */
		default : break;
	}
}

/******************************************************/
static void _gfx_property_set_vector(

		GLuint           program,
		GLuint           location,
		void*            data,
		unsigned int     base,
		GFX_Extensions*  ext)
{
	struct GFX_Value* val = (struct GFX_Value*)data;
	_gfx_property_set_vector_val(location, val, val + 1, base, ext);
}

/******************************************************/
static void _gfx_property_set_vector_ptr(

		GLuint           program,
		GLuint           location,
		void*            data,
		unsigned int     base,
		GFX_Extensions*  ext)
{
	struct GFX_ValuePtr* val = (struct GFX_ValuePtr*)data;
	if(val->ptr)
		_gfx_property_set_vector_val(location, &val->val, val->ptr, base, ext);
}

/******************************************************/
static void _gfx_property_set_matrix_val(

		GLuint             location,
		struct GFX_Value*  val,
		const void*        data,
		unsigned int       base,
		GFX_Extensions*    ext)
{
	data = GFX_PTR_ADD_BYTES(data, base * val->offset);
	size_t cnt = val->count - base * val->offsetSize;

	switch(val->type)
	{
		case GFX_FLOAT : switch(val->components)
		{
			case 4  : ext->UniformMatrix2fv(location, cnt, GL_FALSE, data); break;
			case 9  : ext->UniformMatrix3fv(location, cnt, GL_FALSE, data); break;
			case 16 : ext->UniformMatrix4fv(location, cnt, GL_FALSE, data); break;
		}
		break;

		/* Derp */
		default : break;
	}
}

/******************************************************/
static void _gfx_property_set_matrix(

		GLuint           program,
		GLuint           location,
		void*            data,
		unsigned int     base,
		GFX_Extensions*  ext)
{
	struct GFX_Value* val = (struct GFX_Value*)data;
	_gfx_property_set_matrix_val(location, val, val + 1, base, ext);
}

/******************************************************/
static void _gfx_property_set_matrix_ptr(

		GLuint           program,
		GLuint           location,
		void*            data,
		unsigned int     base,
		GFX_Extensions*  ext)
{
	struct GFX_ValuePtr* val = (struct GFX_ValuePtr*)data;
	if(val->ptr)
		_gfx_property_set_matrix_val(location, &val->val, val->ptr, base, ext);
}

/******************************************************/
static void _gfx_property_set_sampler(

		GLuint           program,
		GLuint           location,
		void*            data,
		unsigned int     base,
		GFX_Extensions*  ext)
{
	struct GFX_Sampler* samp = (struct GFX_Sampler*)data;

	int old;
	GLint unit = _gfx_binder_bind_texture(
		samp->texture,
		samp->target,
		1, &old, ext
	);
	if(!old) ext->Uniform1iv(location, 1, &unit);
}

/******************************************************/
static void _gfx_property_set_block(

		GLuint           program,
		GLuint           location,
		void*            data,
		unsigned int     base,
		GFX_Extensions*  ext)
{
	struct GFX_Block* block = (struct GFX_Block*)data;

	int old;
	size_t index = _gfx_binder_bind_uniform_buffer(
		block->buffer,
		block->offset,
		block->size,
		1, &old, ext
	);
	if(!old) ext->UniformBlockBinding(program, location, index);
}

/******************************************************/
static void _gfx_property_set(

		struct GFX_Map*       map,
		struct GFX_Property*  prop,
		size_t                copy,
		unsigned int          base,
		GFX_Extensions*       ext)
{
	/* Jump table */
	static const GFX_PropertyFunc jump[] =
	{
		_gfx_property_set_unknown, /* Dummy method */

		_gfx_property_set_vector,
		_gfx_property_set_vector_ptr,
		_gfx_property_set_matrix,
		_gfx_property_set_matrix_ptr,
		_gfx_property_set_sampler,
		_gfx_property_set_block
	};

	/* Get value & type, jump to function */
	void* data = _gfx_property_get_value(map, prop, copy);

	jump[_gfx_property_get_type(prop->type)](
		map->handle,
		prop->location,
		data,
		base,
		ext
	);
}

/******************************************************/
void _gfx_property_map_use(

		GFXPropertyMap*  map,
		size_t           copy,
		unsigned int     base,
		GFX_Extensions*  ext)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;

	/* Prevent binding it twice */
	if(ext->program != internal->handle)
	{
		ext->program = internal->handle;
		ext->UseProgram(internal->handle);
	}

	/* Clamp copy */
	copy = (copy < map->copies) ? copy : map->copies - 1;

	/* Set all values of the program */
	struct GFX_Property* prop;
	unsigned char properties = map->properties;

	for(prop = (struct GFX_Property*)(internal + 1); properties--; ++prop)
		_gfx_property_set(internal, prop, copy, base, ext);
}

/******************************************************/
static inline void _gfx_property_init(

		struct GFX_Property* prop)
{
	prop->type     = GFX_INT_PROPERTY_UNKNOWN;
	prop->location = GL_INVALID_INDEX;
	prop->index    = 0;
	prop->size     = 0;
}

/******************************************************/
static int _gfx_property_enable(

		struct GFX_Map*        map,
		struct GFX_Property*   prop,
		size_t                 size,
		const GFX_Extensions*  ext)
{
	/* Nothing to enable */
	if(prop->type == GFX_INT_PROPERTY_UNKNOWN || !size) return 0;

	/* Allocate the value */
	if(!prop->size)
	{
		/* Get byte size and type */
		size_t totalSize = size * (prop->type & GFX_INT_PROPERTY_HAS_COPIES ? map->map.copies : 1);
		unsigned char type = _gfx_property_get_type(prop->type);

		char blockDiff = 0;
		char sampDiff = 0;

		/* Check limits */
		if(type == GFX_INT_PROPERTY_SAMPLER)
		{
			sampDiff = 1;
			if(map->samplers + sampDiff > ext->limits[GFX_LIM_MAX_SAMPLER_PROPERTIES])
				return 0;
		}
		else if(type == GFX_INT_PROPERTY_BLOCK)
		{
			blockDiff = 1;
			if(map->blocks + blockDiff > ext->limits[GFX_LIM_MAX_BUFFER_PROPERTIES])
				return 0;
		}

		/* Insert into value vector */
		size_t index = gfx_vector_get_size(&map->values);
		if(gfx_vector_insert_range(&map->values, totalSize, NULL, map->values.end) == map->values.end)
			return 0;

		prop->index = index;
		prop->size = size;

		map->samplers += sampDiff;
		map->blocks += blockDiff;
	}

	return 1;
}

/******************************************************/
static int _gfx_property_expand(

		struct GFX_Map*       map,
		struct GFX_Property*  prop,
		size_t                num)
{
	/* No copies to insert */
	if(!(prop->type & GFX_INT_PROPERTY_HAS_COPIES) || !num || !prop->size)
		return 0;

	/* Insert the copies */
	size_t insSize = prop->size * num;
	GFXVectorIterator end = _gfx_property_get_value(
		map,
		prop,
		map->map.copies
	);

	if(gfx_vector_insert_range(&map->values, insSize, NULL, end) == map->values.end)
		return 0;

	/* Adjust value index of all properties */
	struct GFX_Property* it;
	unsigned char properties = map->map.properties;

	for(it = (struct GFX_Property*)(map + 1); properties--; ++it)
		if(it->index > prop->index) it->index += insSize;

	/* Initialize all new copies */
	/* Just copy the value of the last copy into them */
	end = _gfx_property_get_value(
		map,
		prop,
		map->map.copies - 1
	);

	while(num--) memcpy(
			_gfx_property_get_value(map, prop, map->map.copies + num),
			end,
			prop->size);

	return 1;
}

/******************************************************/
static void _gfx_property_shrink(

		struct GFX_Map*       map,
		struct GFX_Property*  prop,
		size_t                num)
{
	/* No copies to remove */
	if(prop->type & GFX_INT_PROPERTY_HAS_COPIES && num && prop->size)
	{
		/* Erase the copies */
		size_t erSize = prop->size * num;

		GFXVectorIterator begin = _gfx_property_get_value(
			map,
			prop,
			map->map.copies - num);

		gfx_vector_erase_range(
			&map->values,
			erSize,
			begin);

		/* Adjust value index of all properties */
		struct GFX_Property* it;
		unsigned char properties = map->map.properties;

		for(it = (struct GFX_Property*)(map + 1); properties--; ++it)
			if(it->index > prop->index) it->index -= erSize;
	}
}

/******************************************************/
static void _gfx_property_move(

		struct GFX_Map*       map,
		struct GFX_Property*  prop,
		size_t                dest,
		size_t                src)
{
	/* No copies to move */
	if(prop->type & GFX_INT_PROPERTY_HAS_COPIES && dest != src && prop->size)
	{
		GFXVectorIterator destVal = _gfx_property_get_value(map, prop, dest);
		GFXVectorIterator srcVal = _gfx_property_get_value(map, prop, src);

		/* Copy the memory */
		memcpy(destVal, srcVal, prop->size);
	}
}

/******************************************************/
static inline void _gfx_property_disable(

		struct GFX_Map*       map,
		struct GFX_Property*  prop)
{
	if(prop->size)
	{
		unsigned char type = _gfx_property_get_type(prop->type);
		size_t size = _gfx_property_get_total_size(map, prop);

		/* Check if any buffers or samplers are being removed */
		if(type == GFX_INT_PROPERTY_SAMPLER) --map->samplers;
		else if(type == GFX_INT_PROPERTY_BLOCK) --map->blocks;

		/* Adjust value index of all properties */
		struct GFX_Property* it;
		unsigned char properties = map->map.properties;

		for(it = (struct GFX_Property*)(map + 1); properties--; ++it)
			if(it->index > prop->index) it->index -= size;

		/* Erase from value vector */
		gfx_vector_erase_range_at(&map->values, size, prop->index);

		prop->index = 0;
		prop->size = 0;
	}

	_gfx_property_init(prop);
}

/******************************************************/
GFXPropertyMap* gfx_property_map_create(

		GFXProgram*    program,
		unsigned char  properties)
{
	/* Create new property map, append properties to end of struct */
	size_t size = sizeof(struct GFX_Map) + properties * sizeof(struct GFX_Property);

	struct GFX_Map* map = calloc(1, size);
	if(!map) return NULL;

	map->map.program = program;
	map->map.properties = properties;
	map->map.copies = 1;

	map->handle = _gfx_program_get_handle(program);

	/* Initialize all properties */
	struct GFX_Property* prop;

	for(prop = (struct GFX_Property*)(map + 1); properties--; ++prop)
		_gfx_property_init(prop);

	gfx_vector_init(&map->values, 1);

	return (GFXPropertyMap*)map;
}

/******************************************************/
void gfx_property_map_free(

		GFXPropertyMap* map)
{
	if(map)
	{
		struct GFX_Map* internal = (struct GFX_Map*)map;

		gfx_vector_clear(&internal->values);
		free(map);
	}
}

/******************************************************/
int gfx_property_map_expand(

		GFXPropertyMap*  map,
		size_t           num)
{
	/* Overflow :o */
	if(UCHAR_MAX - num < map->copies)
		return 0;

	struct GFX_Map* internal = (struct GFX_Map*)map;

	/* Calculate the needed memory */
	size_t bSize = gfx_vector_get_byte_size(&internal->values);

	struct GFX_Property* prop;
	unsigned char properties = map->properties;

	for(prop = (struct GFX_Property*)(internal + 1); properties--; ++prop)
		if(prop->type & GFX_INT_PROPERTY_HAS_COPIES) bSize += prop->size * num;

	/* Try to reserve the needed memory */
	/* This is done so the expand calls won't fail */
	if(!gfx_vector_reserve(&internal->values, bSize))
		return 0;

	/* Iterate and insert copies */
	properties = map->properties;

	for(prop = (struct GFX_Property*)(internal + 1); properties--; ++prop)
		_gfx_property_expand(internal, prop, num);

	/* Finally update copy count */
	map->copies += num;

	return 1;
}

/******************************************************/
size_t gfx_property_map_shrink(

		GFXPropertyMap*  map,
		size_t           num)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;

	/* Clamp copies to remove */
	if(num >= map->copies) num = map->copies - 1;

	/* Iterate and erase copies */
	struct GFX_Property* prop;
	unsigned char properties = map->properties;

	for(prop = (struct GFX_Property*)(internal + 1); properties--; ++prop)
		_gfx_property_shrink(internal, prop, num);

	/* Update copy count */
	map->copies -= num;

	return num;
}

/******************************************************/
int gfx_property_map_move(

		GFXPropertyMap*  map,
		size_t           dest,
		size_t           src)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;

	/* Validate indices */
	if(dest >= map->copies || src >= map->copies) return 0;

	/* Iterate and move */
	struct GFX_Property* prop;
	unsigned char properties = map->properties;

	for(prop = (struct GFX_Property*)(internal + 1); properties--; ++prop)
		_gfx_property_move(internal, prop, dest, src);

	return 1;
}

/******************************************************/
static int _gfx_property_map_forward(

		struct GFX_Map*       map,
		struct GFX_Property*  prop,
		unsigned char         type,
		GLuint                location,
		void*                 initData,
		size_t                initSize,
		size_t                dataSize,
		GFX_Extensions*       ext)
{
	/* Disable previous property with equal location */
	struct GFX_Property* it;
	unsigned char properties = map->map.properties;
	unsigned char propType = _gfx_property_get_type(type);

	for(it = (struct GFX_Property*)(map + 1); properties--; ++it)
		if(_gfx_property_get_type(it->type) == propType && it->location == location)
		{
			_gfx_property_disable(map, it);
			break;
		}

	/* Reset property */
	_gfx_property_disable(map, prop);
	prop->type = type;
	prop->location = location;

	/* Try to enable it */
	if(!_gfx_property_enable(map, prop, dataSize, ext))
	{
		_gfx_property_disable(map, prop);
		return 0;
	}

	/* Init values for all copies */
	size_t num = (type & GFX_INT_PROPERTY_HAS_COPIES) ? map->map.copies : 1;
	while(num--)
	{
		void* init = gfx_vector_at(
			&map->values,
			prop->index + prop->size * num
		);

		memcpy(init, initData, initSize);
	}

	return 1;
}

/******************************************************/
int gfx_property_map_forward(

		GFXPropertyMap*  map,
		unsigned char    index,
		int              copies,
		int              ptr,
		unsigned short   property)
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

	/* Get internal type */
	unsigned char type;
	switch(prop->type)
	{
		case GFX_VECTOR_PROPERTY :
			type = ptr ? GFX_INT_PROPERTY_VECTOR_PTR : GFX_INT_PROPERTY_VECTOR;
			break;

		case GFX_MATRIX_PROPERTY :
			type = ptr ? GFX_INT_PROPERTY_MATRIX_PTR : GFX_INT_PROPERTY_MATRIX;
			break;

		case GFX_SAMPLER_PROPERTY :
			type = GFX_INT_PROPERTY_SAMPLER;
			break;

		/* I don't know D: */
		default : return 0;
	}

	/* Get initialization params */
	struct GFX_Sampler samp;
	struct GFX_ValuePtr val;

	void* init;
	size_t initSize;
	size_t size;

	switch(type)
	{
		/* Value property */
		case GFX_INT_PROPERTY_VECTOR :
		case GFX_INT_PROPERTY_MATRIX :
		{
			GFXDataType dataType;
			dataType.unpacked = prop->dataType;

			initSize = sizeof(struct GFX_Value);
			size = initSize +
				(size_t)prop->components *
				(size_t)_gfx_sizeof_data_type(dataType) *
				(size_t)prop->count;

			val.val.type       = prop->dataType;
			val.val.components = prop->components;
			val.val.count      = prop->count;
			val.val.offset     = 0;
			val.val.offsetSize = 0;

			init = &val;
			break;
		}

		/* Pointer value property */
		case GFX_INT_PROPERTY_VECTOR_PTR :
		case GFX_INT_PROPERTY_MATRIX_PTR :
		{
			initSize = sizeof(struct GFX_ValuePtr);
			size = initSize;

			val.val.type       = prop->dataType;
			val.val.components = prop->components;
			val.val.count      = prop->count;
			val.val.offset     = 0;
			val.val.offsetSize = 0;
			val.ptr            = NULL;

			init = &val;
			break;
		}

		/* Sampler property */
		case GFX_INT_PROPERTY_SAMPLER :
		{
			initSize = sizeof(struct GFX_Sampler);
			size = initSize;

			samp.texture = 0;
			samp.target  = 0;

			init = &samp;
			break;
		}
	}

	return _gfx_property_map_forward(
		internal,
		forward,
		type | (copies ? GFX_INT_PROPERTY_HAS_COPIES : 0),
		location,
		init,
		initSize,
		size,
		&window->extensions
	);
}

/******************************************************/
int gfx_property_map_forward_named(

		GFXPropertyMap*  map,
		unsigned char    index,
		int              copies,
		int              ptr,
		const char*      name)
{
	/* Fetch property index and forward it */
	unsigned short property = gfx_program_get_named_property(
		map->program,
		name
	);

	if(property >= map->program->properties)
		return 0;

	return gfx_property_map_forward(
		map,
		index,
		copies,
		ptr,
		property
	);
}

/******************************************************/
int gfx_property_map_forward_block(

		GFXPropertyMap*  map,
		unsigned char    index,
		int              copies,
		unsigned short   block)
{
	/* Get current window, context and property */
	GFX_Window* window = _gfx_window_get_current();

	struct GFX_Map* internal = (struct GFX_Map*)map;
	struct GFX_Property* forward = _gfx_property_map_get_at(
		internal,
		index
	);

	/* Get block */
	const GFXPropertyBlock* bl = gfx_program_get_property_block(
		map->program,
		block
	);

	if(!window || !forward || !bl) return 0;
	if(!bl->numProperties || !bl->properties) return 0;

	/* Forward property */
	struct GFX_Block init;
	init.buffer = 0;
	init.offset = 0;
	init.size   = 0;

	return _gfx_property_map_forward(
		internal,
		forward,
		GFX_INT_PROPERTY_BLOCK | (copies ? GFX_INT_PROPERTY_HAS_COPIES : 0),
		block,
		&init,
		sizeof(struct GFX_Block),
		sizeof(struct GFX_Block),
		&window->extensions
	);
}

/******************************************************/
int gfx_property_map_forward_named_block(

		GFXPropertyMap*  map,
		unsigned char    index,
		int              copies,
		const char*      name)
{
	/* Fetch block index and forward it */
	unsigned short block = gfx_program_get_named_property_block(
		map->program,
		name
	);

	if(block >= map->program->blocks) return 0;

	return gfx_property_map_forward_block(
		map,
		index,
		copies,
		block
	);
}

/******************************************************/
int gfx_property_map_set_value(

		GFXPropertyMap*  map,
		unsigned char    index,
		size_t           copy,
		const void*      value,
		size_t           offset,
		size_t           size)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;
	struct GFX_Property* prop = _gfx_property_map_get_at(internal, index);

	if(!prop || copy >= map->copies) return 0;

	/* Check type */
	unsigned char type = _gfx_property_get_type(prop->type);
	if(type != GFX_INT_PROPERTY_VECTOR && type != GFX_INT_PROPERTY_MATRIX)
		return 0;

	/* Calculate size */
	size_t max = prop->size - sizeof(struct GFX_Value);
	offset = offset > max ? max : offset;

	max -= offset;
	size = (!size || size > max) ? max : size;

	/* Set value */
	struct GFX_Value* val = _gfx_property_get_value(internal, prop, copy);
	memcpy(GFX_PTR_ADD_BYTES(val + 1, offset), value, size);

	return 1;
}

/******************************************************/
int gfx_property_map_set_value_pointer(

		GFXPropertyMap*  map,
		unsigned char    index,
		size_t           copy,
		const void*      ptr)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;
	struct GFX_Property* prop = _gfx_property_map_get_at(internal, index);

	if(!prop || copy >= map->copies) return 0;

	/* Check type */
	unsigned char type = _gfx_property_get_type(prop->type);
	if(type != GFX_INT_PROPERTY_VECTOR_PTR && type != GFX_INT_PROPERTY_MATRIX_PTR)
		return 0;

	/* Set pointer */
	struct GFX_ValuePtr* val = _gfx_property_get_value(internal, prop, copy);
	val->ptr = ptr;

	return 1;
}

/******************************************************/
int gfx_property_map_set_instance_offset(

		GFXPropertyMap*  map,
		unsigned char    index,
		unsigned int     offset)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;
	struct GFX_Property* prop = _gfx_property_map_get_at(internal, index);

	if(!prop) return 0;

	/* Check type */
	unsigned char type = _gfx_property_get_type(prop->type);
	if(
		type != GFX_INT_PROPERTY_VECTOR &&
		type != GFX_INT_PROPERTY_VECTOR_PTR &&
		type != GFX_INT_PROPERTY_MATRIX &&
		type != GFX_INT_PROPERTY_MATRIX_PTR) return 0;

	/* Iterate through copies */
	size_t c;
	for(c = 0; c < map->copies; ++c)
	{
		struct GFX_Value* val = _gfx_property_get_value(internal, prop, c);

		/* Calculate array element size and multiply by offset */
		GFXDataType dataType;
		dataType.unpacked = val->type;

		val->offsetSize = offset;
		val->offset = offset *
			(size_t)val->components *
			(size_t)_gfx_sizeof_data_type(dataType);
	}

	return 1;
}

/******************************************************/
int gfx_property_map_set_sampler(

		GFXPropertyMap*    map,
		unsigned char      index,
		size_t             copy,
		const GFXTexture*  texture)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;
	struct GFX_Property* prop = _gfx_property_map_get_at(internal, index);

	if(!prop || copy >= map->copies) return 0;

	/* Check type */
	unsigned char type = _gfx_property_get_type(prop->type);
	if(type != GFX_INT_PROPERTY_SAMPLER) return 0;

	/* Set sampler */
	struct GFX_Sampler* samp = _gfx_property_get_value(internal, prop, copy);

	samp->texture = _gfx_texture_get_handle(texture);
	samp->target = _gfx_texture_get_internal_target(texture);

	return 1;
}

/******************************************************/
static int _gfx_property_map_set_block(

		struct GFX_Map*  map,
		unsigned char    index,
		size_t           copy,
		GLuint           buffer,
		size_t           offset,
		size_t           size)
{
	struct GFX_Property* prop = _gfx_property_map_get_at(map, index);
	if(!prop || copy >= map->map.copies) return 0;

	/* Check type */
	unsigned char type = _gfx_property_get_type(prop->type);
	if(type != GFX_INT_PROPERTY_BLOCK) return 0;

	/* Set block */
	struct GFX_Block* buff = _gfx_property_get_value(map, prop, copy);

	buff->buffer = buffer;
	buff->offset = offset;
	buff->size = size;

	return 1;
}

/******************************************************/
int gfx_property_map_set_buffer(

		GFXPropertyMap*   map,
		unsigned char     index,
		size_t            copy,
		const GFXBuffer*  buffer,
		size_t            offset,
		size_t            size)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;
	return _gfx_property_map_set_block(
		internal,
		index,
		copy,
		_gfx_buffer_get_handle(buffer),
		offset,
		size
	);
}

/******************************************************/
int gfx_property_map_set_shared_buffer(

		GFXPropertyMap*         map,
		unsigned char           index,
		size_t                  copy,
		const GFXSharedBuffer*  buffer,
		size_t                  offset,
		size_t                  size)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;
	return _gfx_property_map_set_block(
		internal,
		index,
		copy,
		_gfx_shared_buffer_get_handle(buffer),
		offset + buffer->offset,
		size
	);
}
