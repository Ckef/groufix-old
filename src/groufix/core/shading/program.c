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

#include "groufix/core/errors.h"
#include "groufix/core/internal.h"
#include "groufix/containers/vector.h"

#include <stdlib.h>


/* Define unsupported float uniform types */
#ifndef GL_SAMPLER_1D
	#define GL_SAMPLER_1D                                 -0x0001
#endif
#ifndef GL_SAMPLER_1D_SHADOW
	#define GL_SAMPLER_1D_SHADOW                          -0x0002
#endif
#ifndef GL_SAMPLER_BUFFER
	#define GL_SAMPLER_BUFFER                             -0x0003
#endif
#ifndef GL_SAMPLER_1D_ARRAY
	#define GL_SAMPLER_1D_ARRAY                           -0x0004
#endif
#ifndef GL_SAMPLER_1D_ARRAY_SHADOW
	#define GL_SAMPLER_1D_ARRAY_SHADOW                    -0x0005
#endif
#ifndef GL_SAMPLER_2D_MULTISAMPLE_ARRAY
	#define GL_SAMPLER_2D_MULTISAMPLE_ARRAY               -0x0006
#endif

/* Define unsupported int uniform types */
#ifndef GL_INT_SAMPLER_1D
	#define GL_INT_SAMPLER_1D                             -0x0007
#endif
#ifndef GL_INT_SAMPLER_BUFFER
	#define GL_INT_SAMPLER_BUFFER                         -0x0008
#endif
#ifndef GL_INT_SAMPLER_1D_ARRAY
	#define GL_INT_SAMPLER_1D_ARRAY                       -0x0009
#endif
#ifndef GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
	#define GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY           -0x000a
#endif

/* Define unsupported unsigned int uniform types */
#ifndef GL_UNSIGNED_INT_SAMPLER_1D
	#define GL_UNSIGNED_INT_SAMPLER_1D                    -0x000b
#endif
#ifndef GL_UNSIGNED_INT_SAMPLER_BUFFER
	#define GL_UNSIGNED_INT_SAMPLER_BUFFER                -0x000c
#endif
#ifndef GL_UNSIGNED_INT_SAMPLER_1D_ARRAY
	#define GL_UNSIGNED_INT_SAMPLER_1D_ARRAY              -0x000d
#endif
#ifndef GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
	#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY  -0x000e
#endif


/******************************************************/
/* Internal Property */
struct GFX_Property
{
	GFXProperty  property; /* Super class */
	GLint        location;
};

/* Internal Program */
struct GFX_Program
{
	/* Super class */
	GFXProgram program;

	/* Hidden data */
	GLuint           handle;     /* OpenGL handle */
	GFXVector        properties; /* Stores GFX_Property */
	GFXVector        blocks;     /* Stores GFXPropertyBlock */
};

