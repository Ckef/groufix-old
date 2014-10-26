/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/shading/internal.h"
#include "groufix/core/memory/internal.h"
#include "groufix/core/errors.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

/* Internal property types */
#define GFX_INT_PROPERTY_EMPTY       0x00
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

		unsigned char,
		GLuint,
		GLuint,
		void*,
		unsigned int,
		unsigned int,
		GFX_WIND_ARG);


/* Internal property map */
struct GFX_Map
{
	/* Super Class */
	GFXPropertyMap map;

	/* Hidden data */
	GFXVector      data;     /* Data of all properties */
	unsigned char  samplers; /* Number of sampler properties */
	unsigned char  blocks;   /* Number of block properties */
};

/* Internal property */
struct GFX_Property
{
	GLuint         handle;   /* OpenGL program handle */
	unsigned char  type;     /* Internal type and other flags */
	GLuint         location; /* Block index or uniform location */
	size_t         index;    /* Of value in value vector */
};

/* Internal vector/matrix (a.k.a value) header */
struct GFX_Value
{
	GFXUnpackedType  type;
	unsigned char    components;
	size_t           count;
	size_t           size; /* Size of the entire value (of a single copy) in bytes */
};

/******************************************************/
/* Internal vector/matrix pointer body */
struct GFX_ValuePtr
{
	const void*  ptr;
	size_t       size;   /* Max number of elements which can be read */
	size_t       offset; /* Per instance offset in array elements */
	size_t       bytes;  /* Per instance offset in bytes */
};

/* Internal sampler body */
struct GFX_Sampler
{
	GLuint texture;
	GLuint target;
};

/* Internal block body */
struct GFX_Block
{
	GLuint      buffer;
	GLintptr    offset;
	GLsizeiptr  size;
};

/******************************************************/
static int _gfx_property_map_eval_limits(

		struct GFX_Map*  map,
		unsigned char    type,
		GFX_WIND_ARG)
{
	switch(type)
	{
		/* Sampler property */
		case GFX_INT_PROPERTY_SAMPLER :

			if(++map->samplers > GFX_WIND_GET.lim[GFX_LIM_MAX_SAMPLER_PROPERTIES])
			{
				--map->samplers;
				gfx_errors_push(
					GFX_ERROR_INCOMPATIBLE_CONTEXT,
					"A Property Map attempted to forward too many sampler properties."
				);
				return 0;
			}
			return 1;

		/* Buffer property */
		case GFX_INT_PROPERTY_BLOCK :

			if(++map->blocks > GFX_WIND_GET.lim[GFX_LIM_MAX_BUFFER_PROPERTIES])
			{
				--map->blocks;
				gfx_errors_push(
					GFX_ERROR_INCOMPATIBLE_CONTEXT,
					"A Property Map attempted to forward too many buffer properties."
				);
				return 0;
			}
			return 1;

		/* No limits to be evaluated */
		default : return 1;
	}
}

/******************************************************/
static inline struct GFX_Property* _gfx_property_map_get_at(

		struct GFX_Map*  map,
		unsigned char    index)
{
	return index >= map->map.properties ?
		NULL : ((struct GFX_Property*)(map + 1)) + index;
}

/******************************************************/
static inline void* _gfx_property_get_data(

		struct GFX_Map*       map,
		struct GFX_Property*  prop)
{
	return gfx_vector_at(&map->data, prop->index);
}

/******************************************************/
static inline void* _gfx_property_get_copy(

		unsigned char  flags,
		void*          data,
		size_t         copySize,
		unsigned int   copy)
{
	return (flags & GFX_INT_PROPERTY_HAS_COPIES) ?
		GFX_PTR_ADD_BYTES(data, copySize * copy) :
		data;
}

