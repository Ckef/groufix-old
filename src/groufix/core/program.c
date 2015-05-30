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

#include "groufix/core/internal.h"

#include <limits.h>
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
/* Internal Program */
typedef struct GFX_Program
{
	/* Super class */
	GFXProgram program;

	/* Hidden data */
	GFX_RenderObjectID  id;
	unsigned int        references; /* Reference counter */
	GLuint              handle;     /* OpenGL handle */
	GFXVector           properties; /* Stores GFX_Property */
	GFXVector           blocks;     /* Stores GFXPropertyBlock */

} GFX_Program;


/* Internal Property */
typedef struct GFX_Property
{
	GFXProperty  property; /* Super class */
	GLint        location;

} GFX_Property;


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

		GFX_Program*    program,
		unsigned short  properties,
		GFX_CONT_ARG)
{
	/* Reserve memory */
	gfx_vector_clear(&program->properties);
	if(!gfx_vector_reserve(&program->properties, properties))
		return 0;

	/* Retrieve uniforms */
	GLuint indices[properties];
	GLint lens[properties];

	unsigned short i;
	for(i = 0; i < properties; ++i) indices[i] = i;
	GFX_REND_GET.GetActiveUniformsiv(
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

		GFX_REND_GET.GetActiveUniform(
			program->handle,
			i,
			lens[i],
			NULL,
			&size,
			&type,
			name);

		GLint loc = GFX_REND_GET.GetUniformLocation(
			program->handle,
			name);

		/* Create and insert */
		GFX_Property prop;

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

		GFX_Program*    program,
		unsigned short  blocks,
		GFX_CONT_ARG)
{
	/* Reserve memory */
	gfx_vector_clear(&program->blocks);
	if(!gfx_vector_reserve(&program->blocks, blocks)) return 0;

	/* Retrieve uniform blocks */
	unsigned short k;
	for(k = 0; k < blocks; ++k)
	{
		GLint size;
		GFX_REND_GET.GetActiveUniformBlockiv(
			program->handle,
			k,
			GL_UNIFORM_BLOCK_DATA_SIZE,
			&size
		);

		/* Get associated uniform indices */
		GLint uniforms;
		GFX_REND_GET.GetActiveUniformBlockiv(
			program->handle,
			k,
			GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,
			&uniforms
		);

		GLint uniformIndices[uniforms];
		GFX_REND_GET.GetActiveUniformBlockiv(
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

		GFX_REND_GET.GetActiveUniformsiv(
			program->handle,
			uniforms,
			indices,
			GL_UNIFORM_OFFSET,
			offsets);

		GFX_REND_GET.GetActiveUniformsiv(
			program->handle,
			uniforms,
			indices,
			GL_UNIFORM_ARRAY_STRIDE,
			arrStrides);

		GFX_REND_GET.GetActiveUniformsiv(
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

		GFX_Program* program)
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

		GFX_Program* program,
		GFX_CONT_ARG)
{
	_gfx_program_unprepare(program);

	/* Get number of properties */
	GLint properties;
	GLint blocks;

	GFX_REND_GET.GetProgramiv(program->handle, GL_ACTIVE_UNIFORMS, &properties);
	GFX_REND_GET.GetProgramiv(program->handle, GL_ACTIVE_UNIFORM_BLOCKS, &blocks);

	properties = properties < 0 ? 0 : properties;
	blocks = blocks < 0 ? 0 : blocks;

	/* Prepare properties and blocks */
	program->program.properties =
		_gfx_program_prepare_properties(program, properties, GFX_CONT_AS_ARG);
	program->program.blocks =
		_gfx_program_prepare_blocks(program, blocks, GFX_CONT_AS_ARG);
}

/******************************************************/
static void _gfx_program_obj_free(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_Program* program = (GFX_Program*)object;

	program->id = id;
	program->program.linked = 0;
	program->handle = 0;
}

/******************************************************/
static void _gfx_program_obj_save_restore(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_Program* program = (GFX_Program*)object;
	program->id = id;
}

/******************************************************/
/* vtable for render object part of the program */
static GFX_RenderObjectFuncs _gfx_program_obj_funcs =
{
	_gfx_program_obj_free,
	_gfx_program_obj_save_restore,
	_gfx_program_obj_save_restore
};

/******************************************************/
GLuint _gfx_gl_program_get_handle(

		const GFXProgram* program)
{
	return ((const GFX_Program*)program)->handle;
}

/******************************************************/
GLint _gfx_gl_program_get_location(

		const GFXProgram*  program,
		unsigned short     index)
{
	/* Validate index */
	if(index >= program->properties) return -1;
	const GFX_Program* internal = (const GFX_Program*)program;

	return ((GFX_Property*)gfx_vector_at(&internal->properties, index))->location;
}

/******************************************************/
int _gfx_program_reference(

		GFXProgram*   program,
		unsigned int  references)
{
	GFX_Program* internal = (GFX_Program*)program;

	if(UINT_MAX - references < internal->references)
	{
		/* Overflow error */
		gfx_errors_push(
			GFX_ERROR_OVERFLOW,
			"Overflow occurred during Program referencing."
		);
		return 0;
	}

	internal->references += references;
	return 1;
}

/******************************************************/
GFXProgram* gfx_program_create(

		size_t instances)
{
	GFX_CONT_INIT(NULL);

	/* Create new program */
	GFX_Program* prog = calloc(1, sizeof(GFX_Program));
	if(!prog)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Program could not be allocated."
		);
		return NULL;
	}

	/* Register as object */
	prog->id = _gfx_render_object_register(
		&GFX_CONT_GET.objects,
		prog,
		&_gfx_program_obj_funcs
	);

	if(!prog->id.id)
	{
		free(prog);
		return NULL;
	}

	/* Initialize */
	prog->references = 1;
	prog->program.instances = instances;
	prog->handle = GFX_REND_GET.CreateProgram();

	gfx_vector_init(&prog->properties, sizeof(GFX_Property));
	gfx_vector_init(&prog->blocks, sizeof(GFXPropertyBlock));

	return (GFXProgram*)prog;
}

/******************************************************/
void gfx_program_free(

		GFXProgram* program)
{
	if(program)
	{
		GFX_Program* internal = (GFX_Program*)program;

		/* Check references */
		if(!(--internal->references))
		{
			GFX_CONT_INIT_UNSAFE;

			/* Unregister as object */
			_gfx_render_object_unregister(internal->id);

			if(!GFX_CONT_EQ(NULL))
			{
				/* Delete program */
				if(GFX_REND_GET.program == internal->handle)
					GFX_REND_GET.program = 0;

				GFX_REND_GET.DeleteProgram(internal->handle);
			}

			/* Clear resources */
			_gfx_program_unprepare(internal);

			gfx_vector_clear(&internal->properties);
			gfx_vector_clear(&internal->blocks);

			free(program);
		}
	}
}

/******************************************************/
int gfx_program_set_attribute(

		GFXProgram*   program,
		unsigned int  index,
		const char*   name)
{
	GFX_CONT_INIT(0);

	if(index >= GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_ATTRIBS])
		return 0;

	GFX_Program* internal = (GFX_Program*)program;

	/* Set the attribute */
	GFX_REND_GET.BindAttribLocation(
		internal->handle,
		index,
		name
	);

	program->linked = 0;

	return 1;
}