/******************************************************/
static void _gfx_program_uniform_type_to_property(

		GFXProperty*  property,
		GLenum        type)
{
	switch(type)
	{
		/* Float vectors */
		case GL_FLOAT :
			property->type       = GFX_VECTOR_PROPERTY;
			property->dataType   = GFX_FLOAT;
			property->components = 1;
			break;

		case GL_FLOAT_VEC2 :
			property->type       = GFX_VECTOR_PROPERTY;
			property->dataType   = GFX_FLOAT;
			property->components = 2;
			break;

		case GL_FLOAT_VEC3 :
			property->type       = GFX_VECTOR_PROPERTY;
			property->dataType   = GFX_FLOAT;
			property->components = 3;
			break;

		case GL_FLOAT_VEC4 :
			property->type       = GFX_VECTOR_PROPERTY;
			property->dataType   = GFX_FLOAT;
			property->components = 4;
			break;

		/* Int vectors */
		case GL_INT :
			property->type       = GFX_VECTOR_PROPERTY;
			property->dataType   = GFX_INT;
			property->components = 1;
			break;

		case GL_INT_VEC2 :
			property->type       = GFX_VECTOR_PROPERTY;
			property->dataType   = GFX_INT;
			property->components = 2;
			break;

		case GL_INT_VEC3 :
			property->type       = GFX_VECTOR_PROPERTY;
			property->dataType   = GFX_INT;
			property->components = 3;
			break;

		case GL_INT_VEC4 :
			property->type       = GFX_VECTOR_PROPERTY;
			property->dataType   = GFX_INT;
			property->components = 4;
			break;

		/* Unsigned int vectors */
		case GL_UNSIGNED_INT :
			property->type       = GFX_VECTOR_PROPERTY;
			property->dataType   = GFX_UNSIGNED_INT;
			property->components = 1;
			break;

		case GL_UNSIGNED_INT_VEC2 :
			property->type       = GFX_VECTOR_PROPERTY;
			property->dataType   = GFX_UNSIGNED_INT;
			property->components = 2;
			break;

		case GL_UNSIGNED_INT_VEC3 :
			property->type       = GFX_VECTOR_PROPERTY;
			property->dataType   = GFX_UNSIGNED_INT;
			property->components = 3;
			break;

		case GL_UNSIGNED_INT_VEC4 :
			property->type       = GFX_VECTOR_PROPERTY;
			property->dataType   = GFX_UNSIGNED_INT;
			property->components = 4;
			break;

		/* Matrices */
		case GL_FLOAT_MAT2 :
			property->type       = GFX_MATRIX_PROPERTY;
			property->dataType   = GFX_FLOAT;
			property->components = 4;
			break;

		case GL_FLOAT_MAT3 :
			property->type       = GFX_MATRIX_PROPERTY;
			property->dataType   = GFX_FLOAT;
			property->components = 9;
			break;

		case GL_FLOAT_MAT4 :
			property->type       = GFX_MATRIX_PROPERTY;
			property->dataType   = GFX_FLOAT;
			property->components = 16;
			break;

		/* Samplers */
		case GL_SAMPLER_1D :
		case GL_SAMPLER_1D_SHADOW :
		case GL_SAMPLER_BUFFER :
			property->type       = GFX_SAMPLER_PROPERTY;
			property->dataType   = GFX_FLOAT;
			property->components = 1;
			break;

		case GL_SAMPLER_2D :
		case GL_SAMPLER_2D_SHADOW :
		case GL_SAMPLER_1D_ARRAY :
		case GL_SAMPLER_1D_ARRAY_SHADOW :
		case GL_SAMPLER_2D_MULTISAMPLE :
			property->type       = GFX_SAMPLER_PROPERTY;
			property->dataType   = GFX_FLOAT;
			property->components = 2;
			break;

		case GL_SAMPLER_3D :
		case GL_SAMPLER_CUBE :
		case GL_SAMPLER_2D_ARRAY :
		case GL_SAMPLER_2D_ARRAY_SHADOW :
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY :
		case GL_SAMPLER_CUBE_SHADOW :
			property->type       = GFX_SAMPLER_PROPERTY;
			property->dataType   = GFX_FLOAT;
			property->components = 3;
			break;

		case GL_INT_SAMPLER_1D :
		case GL_INT_SAMPLER_BUFFER :
			property->type       = GFX_SAMPLER_PROPERTY;
			property->dataType   = GFX_INT;
			property->components = 1;
			break;

		case GL_INT_SAMPLER_2D :
		case GL_INT_SAMPLER_1D_ARRAY :
		case GL_INT_SAMPLER_2D_MULTISAMPLE :
			property->type       = GFX_SAMPLER_PROPERTY;
			property->dataType   = GFX_INT;
			property->components = 2;
			break;

		case GL_INT_SAMPLER_3D :
		case GL_INT_SAMPLER_CUBE :
		case GL_INT_SAMPLER_2D_ARRAY :
		case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY :
			property->type       = GFX_SAMPLER_PROPERTY;
			property->dataType   = GFX_INT;
			property->components = 3;
			break;

		case GL_UNSIGNED_INT_SAMPLER_1D :
		case GL_UNSIGNED_INT_SAMPLER_BUFFER :
			property->type       = GFX_SAMPLER_PROPERTY;
			property->dataType   = GFX_UNSIGNED_INT;
			property->components = 1;
			break;

		case GL_UNSIGNED_INT_SAMPLER_2D :
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY :
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE :
			property->type       = GFX_SAMPLER_PROPERTY;
			property->dataType   = GFX_UNSIGNED_INT;
			property->components = 2;
			break;

		case GL_UNSIGNED_INT_SAMPLER_3D :
		case GL_UNSIGNED_INT_SAMPLER_CUBE :
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY :
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY :
			property->type       = GFX_SAMPLER_PROPERTY;
			property->dataType   = GFX_UNSIGNED_INT;
			property->components = 3;
			break;

		/* Uuuuuh... */
		default :
			property->type       = GFX_VECTOR_PROPERTY;
			property->dataType   = GFX_FLOAT;
			property->components = 0;
			break;
	}
}