/******************************************************/
static inline void* _gfx_property_derive_copy(

		unsigned char         flags,
		void*                 data,
		size_t*               copySize,
		unsigned int          copy)
{
	switch(flags & ~GFX_INT_PROPERTY_HAS_COPIES)
	{
		case GFX_INT_PROPERTY_VECTOR :
		case GFX_INT_PROPERTY_MATRIX :
			*copySize = ((struct GFX_Value*)data)->size;
			data = GFX_PTR_ADD_BYTES(data, sizeof(struct GFX_Value));
			break;

		case GFX_INT_PROPERTY_VECTOR_PTR :
		case GFX_INT_PROPERTY_MATRIX_PTR :
			*copySize = sizeof(struct GFX_ValuePtr);
			data = GFX_PTR_ADD_BYTES(data, sizeof(struct GFX_Value));
			break;

		case GFX_INT_PROPERTY_SAMPLER :
			*copySize = sizeof(struct GFX_Sampler);
			break;

		case GFX_INT_PROPERTY_BLOCK :
			*copySize = sizeof(struct GFX_Block);
			break;
	}

	return _gfx_property_get_copy(flags, data, *copySize, copy);
}

/******************************************************/
static void _gfx_property_set_empty(

		unsigned char  flags,
		GLuint         program,
		GLuint         location,
		void*          data,
		unsigned int   copy,
		unsigned int   base,
		GFX_WIND_ARG)
{
	/* Some unknown property type, ignore :D */
}

/******************************************************/
static inline void _gfx_property_set_vector_val(

		GLuint             program,
		GLuint             location,
		struct GFX_Value*  val,
		const void*        data,
		size_t             elements,
		GFX_WIND_ARG)
{
	switch(val->type)
	{
		case GFX_FLOAT : switch(val->components)
		{
			case 1 : GFX_REND_GET.ProgramUniform1fv(
				program, location, elements, data); return;
			case 2 : GFX_REND_GET.ProgramUniform2fv(
				program, location, elements, data); return;
			case 3 : GFX_REND_GET.ProgramUniform3fv(
				program, location, elements, data); return;
			case 4 : GFX_REND_GET.ProgramUniform4fv(
				program, location, elements, data); return;

			default : return;
		}

		case GFX_INT : switch(val->components)
		{
			case 1 : GFX_REND_GET.ProgramUniform1iv(
				program, location, elements, data); return;
			case 2 : GFX_REND_GET.ProgramUniform2iv(
				program, location, elements, data); return;
			case 3 : GFX_REND_GET.ProgramUniform3iv(
				program, location, elements, data); return;
			case 4 : GFX_REND_GET.ProgramUniform4iv(
				program, location, elements, data); return;

			default : return;
		}

		case GFX_UNSIGNED_INT : switch(val->components)
		{
			case 1 : GFX_REND_GET.ProgramUniform1uiv(
				program, location, elements, data); return;
			case 2 : GFX_REND_GET.ProgramUniform2uiv(
				program, location, elements, data); return;
			case 3 : GFX_REND_GET.ProgramUniform3uiv(
				program, location, elements, data); return;
			case 4 : GFX_REND_GET.ProgramUniform4uiv(
				program, location, elements, data); return;

			default : return;
		}

		/* Herp */
		default : return;
	}
}

/******************************************************/
static void _gfx_property_set_vector(

		unsigned char  flags,
		GLuint         program,
		GLuint         location,
		void*          data,
		unsigned int   copy,
		unsigned int   base,
		GFX_WIND_ARG)
{
	void* val = _gfx_property_get_copy(
		flags,
		GFX_PTR_ADD_BYTES(data, sizeof(struct GFX_Value)),
		((struct GFX_Value*)data)->size,
		copy);

	_gfx_property_set_vector_val(
		program,
		location,
		(struct GFX_Value*)data,
		val,
		((struct GFX_Value*)data)->count,
		GFX_WIND_AS_ARG);
}

/******************************************************/
static void _gfx_property_set_vector_ptr(

		unsigned char  flags,
		GLuint         program,
		GLuint         location,
		void*          data,
		unsigned int   copy,
		unsigned int   base,
		GFX_WIND_ARG)
{
	struct GFX_ValuePtr* ptr = _gfx_property_get_copy(
		flags,
		GFX_PTR_ADD_BYTES(data, sizeof(struct GFX_Value)),
		sizeof(struct GFX_ValuePtr),
		copy
	);

	/* Compute number of elements to upload */
	struct GFX_Value* val = (struct GFX_Value*)data;

	size_t elements = base * ptr->offset;
	elements = (elements > ptr->size) ? 0 : ptr->size - elements;

	_gfx_property_set_vector_val(
		program,
		location,
		val,
		GFX_PTR_ADD_BYTES(ptr->ptr, base * ptr->bytes),
		(elements > val->count) ? val->count : elements,
		GFX_WIND_AS_ARG
	);
}