/******************************************************/
int gfx_program_set_feedback(

		GFXProgram*      program,
		size_t           num,
		const char**     names,
		GFXFeedbackMode  mode)
{
	GFX_CONT_INIT(0);

	if(num > GFX_CONT_GET.lim[GFX_LIM_MAX_FEEDBACK_BUFFERS] &&
		mode == GFX_FEEDBACK_SEPARATE) return 0;

	GFX_Program* internal = (GFX_Program*)program;

	/* Specify transform feedback */
	GFX_REND_GET.TransformFeedbackVaryings(
		internal->handle,
		num,
		names,
		mode
	);

	program->linked = 0;

	return 1;
}

/******************************************************/
int gfx_program_link(

		GFXProgram*  program,
		size_t       num,
		GFXShader**  shaders,
		int          binary)
{
	/* Already linked */
	if(!program->linked)
	{
		GFX_CONT_INIT(0);

		GFX_Program* internal = (GFX_Program*)program;

		/* Set binary parameter */
		if(GFX_CONT_GET.ext[GFX_EXT_PROGRAM_BINARY])
			GFX_REND_GET.ProgramParameteri(
				internal->handle,
				GL_PROGRAM_BINARY_RETRIEVABLE_HINT,
				binary ? GL_TRUE : GL_FALSE
			);

		/* Set separable parameter */
		if(GFX_CONT_GET.ext[GFX_EXT_PROGRAM_MAP])
			GFX_REND_GET.ProgramParameteri(
				internal->handle,
				GL_PROGRAM_SEPARABLE,
				GL_TRUE
			);

		/* Compile and attach all shaders */
		size_t i = 0;
		for(i = 0; i < num; ++i)
		{
			/* Uh oh, compiling went wrong, detach all! */
			if(!gfx_shader_compile(shaders[i]))
			{
				while(i) GFX_REND_GET.DetachShader(
					internal->handle,
					_gfx_gl_shader_get_handle(shaders[--i])
				);
				return 0;
			}

			/* Attach shader */
			GFX_REND_GET.AttachShader(
				internal->handle,
				_gfx_gl_shader_get_handle(shaders[i])
			);
		}

		/* Try to link */
		GLint status;
		GFX_REND_GET.LinkProgram(internal->handle);
		GFX_REND_GET.GetProgramiv(
			internal->handle,
			GL_LINK_STATUS, &status
		);

		/* Detach all shaders */
		for(i = 0; i < num; ++i) GFX_REND_GET.DetachShader(
			internal->handle,
			_gfx_gl_shader_get_handle(shaders[i])
		);

		if(!status)
		{
			/* Generate error */
			GLint len;
			GFX_REND_GET.GetProgramiv(
				internal->handle,
				GL_INFO_LOG_LENGTH,
				&len
			);

			char buff[len];
			GFX_REND_GET.GetProgramInfoLog(
				internal->handle,
				len,
				NULL,
				buff
			);

			gfx_errors_push(GFX_ERROR_LINK_FAIL, buff);

			return 0;
		}

		/* Prepare program */
		_gfx_program_prepare(internal, GFX_CONT_AS_ARG);
	}

	/* Woop woop! */
	return program->linked = 1;
}