/******************************************************/
static unsigned short _gfx_program_prepare_properties(

		struct GFX_Program*  program,
		unsigned short       properties,
		GFX_Extensions*      ext)
{
	/* Reserve memory */
	gfx_vector_clear(&program->properties);
	if(!gfx_vector_reserve(&program->properties, properties))
		return 0;

	/* Retrieve uniforms */
	GLuint indices[properties];
	GLint lens[properties];

	size_t i;
	for(i = 0; i < properties; ++i) indices[i] = i;
	ext->GetActiveUniformsiv(
		program->handle,
		properties,
		indices,
		GL_UNIFORM_NAME_LENGTH,
		lens
	);

	/* Create properties */
	for(i = 0; i < properties; ++i)
	{
		/* Get the uniform location */
		char name[lens[i]];
		GLint size;
		GLenum type;

		ext->GetActiveUniform(
			program->handle,
			i,
			lens[i],
			NULL,
			&size,
			&type,
			name);

		GLint loc = ext->GetUniformLocation(
			program->handle,
			name);

		/* Create and insert */
		struct GFX_Property prop;

		_gfx_program_uniform_type_to_property(&prop.property, type);
		prop.property.count = prop.property.components ? size : 0;
		prop.location = loc;

		/* Finally insert */
		gfx_vector_insert(
			&program->properties,
			&prop,
			program->properties.end
		);
	}

	return properties;
}

/******************************************************/
static unsigned short _gfx_program_prepare_blocks(

		struct GFX_Program*  program,
		unsigned short       blocks,
		GFX_Extensions*      ext)
{
	/* Reserve memory */
	gfx_vector_clear(&program->blocks);
	if(!gfx_vector_reserve(&program->blocks, blocks)) return 0;

	/* Retrieve uniform blocks */
	size_t k;
	for(k = 0; k < blocks; ++k)
	{
		GLint size;
		ext->GetActiveUniformBlockiv(
			program->handle,
			k,
			GL_UNIFORM_BLOCK_DATA_SIZE,
			&size
		);

		/* Get associated uniform indices */
		GLint uniforms;
		ext->GetActiveUniformBlockiv(
			program->handle,
			k,
			GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,
			&uniforms
		);

		GLint uniformIndices[uniforms];
		ext->GetActiveUniformBlockiv(
			program->handle,
			k,
			GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
			uniformIndices
		);

		/* Retrieve information for all associated uniforms */
		GLuint indices[uniforms];
		GLint offsets[uniforms];
		GLint arrStrides[uniforms];
		GLint matStrides[uniforms];

		size_t j;
		for(j = 0; j < uniforms; ++j)
			indices[j] = uniformIndices[j];

		ext->GetActiveUniformsiv(
			program->handle,
			uniforms,
			indices,
			GL_UNIFORM_OFFSET,
			offsets);

		ext->GetActiveUniformsiv(
			program->handle,
			uniforms,
			indices,
			GL_UNIFORM_ARRAY_STRIDE,
			arrStrides);

		ext->GetActiveUniformsiv(
			program->handle,
			uniforms,
			indices,
			GL_UNIFORM_MATRIX_STRIDE,
			matStrides);

		/* Create block property */
		GFXPropertyBlock block;
		block.size          = size;
		block.numProperties = uniforms;
		block.properties    = malloc(sizeof(GFXPropertyBlockMember) * uniforms);

		if(block.properties) for(j = 0; j < uniforms; ++j)
		{
			block.properties[j].property = indices[j];
			block.properties[j].offset = offsets[j];
			block.properties[j].stride =
				(matStrides[j] > 0) ? matStrides[j] :
				(arrStrides[j] > 0) ? arrStrides[j] :
				0;
		}

		gfx_vector_insert(
			&program->blocks,
			&block,
			program->blocks.end
		);
	}

	return blocks;
}

/******************************************************/
static void _gfx_program_unprepare(

		struct GFX_Program* program)
{
	/* Free all property block members */
	GFXVectorIterator it;
	for(
		it = program->blocks.begin;
		it != program->blocks.end;
		it = gfx_vector_next(&program->blocks, it))
	{
		GFXPropertyBlock* bl = (GFXPropertyBlock*)it;

		free(bl->properties);
		bl->properties = NULL;
		bl->numProperties = 0;
	}
}