/******************************************************/
static inline void _gfx_property_set_matrix_val(

		GLuint             program,
		GLuint             location,
		struct GFX_Value*  val,
		const void*        data,
		size_t             elements,
		GFX_WIND_ARG)
{
	switch(val->type)
	{
		case GFX_FLOAT : switch(val->components)
		{
			case 4  : GFX_REND_GET.ProgramUniformMatrix2fv(
				program, location, elements, GL_FALSE, data); return;
			case 8  : GFX_REND_GET.ProgramUniformMatrix3fv(
				program, location, elements, GL_FALSE, data); return;
			case 16 : GFX_REND_GET.ProgramUniformMatrix4fv(
				program, location, elements, GL_FALSE, data); return;

			default : return;
		}

		/* Derp */
		default : return;
	}
}

/******************************************************/
static void _gfx_property_set_matrix(

		unsigned char  flags,
		GLuint         program,
		GLuint         location,
		void*          data,
		unsigned int   copy,
		unsigned int   base,
		GFX_WIND_ARG)
{
	void* val = _gfx_property_get_copy(
		flags,
		GFX_PTR_ADD_BYTES(data, sizeof(struct GFX_Value)),
		((struct GFX_Value*)data)->size,
		copy);

	_gfx_property_set_matrix_val(
		program,
		location,
		(struct GFX_Value*)data,
		val,
		((struct GFX_Value*)data)->count,
		GFX_WIND_AS_ARG);
}

/******************************************************/
static void _gfx_property_set_matrix_ptr(

		unsigned char  flags,
		GLuint         program,
		GLuint         location,
		void*          data,
		unsigned int   copy,
		unsigned int   base,
		GFX_WIND_ARG)
{
	struct GFX_ValuePtr* ptr = _gfx_property_get_copy(
		flags,
		GFX_PTR_ADD_BYTES(data, sizeof(struct GFX_Value)),
		sizeof(struct GFX_ValuePtr),
		copy
	);

	/* Compute number of elements to upload */
	struct GFX_Value* val = (struct GFX_Value*)data;

	size_t elements = base * ptr->offset;
	elements = (elements > ptr->size) ? 0 : ptr->size - elements;

	_gfx_property_set_matrix_val(
		program,
		location,
		val,
		GFX_PTR_ADD_BYTES(ptr->ptr, base * ptr->bytes),
		(elements > val->count) ? val->count : elements,
		GFX_WIND_AS_ARG
	);
}

/******************************************************/
static void _gfx_property_set_sampler(

		unsigned char  flags,
		GLuint         program,
		GLuint         location,
		void*          data,
		unsigned int   copy,
		unsigned int   base,
		GFX_WIND_ARG)
{
	struct GFX_Sampler* val = _gfx_property_get_copy(
		flags,
		data,
		sizeof(struct GFX_Sampler),
		copy
	);

	/* Bind texture and upload binding point */
	int old;
	GLint unit = _gfx_binder_bind_texture(
		val->texture,
		val->target,
		1,
		&old,
		GFX_WIND_AS_ARG);

	if(!old) GFX_REND_GET.ProgramUniform1iv(
		program,
		location,
		1,
		&unit);
}

/******************************************************/
static void _gfx_property_set_block(

		unsigned char  flags,
		GLuint         program,
		GLuint         location,
		void*          data,
		unsigned int   copy,
		unsigned int   base,
		GFX_WIND_ARG)
{
	struct GFX_Block* val = _gfx_property_get_copy(
		flags,
		data,
		sizeof(struct GFX_Block),
		copy
	);

	/* Bind buffer and upload binding point */
	int old;
	size_t index = _gfx_binder_bind_uniform_buffer(
		val->buffer,
		val->offset,
		val->size,
		1,
		&old,
		GFX_WIND_AS_ARG);

	if(!old) GFX_REND_GET.UniformBlockBinding(
		program,
		location,
		index);
}