/******************************************************/
void* gfx_program_get_binary(

		const GFXProgram*  program,
		GFXProgramFormat*  format,
		size_t*            size)
{
	GFX_CONT_INIT((*size = 0, NULL));

	const GFX_Program* internal = (const GFX_Program*)program;

	/* Get data byte size */
	GLint bytes;
	GFX_REND_GET.GetProgramiv(
		internal->handle,
		GL_PROGRAM_BINARY_LENGTH,
		&bytes
	);
	*size = bytes;

	if(!bytes) return NULL;

	/* Get the data */
	void* data = malloc(bytes);
	GFX_REND_GET.GetProgramBinary(
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
	GFX_CONT_INIT(0);

	GFX_Program* internal = (GFX_Program*)program;

	/* Set binary representation */
	GLint status;
	GFX_REND_GET.ProgramBinary(
		internal->handle,
		format,
		data,
		size);

	GFX_REND_GET.GetProgramiv(
		internal->handle,
		GL_LINK_STATUS,
		&status);

	/* Prepare program */
	if(status)
	{
		_gfx_program_prepare(internal, GFX_CONT_AS_ARG);
		program->linked = 1;
	}

	return status;
}

/******************************************************/
const GFXProperty* gfx_program_get_property(

		const GFXProgram*  program,
		unsigned short     index)
{
	/* Validate index */
	if(index >= program->properties) return NULL;
	const GFX_Program* internal = (const GFX_Program*)program;

	return gfx_vector_at(&internal->properties, index);
}

/******************************************************/
unsigned short gfx_program_get_named_property(

		const GFXProgram*  program,
		const char*        name)
{
	GFX_CONT_INIT(0);

	const GFX_Program* internal = (const GFX_Program*)program;

	/* Get index */
	GLuint index;
	GFX_REND_GET.GetUniformIndices(
		internal->handle,
		1,
		&name,
		&index
	);

	return (index == GL_INVALID_INDEX) ? program->properties : index;
}

/******************************************************/
const GFXPropertyBlock* gfx_program_get_property_block(

		const GFXProgram*  program,
		unsigned short     index)
{
	/* Validate index */
	if(index >= program->blocks) return NULL;
	const GFX_Program* internal = (const GFX_Program*)program;

	return gfx_vector_at(&internal->blocks, index);
}

/******************************************************/
unsigned short gfx_program_get_named_property_block(

		const GFXProgram*  program,
		const char*        name)
{
	GFX_CONT_INIT(0);

	const GFX_Program* internal = (const GFX_Program*)program;

	/* Get index */
	GLuint index = GFX_REND_GET.GetUniformBlockIndex(
		internal->handle,
		name
	);

	return (index == GL_INVALID_INDEX) ? program->blocks : index;
}