/******************************************************/
static void _gfx_program_prepare(

		struct GFX_Program*  program,
		GFX_Extensions*      ext)
{
	_gfx_program_unprepare(program);

	/* Get number of properties */
	GLint properties;
	GLint blocks;

	ext->GetProgramiv(program->handle, GL_ACTIVE_UNIFORMS, &properties);
	ext->GetProgramiv(program->handle, GL_ACTIVE_UNIFORM_BLOCKS, &blocks);

	properties = properties < 0 ? 0 : properties;
	blocks = blocks < 0 ? 0 : blocks;

	/* Prepare properties and blocks */
	program->program.properties =
		_gfx_program_prepare_properties(program, properties, ext);
	program->program.blocks =
		_gfx_program_prepare_blocks(program, blocks, ext);
}

/******************************************************/
static void _gfx_program_obj_free(

		void*            object,
		GFX_Extensions*  ext)
{
	struct GFX_Program* program = (struct GFX_Program*)object;

	program->handle = 0;
	program->program.id = 0;
}

/******************************************************/
/* vtable for hardware part of the program */
static GFX_HardwareFuncs _gfx_program_obj_funcs =
{
	_gfx_program_obj_free,
	NULL,
	NULL
};

/******************************************************/
GLint _gfx_program_get_location(

		GFXProgram*     program,
		unsigned short  index)
{
	/* Validate index */
	if(index >= program->properties) return -1;
	struct GFX_Program* internal = (struct GFX_Program*)program;

	return ((struct GFX_Property*)gfx_vector_at(&internal->properties, index))->location;
}

/******************************************************/
GLuint _gfx_program_get_handle(

		const GFXProgram* program)
{
	return ((struct GFX_Program*)program)->handle;
}

/******************************************************/
GFXProgram* gfx_program_create(void)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Create new program */
	struct GFX_Program* prog = calloc(1, sizeof(struct GFX_Program));
	if(!prog) return NULL;

	/* Register as object */
	prog->program.id = _gfx_hardware_object_register(
		prog,
		&_gfx_program_obj_funcs
	);

	if(!prog->program.id)
	{
		free(prog);
		return NULL;
	}

	/* Create OpenGL program */
	prog->handle = window->extensions.CreateProgram();

	gfx_vector_init(&prog->properties, sizeof(struct GFX_Property));
	gfx_vector_init(&prog->blocks, sizeof(GFXPropertyBlock));

	return (GFXProgram*)prog;
}

/******************************************************/
void gfx_program_free(

		GFXProgram* program)
{
	if(program)
	{
		struct GFX_Program* internal = (struct GFX_Program*)program;

		/* Unregister as object */
		_gfx_hardware_object_unregister(program->id);

		/* Get current window and context */
		GFX_Window* window = _gfx_window_get_current();
		if(window)
		{
			window->extensions.DeleteProgram(internal->handle);
			if(window->extensions.program == internal->handle)
				window->extensions.program = 0;
		}

		/* Unprepare all uniforms */
		_gfx_program_unprepare(internal);

		gfx_vector_clear(&internal->properties);
		gfx_vector_clear(&internal->blocks);

		free(program);
	}
}

/******************************************************/
int gfx_program_set_attribute(

		GFXProgram*   program,
		unsigned int  index,
		const char*   name)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return 0;

	if(index >= window->extensions.limits[GFX_LIM_MAX_VERTEX_ATTRIBS])
		return 0;

	struct GFX_Program* internal = (struct GFX_Program*)program;

	/* Set the attribute */
	window->extensions.BindAttribLocation(
		internal->handle,
		index,
		name
	);

	return 1;
}

/******************************************************/
int gfx_program_set_feedback(

		GFXProgram*      program,
		size_t           num,
		const char**     names,
		GFXFeedbackMode  mode)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return 0;

	if(num > window->extensions.limits[GFX_LIM_MAX_FEEDBACK_BUFFERS] &&
		mode == GFX_FEEDBACK_SEPARATE) return 0;

	struct GFX_Program* internal = (struct GFX_Program*)program;

	/* Specify transform feedback */
	window->extensions.TransformFeedbackVaryings(
		internal->handle,
		num,
		names,
		mode
	);

	return 1;
}