/******************************************************/
void _gfx_property_map_use(

		GFXPropertyMap*  map,
		unsigned int     copy,
		unsigned int     base,
		GFX_WIND_ARG)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;

	/* Use program map */
	_gfx_program_map_use(map->programMap, GFX_WIND_AS_ARG);

	/* Clamp copy */
	copy = (copy < map->copies) ? copy : map->copies - 1;

	/* Set all values of the program */
	struct GFX_Property* prop;
	unsigned char properties = map->properties;

	for(prop = (struct GFX_Property*)(internal + 1); properties--; ++prop)
	{
		/* Jump table */
		static const GFX_PropertyFunc jump[] =
		{
			_gfx_property_set_empty, /* Dummy method */

			_gfx_property_set_vector,
			_gfx_property_set_vector_ptr,
			_gfx_property_set_matrix,
			_gfx_property_set_matrix_ptr,
			_gfx_property_set_sampler,
			_gfx_property_set_block
		};

		/* Jump to function */
		jump[prop->type & ~GFX_INT_PROPERTY_HAS_COPIES](
			prop->type,
			prop->handle,
			prop->location,
			_gfx_property_get_data(internal, prop),
			copy,
			base,
			GFX_WIND_AS_ARG
		);
	}
}

/******************************************************/
GFXPropertyMap* gfx_property_map_create(

		GFXProgramMap*  programMap,
		unsigned char   properties)
{
	/* First of all, attempt to block the program map */
	if(!_gfx_program_map_block(programMap)) return NULL;

	/* Create new property map, append properties to end of struct */
	size_t size =
		sizeof(struct GFX_Map) +
		properties * sizeof(struct GFX_Property);

	struct GFX_Map* map = calloc(1, size);
	if(!map)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Property Map could not be allocated."
		);

		_gfx_program_map_unblock(programMap);
		return NULL;
	}

	map->map.programMap = programMap;
	map->map.properties = properties;
	map->map.copies = 1;

	/* Initialize all properties */
	struct GFX_Property* prop;
	for(prop = (struct GFX_Property*)(map + 1); properties--; ++prop)
	{
		prop->type = GFX_INT_PROPERTY_EMPTY;
		prop->location = GL_INVALID_INDEX;
	}

	gfx_vector_init(&map->data, 1);

	return (GFXPropertyMap*)map;
}

/******************************************************/
void gfx_property_map_free(

		GFXPropertyMap* map)
{
	if(map)
	{
		struct GFX_Map* internal = (struct GFX_Map*)map;

		/* Unblock the associated program map */
		_gfx_program_map_unblock(map->programMap);

		gfx_vector_clear(&internal->data);
		free(map);
	}
}

/******************************************************/
int gfx_property_map_expand(

		GFXPropertyMap*  map,
		unsigned int     num)
{
	if(!num) return 0;

	/* Overflow :o */
	if(UINT_MAX - num < map->copies)
	{
		/* Overflow error */
		gfx_errors_push(
			GFX_ERROR_OVERFLOW,
			"Overflow occurred during expansion of copies at a property map."
		);
		return 0;
	}

	struct GFX_Map* internal = (struct GFX_Map*)map;

	/* Calculate the needed memory */
	size_t dSize = gfx_vector_get_byte_size(&internal->data);

	struct GFX_Property* prop;
	unsigned char properties = map->properties;

	for(prop = (struct GFX_Property*)(internal + 1); properties--; ++prop)
		if(prop->type & GFX_INT_PROPERTY_HAS_COPIES)
		{
			size_t copySize = 0;

			GFXVectorIterator it = _gfx_property_get_data(
				internal, prop);
			_gfx_property_derive_copy(
				prop->type, it, &copySize, 0);

			dSize += copySize * num;
		}

	/* Try to reserve the needed memory */
	/* This is done so we don't have to reverse when a single property fails */
	if(!gfx_vector_reserve(&internal->data, dSize))
		return 0;

	/* Iterate and insert copies */
	properties = map->properties;

	for(prop = (struct GFX_Property*)(internal + 1); properties--; ++prop)
		if(prop->type & GFX_INT_PROPERTY_HAS_COPIES)
		{
			size_t copySize = 0;

			GFXVectorIterator it = _gfx_property_get_data(
				internal, prop);
			it = _gfx_property_derive_copy(
				prop->type, it, &copySize, map->copies);

			copySize *= num;
			it = gfx_vector_insert_range(
				&internal->data,
				copySize,
				NULL,
				it);

			/* Initialize to 0s */
			memset(it, 0, copySize);

			/* Adjust value index of all properties */
			struct GFX_Property* Iprop;
			unsigned char Iprops = map->properties;

			for(Iprop = (struct GFX_Property*)(internal + 1); Iprops--; ++Iprop)
				if(Iprop->index > prop->index) Iprop->index += copySize;
		}

	/* Yey */
	map->copies += num;

	return 1;
}

/******************************************************/
unsigned int gfx_property_map_shrink(

		GFXPropertyMap*  map,
		unsigned int     num)
{
	if(!num) return 0;
	struct GFX_Map* internal = (struct GFX_Map*)map;

	/* Clamp copies to remove */
	if(num >= map->copies) num = map->copies - 1;
	map->copies -= num;

	/* Iterate over properties and erase */
	struct GFX_Property* prop;
	unsigned char properties = map->properties;

	for(prop = (struct GFX_Property*)(internal + 1); properties--; ++prop)
		if(prop->type & GFX_INT_PROPERTY_HAS_COPIES)
		{
			size_t copySize = 0;

			GFXVectorIterator it = _gfx_property_get_data(
				internal, prop);
			it = _gfx_property_derive_copy(
				prop->type, it, &copySize, map->copies);

			copySize *= num;
			gfx_vector_erase_range(
				&internal->data,
				copySize,
				it);

			/* Adjust value index of all properties */
			struct GFX_Property* Iprop;
			unsigned char Iprops = map->properties;

			for(Iprop = (struct GFX_Property*)(internal + 1); Iprops--; ++Iprop)
				if(Iprop->index > prop->index) Iprop->index -= copySize;
		}

	return num;
}

/******************************************************/
int gfx_property_map_move(

		GFXPropertyMap*  map,
		unsigned int     dest,
		unsigned int     src)
{
	struct GFX_Map* internal = (struct GFX_Map*)map;

	/* Validate indices */
	if(dest >= map->copies || src >= map->copies) return 0;
	if(dest == src) return 1;

	/* Iterate over properties and move */
	struct GFX_Property* prop;
	unsigned char properties = map->properties;

	for(prop = (struct GFX_Property*)(internal + 1); properties--; ++prop)
		if(prop->type & GFX_INT_PROPERTY_HAS_COPIES)
		{
			size_t copySize = 0;

			GFXVectorIterator it =
				_gfx_property_get_data(internal, prop);
			void* destVal =
				_gfx_property_derive_copy(prop->type, it, &copySize, dest);
			void* srcVal =
				_gfx_property_derive_copy(prop->type, it, &copySize, src);

			/* Copy */
			memcpy(destVal, srcVal, copySize);
		}

	return 1;
}

/******************************************************/
static void _gfx_property_map_disable(

		struct GFX_Map*       map,
		struct GFX_Property*  prop)
{
	if(prop->type != GFX_INT_PROPERTY_EMPTY)
	{
		unsigned char type = prop->type & ~GFX_INT_PROPERTY_HAS_COPIES;

		/* Check if any limits are being negated */
		switch(type)
		{
			case GFX_INT_PROPERTY_SAMPLER : --map->samplers; break;
			case GFX_INT_PROPERTY_BLOCK : --map->blocks; break;
		}

		/* Get the range to remove from the data */
		struct GFX_Property* it;
		size_t end = gfx_vector_get_byte_size(&map->data);
		unsigned char properties = map->map.properties;

		for(it = (struct GFX_Property*)(map + 1); properties--; ++it)
			if(it->index > prop->index && it->index < end) end = it->index;

		/* Adjust value index of all properties */
		size_t offset = end - prop->index;
		properties = map->map.properties;

		for(it = (struct GFX_Property*)(map + 1); properties--; ++it)
			if(it->index > prop->index) it->index -= offset;

		/* Erase from data */
		gfx_vector_erase_range_at(&map->data, offset, prop->index);

		prop->handle   = 0;
		prop->type     = GFX_INT_PROPERTY_EMPTY;
		prop->location = GL_INVALID_INDEX;
		prop->index    = 0;
	}
}