/******************************************************/
int gfx_program_link(

		GFXProgram*  program,
		size_t       num,
		GFXShader**  shaders,
		int          binary)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return 0;

	struct GFX_Program* internal = (struct GFX_Program*)program;

	/* Set binary parameter */
	if(window->extensions.flags[GFX_EXT_PROGRAM_BINARY])
	{
		window->extensions.ProgramParameteri(
			internal->handle,
			GL_PROGRAM_BINARY_RETRIEVABLE_HINT,
			binary ? GL_TRUE : GL_FALSE
		);
	}

	/* Compile and attach all shaders */
	size_t i = 0;
	for(i = 0; i < num; ++i)
	{
		/* Uh oh, compiling went wrong, detach all! */
		if(!gfx_shader_compile(shaders[i]))
		{
			while(i) window->extensions.DetachShader(
				internal->handle,
				_gfx_shader_get_handle(shaders[--i])
			);
			return 0;
		}

		/* Attach shader */
		window->extensions.AttachShader(
			internal->handle,
			_gfx_shader_get_handle(shaders[i])
		);
	}

	/* Try to link */
	GLint status;
	window->extensions.LinkProgram(internal->handle);
	window->extensions.GetProgramiv(
		internal->handle,
		GL_LINK_STATUS, &status
	);

	/* Detach all shaders */
	for(i = 0; i < num; ++i) window->extensions.DetachShader(
		internal->handle,
		_gfx_shader_get_handle(shaders[i])
	);

	if(!status)
	{
		/* Generate error */
		GLint len;
		window->extensions.GetProgramiv(
			internal->handle,
			GL_INFO_LOG_LENGTH,
			&len
		);

		char buff[len];
		window->extensions.GetProgramInfoLog(
			internal->handle,
			len,
			NULL,
			buff
		);

		gfx_errors_push(GFX_ERROR_LINK_FAIL, buff);
	}

	/* Prepare program */
	else _gfx_program_prepare(
		internal,
		&window->extensions
	);

	return status;
}

/******************************************************/
void* gfx_program_get_binary(

		GFXProgram*        program,
		GFXProgramFormat*  format,
		size_t*            size)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window)
	{
		*size = 0;
		return NULL;
	}

	struct GFX_Program* internal = (struct GFX_Program*)program;

	/* Get data byte size */
	GLint bytes;
	window->extensions.GetProgramiv(
		internal->handle,
		GL_PROGRAM_BINARY_LENGTH,
		&bytes
	);
	*size = bytes;

	if(!bytes) return NULL;

	/* Get the data */
	void* data = malloc(bytes);
	window->extensions.GetProgramBinary(
		internal->handle,
		bytes,
		NULL,
		format,
		data
	);

	return data;
}

/******************************************************/
int gfx_program_set_binary(

		GFXProgram*       program,
		GFXProgramFormat  format,
		size_t            size,
		const void*       data)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return 0;

	struct GFX_Program* internal = (struct GFX_Program*)program;

	/* Set binary representation */
	GLint status;
	window->extensions.ProgramBinary(
		internal->handle,
		format,
		data,
		size);

	window->extensions.GetProgramiv(
		internal->handle,
		GL_LINK_STATUS,
		&status);

	/* Prepare program */
	if(status) _gfx_program_prepare(
		internal,
		&window->extensions
	);

	return status;
}

/******************************************************/
const GFXProperty* gfx_program_get_property(

		GFXProgram*     program,
		unsigned short  index)
{
	/* Validate index */
	if(index >= program->properties) return NULL;
	struct GFX_Program* internal = (struct GFX_Program*)program;

	return gfx_vector_at(&internal->properties, index);
}

/******************************************************/
unsigned short gfx_program_get_named_property(

		GFXProgram*  program,
		const char*  name)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return 0;

	struct GFX_Program* internal = (struct GFX_Program*)program;

	/* Get index */
	GLuint index;
	window->extensions.GetUniformIndices(
		internal->handle,
		1,
		&name,
		&index
	);

	return (index == GL_INVALID_INDEX) ? program->properties : index;
}

/******************************************************/
const GFXPropertyBlock* gfx_program_get_property_block(

		GFXProgram*     program,
		unsigned short  index)
{
	/* Validate index */
	if(index >= program->blocks) return NULL;
	struct GFX_Program* internal = (struct GFX_Program*)program;

	return gfx_vector_at(&internal->blocks, index);
}

/******************************************************/
unsigned short gfx_program_get_named_property_block(

		GFXProgram*  program,
		const char*  name)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return 0;

	struct GFX_Program* internal = (struct GFX_Program*)program;

	/* Get index */
	GLuint index = window->extensions.GetUniformBlockIndex(
		internal->handle,
		name
	);

	return (index == GL_INVALID_INDEX) ? program->blocks : index;
}