/******************************************************/
static int _gfx_property_map_forward(

		struct GFX_Map*       map,
		struct GFX_Property*  prop,
		const void*           headData,
		size_t                headSize,
		size_t                copySize,
		GFX_WIND_ARG)
{
	/* Attempt to allocate */
	size_t size = headSize + copySize *
		(prop->type & GFX_INT_PROPERTY_HAS_COPIES ? map->map.copies : 1);

	GFXVectorIterator it = gfx_vector_insert_range(
		&map->data,
		size,
		NULL,
		map->data.end
	);

	if(it != map->data.end)
	{
		/* Disable any identical properties */
		struct GFX_Property* disable;
		unsigned char type = prop->type & ~GFX_INT_PROPERTY_HAS_COPIES;
		unsigned char properties = map->map.properties;

		for(disable = (struct GFX_Property*)(map + 1); properties--; ++disable)
			if(
				disable != prop &&
				(disable->type & ~GFX_INT_PROPERTY_HAS_COPIES) == type &&
				disable->location == prop->location)
			{
				_gfx_property_map_disable(map, disable);
			}

		/* Check limits */
		if(_gfx_property_map_eval_limits(map, type, GFX_WIND_AS_ARG))
		{
			/* Initialize memory and set index */
			memset(GFX_PTR_ADD_BYTES(it, headSize), 0, size - headSize);
			if(headSize) memcpy(it, headData, headSize);

			prop->index = gfx_vector_get_index(&map->data, it);

			return 1;
		}

		/* Nevermind, remove memory */
		gfx_vector_erase_range(&map->data, size, it);
	}

	/* Failed */
	prop->handle   = 0;
	prop->type     = GFX_INT_PROPERTY_EMPTY;
	prop->location = GL_INVALID_INDEX;
	prop->index    = 0;

	return 0;
}

/******************************************************/
int gfx_property_map_forward(

		GFXPropertyMap*  map,
		unsigned char    index,
		int              copies,
		int              ptr,
		GFXShaderStage   stage,
		unsigned short   property)
{
	GFX_WIND_INIT(0);

	/* Get program */
	GFXProgram* prog = gfx_program_map_get(map->programMap, stage);
	if(!prog) return 0;

	struct GFX_Map* internal =
		(struct GFX_Map*)map;
	struct GFX_Property* forward =
		_gfx_property_map_get_at(internal, index);

	/* Get property and location */
	GLint location =
		_gfx_program_get_location(prog, property);
	const GFXProperty* prop =
		gfx_program_get_property(prog, property);

	if(!forward || location < 0 || !prop)
		return 0;
	if(!prop->components || !prop->count)
		return 0;

	/* First forcefully disable the property */
	_gfx_property_map_disable(internal, forward);

	/* Calcualte copy size for values */
	struct GFX_Value head;
	head.type       = prop->dataType;
	head.components = prop->components;
	head.count      = prop->count;

	GFXDataType dataType;
	dataType.unpacked = prop->dataType;

	head.size =
		(size_t)prop->components *
		(size_t)_gfx_sizeof_data_type(dataType) *
		(size_t)prop->count;

	/* Get internal type & size */
	size_t headSize = 0;
	size_t copySize = 0;

	switch(prop->type)
	{
		case GFX_VECTOR_PROPERTY :
			forward->type = ptr ? GFX_INT_PROPERTY_VECTOR_PTR : GFX_INT_PROPERTY_VECTOR;
			headSize = sizeof(struct GFX_Value);
			copySize = ptr ? sizeof(struct GFX_ValuePtr) : head.size;
			break;

		case GFX_MATRIX_PROPERTY :
			forward->type = ptr ? GFX_INT_PROPERTY_MATRIX_PTR : GFX_INT_PROPERTY_MATRIX;
			headSize = sizeof(struct GFX_Value);
			copySize = ptr ? sizeof(struct GFX_ValuePtr) : head.size;
			break;

		case GFX_SAMPLER_PROPERTY :
			forward->type = GFX_INT_PROPERTY_SAMPLER;
			copySize = sizeof(struct GFX_Sampler);
			break;

		/* I don't know D: */
		default : return 0;
	}

	/* Set handle, type and location */
	forward->handle = _gfx_program_get_handle(prog);
	forward->type |= copies ? GFX_INT_PROPERTY_HAS_COPIES : 0;
	forward->location = location;

	/* Forward all data */
	return _gfx_property_map_forward(
		internal,
		forward,
		&head,
		headSize,
		copySize,
		GFX_WIND_AS_ARG
	);
}

/******************************************************/
int gfx_property_map_forward_named(

		GFXPropertyMap*  map,
		unsigned char    index,
		int              copies,
		int              ptr,
		GFXShaderStage   stage,
		const char*      name)
{
	/* Get program */
	GFXProgram* prog = gfx_program_map_get(map->programMap, stage);
	if(!prog) return 0;

	/* Fetch property index and forward it */
	unsigned short prop = gfx_program_get_named_property(prog, name);
	if(prop >= prog->properties) return 0;

	return gfx_property_map_forward(map, index, copies, ptr, stage, prop);
}

/******************************************************/
int gfx_property_map_forward_block(

		GFXPropertyMap*  map,
		unsigned char    index,
		int              copies,
		GFXShaderStage   stage,
		unsigned short   block)
{
	GFX_WIND_INIT(0);

	/* Get program */
	GFXProgram* prog = gfx_program_map_get(map->programMap, stage);
	if(!prog) return 0;

	struct GFX_Map* internal =
		(struct GFX_Map*)map;
	struct GFX_Property* forward =
		_gfx_property_map_get_at(internal, index);

	/* Get block */
	const GFXPropertyBlock* bl =
		gfx_program_get_property_block(prog, block);

	if(!forward || !bl)
		return 0;
	if(!bl->numProperties || !bl->properties)
		return 0;

	/* First forcefully disable the property */
	_gfx_property_map_disable(internal, forward);

	/* Set handle, type and location */
	forward->handle =
		_gfx_program_get_handle(prog);

	forward->type = GFX_INT_PROPERTY_BLOCK |
		(copies ? GFX_INT_PROPERTY_HAS_COPIES : 0);

	forward->location = block;

	/* Forward all data */
	return _gfx_property_map_forward(
		internal,
		forward,
		NULL,
		0,
		sizeof(struct GFX_Block),
		GFX_WIND_AS_ARG
	);
}

/******************************************************/
int gfx_property_map_forward_named_block(

		GFXPropertyMap*  map,
		unsigned char    index,
		int              copies,
		GFXShaderStage   stage,
		const char*      name)
{
	/* Get program */
	GFXProgram* prog = gfx_program_map_get(map->programMap, stage);
	if(!prog) return 0;

	/* Fetch block index and forward it */
	unsigned short block = gfx_program_get_named_property_block(prog, name);
	if(block >= prog->blocks) return 0;

	return gfx_property_map_forward_block(map, index, copies, stage, block);
}

/******************************************************/
int gfx_property_map_set_value(

		GFXPropertyMap*  map,
		unsigned char    index,
		unsigned int     copy,
		const void*      value,
		size_t           offset,
		size_t           size)
{
	struct GFX_Map* internal =
		(struct GFX_Map*)map;
	struct GFX_Property* prop =
		_gfx_property_map_get_at(internal, index);

	if(!prop || copy >= map->copies) return 0;

	/* Check type */
	unsigned char type = prop->type & ~GFX_INT_PROPERTY_HAS_COPIES;
	if(
		type != GFX_INT_PROPERTY_VECTOR &&
		type != GFX_INT_PROPERTY_MATRIX)
	{
		return 0;
	}

	/* Get data */
	struct GFX_Value* val = _gfx_property_get_data(
		internal,
		prop);

	void* data = _gfx_property_get_copy(
		prop->type,
		GFX_PTR_ADD_BYTES(val, sizeof(struct GFX_Value)),
		val->size,
		copy);

	/* Calculate size */
	offset = offset > val->size ? val->size : offset;
	size_t max = val->size - offset;

	size = (!size || size > max) ? max : size;

	/* Set data */
	memcpy(GFX_PTR_ADD_BYTES(data, offset), value, size);

	return 1;
}

/******************************************************/
int gfx_property_map_set_value_pointer(

		GFXPropertyMap*  map,
		unsigned char    index,
		unsigned int     copy,
		const void*      ptr,
		size_t           offset,
		size_t           size)
{
	struct GFX_Map* internal =
		(struct GFX_Map*)map;
	struct GFX_Property* prop =
		_gfx_property_map_get_at(internal, index);

	if(!prop || copy >= map->copies || offset > size) return 0;

	/* Check type */
	unsigned char type = prop->type & ~GFX_INT_PROPERTY_HAS_COPIES;
	if(
		type != GFX_INT_PROPERTY_VECTOR_PTR &&
		type != GFX_INT_PROPERTY_MATRIX_PTR)
	{
		return 0;
	}

	/* Get data */
	struct GFX_Value* val = _gfx_property_get_data(
		internal,
		prop);

	struct GFX_ValuePtr* data = _gfx_property_get_copy(
		prop->type,
		GFX_PTR_ADD_BYTES(val, sizeof(struct GFX_Value)),
		sizeof(struct GFX_ValuePtr),
		copy);

	/* Scale offset by array element size */
	GFXDataType dataType;
	dataType.unpacked = val->type;

	size_t byteOffset =
		offset *
		(size_t)val->components *
		(size_t)_gfx_sizeof_data_type(dataType);

	/* Set data */
	data->ptr = GFX_PTR_ADD_BYTES(ptr, byteOffset);
	data->size = size - offset;

	return 1;
}

/******************************************************/
int gfx_property_map_set_instance_offset(

		GFXPropertyMap*  map,
		unsigned char    index,
		unsigned int     copy,
		unsigned int     offset)
{
	struct GFX_Map* internal =
		(struct GFX_Map*)map;
	struct GFX_Property* prop =
		_gfx_property_map_get_at(internal, index);

	if(!prop || copy >= map->copies) return 0;

	/* Check type */
	unsigned char type = prop->type & ~GFX_INT_PROPERTY_HAS_COPIES;
	if(
		type != GFX_INT_PROPERTY_VECTOR_PTR &&
		type != GFX_INT_PROPERTY_MATRIX_PTR)
	{
		return 0;
	}

	/* Get data */
	struct GFX_Value* val = _gfx_property_get_data(
		internal,
		prop);

	struct GFX_ValuePtr* data = _gfx_property_get_copy(
		prop->type,
		GFX_PTR_ADD_BYTES(val, sizeof(struct GFX_Value)),
		sizeof(struct GFX_ValuePtr),
		copy);

	/* Calculate array element size and multiply by offset */
	GFXDataType dataType;
	dataType.unpacked = val->type;

	data->offset = offset;
	data->bytes = offset *
		(size_t)val->components *
		(size_t)_gfx_sizeof_data_type(dataType);

	return 1;
}

/******************************************************/
int gfx_property_map_set_sampler(

		GFXPropertyMap*    map,
		unsigned char      index,
		unsigned int       copy,
		const GFXTexture*  texture)
{
	struct GFX_Map* internal =
		(struct GFX_Map*)map;
	struct GFX_Property* prop =
		_gfx_property_map_get_at(internal, index);

	if(!prop || copy >= map->copies) return 0;

	/* Check type */
	unsigned char type = prop->type & ~GFX_INT_PROPERTY_HAS_COPIES;
	if(type != GFX_INT_PROPERTY_SAMPLER) return 0;

	/* Get data */
	struct GFX_Sampler* samp = _gfx_property_get_data(
		internal,
		prop);

	samp = _gfx_property_get_copy(
		prop->type,
		samp,
		sizeof(struct GFX_Sampler),
		copy);

	/* Set data */
	samp->texture = _gfx_texture_get_handle(texture);
	samp->target = _gfx_texture_get_internal_target(texture);

	return 1;
}

/******************************************************/
static int _gfx_property_map_set_block(

		struct GFX_Map*  map,
		unsigned char    index,
		unsigned int     copy,
		GLuint           buffer,
		size_t           offset,
		size_t           size)
{
	struct GFX_Property* prop =
		_gfx_property_map_get_at(map, index);

	if(!prop || copy >= map->map.copies) return 0;

	/* Check type */
	unsigned char type = prop->type & ~GFX_INT_PROPERTY_HAS_COPIES;
	if(type != GFX_INT_PROPERTY_BLOCK) return 0;

	/* Get data */
	struct GFX_Block* block = _gfx_property_get_data(
		map,
		prop);

	block = _gfx_property_get_copy(
		prop->type,
		block,
		sizeof(struct GFX_Block),
		copy);

	/* Set data */
	block->buffer = buffer;
	block->offset = offset;
	block->size = size;

	return 1;
}

/******************************************************/
int gfx_property_map_set_buffer(

		GFXPropertyMap*   map,
		unsigned char     index,
		unsigned int      copy,
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
		unsigned int            copy,
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
