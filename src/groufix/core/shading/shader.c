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
#include "groufix/core/renderer.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* GLSL version strings */
static const char* _gfx_glsl_versions[] =
{
	"#version 150\n\0",
	"#version 300\n\0",
	"#version 310\n\0",
	"#version 330\n\0",
	"#version 400\n\0",
	"#version 410\n\0",
	"#version 420\n\0",
	"#version 430\n\0",
	"#version 440\n\0"
};

/* Internal Shader */
struct GFX_Shader
{
	/* Super class */
	GFXShader shader;

	/* Hidden data */
	GLuint handle; /* OpenGL handle */
};

/******************************************************/
static int _gfx_shader_eval_type(

		GFXShaderType type)
{
	switch(type)
	{
		/* GFX_EXT_TESSELLATION_SHADER */
		case GFX_TESS_CONTROL_SHADER :
		case GFX_TESS_EVAL_SHADER :

			if(!(GFX_WND)->flags[GFX_EXT_TESSELLATION_SHADER])
			{
				gfx_errors_push(
					GFX_ERROR_INCOMPATIBLE_CONTEXT,
					"GFX_EXT_TESSELLATION_SHADER is incompatible with this context."
				);
				return 0;
			}
			return 1;

		/* GFX_EXT_GEOMETRY_SHADER */
		case GFX_GEOMETRY_SHADER :

			if(!(GFX_WND)->flags[GFX_EXT_GEOMETRY_SHADER])
			{
				gfx_errors_push(
					GFX_ERROR_INCOMPATIBLE_CONTEXT,
					"GFX_EXT_GEOMETRY_SHADER is incompatible with this context."
				);
				return 0;
			}
			return 1;

		/* Everything else */
		default : return 1;
	}
}

/******************************************************/
static inline const char* _gfx_shader_get_glsl(

		int  major,
		int  minor)
{
#if defined(GFX_GLES)

	/* GLES context */
	switch(major)
	{
		case 3 : switch(minor)
		{
			case 0 : return _gfx_glsl_versions[1];
			case 1 : return _gfx_glsl_versions[2];
		}

		default : return NULL;
	}

#elif defined(GFX_GL)

	/* GL context */
	switch(major)
	{
		case 3 : switch(minor)
		{
			case 2 : return _gfx_glsl_versions[0];
			case 3 : return _gfx_glsl_versions[3];
			default : return NULL;
		}
		case 4 : switch(minor)
		{
			case 0 : return _gfx_glsl_versions[4];
			case 1 : return _gfx_glsl_versions[5];
			case 2 : return _gfx_glsl_versions[6];
			case 3 : return _gfx_glsl_versions[7];
			case 4 : return _gfx_glsl_versions[8];
			default : return NULL;
		}

		default : return NULL;
	}

#endif
}

/******************************************************/
static const char* _gfx_shader_eval_glsl(

		int  major,
		int  minor)
{
	const char* glsl = _gfx_shader_get_glsl(major, minor);
	if(!glsl)
	{
		gfx_errors_push(
			GFX_ERROR_PLATFORM_ERROR,
			"GLSL version could not be resolved for a shader."
		);
	}

	return glsl;
}

/******************************************************/
static int _gfx_shader_preprocess_version(

		size_t*        num,
		const char***  src)
{
	size_t newNum = *num;
	const char** newSrc = *src;

	/* Search for #version preprocessor */
	const char* vers = "#version";
	size_t n;

	for(n = 0; n < newNum; ++n)
		if(strstr(newSrc[n], vers)) break;

	/* Prepend it if not found */
	if(n >= newNum)
	{
		/* Fetch version string */
		const char* glsl = _gfx_shader_eval_glsl(
			(GFX_WND)->context.major,
			(GFX_WND)->context.minor
		);

		if(!glsl) return 0;

		/* Allocate new sources */
		newSrc = malloc(sizeof(char*) * ++newNum);
		if(!newSrc) return 0;

		memcpy(newSrc + 1, *src, sizeof(char*) * (*num));
		newSrc[0] = glsl;

		/* Free original */
		free(*src);
	}

	*num = newNum;
	*src = newSrc;

	return 1;
}

/******************************************************/
static void _gfx_shader_obj_free(

		void* object)
{
	struct GFX_Shader* shader = (struct GFX_Shader*)object;

	shader->handle = 0;
	shader->shader.compiled = 0;
	shader->shader.id = 0;
}

/******************************************************/
/* vtable for hardware part of the shader */
static GFX_HardwareFuncs _gfx_shader_obj_funcs =
{
	_gfx_shader_obj_free,
	NULL,
	NULL
};

/******************************************************/
GLuint _gfx_shader_get_handle(

		const GFXShader* shader)
{
	return ((struct GFX_Shader*)shader)->handle;
}

/******************************************************/
GFXShader* gfx_shader_create(

		GFXShaderType type)
{
	if(!GFX_WND) return NULL;
	if(!_gfx_shader_eval_type(type)) return NULL;

	/* Create new shader */
	struct GFX_Shader* shader = calloc(1, sizeof(struct GFX_Shader));
	if(!shader)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Shader could not be allocated."
		);
		return NULL;
	}

	/* Register as object */
	shader->shader.id = _gfx_hardware_object_register(
		shader,
		&_gfx_shader_obj_funcs
	);

	if(!shader->shader.id)
	{
		free(shader);
		return NULL;
	}

	/* Create OpenGL resources */
	shader->shader.type = type;
	shader->handle = (GFX_RND).CreateShader(type);

	return (GFXShader*)shader;
}

/******************************************************/
void gfx_shader_free(

		GFXShader* shader)
{
	if(shader)
	{
		struct GFX_Shader* internal = (struct GFX_Shader*)shader;

		/* Unregister as object */
		_gfx_hardware_object_unregister(shader->id);

		/* Get current window and context */
		if(GFX_WND) (GFX_RND).DeleteShader(internal->handle);

		free(shader);
	}
}

/******************************************************/
int gfx_shader_set_source(

		GFXShader*    shader,
		size_t        num,
		const char**  src)
{
	if(!GFX_WND || !num) return 0;
	struct GFX_Shader* internal = (struct GFX_Shader*)shader;

	/* Copy sources for preprocessing */
	const char** newSrc = malloc(sizeof(char*) * num);
	if(!newSrc) return 0;

	memcpy(newSrc, src, sizeof(char*) * num);

	/* Preprocess */
	int success;
	success = _gfx_shader_preprocess_version(&num, &newSrc);

	if(success)
	{
		/* Set the source */
		(GFX_RND).ShaderSource(internal->handle, num, newSrc, NULL);
		shader->compiled = 0;
	}

	/* Deallocate sources */
	free(newSrc);

	return success;
}

/******************************************************/
char* gfx_shader_get_source(

		GFXShader*  shader,
		size_t*     length)
{
	if(!GFX_WND)
	{
		if(length) *length = 0;
		return NULL;
	}

	struct GFX_Shader* internal = (struct GFX_Shader*)shader;

	/* Get source length */
	GLint len;
	(GFX_RND).GetShaderiv(
		internal->handle,
		GL_SHADER_SOURCE_LENGTH,
		&len
	);

	if(!len)
	{
		if(length) *length = 0;
		return NULL;
	}

	/* Get actual source */
	char* buff = malloc(len);
	(GFX_RND).GetShaderSource(
		internal->handle,
		len,
		NULL,
		buff
	);

	if(length) *length = len - 1;

	return buff;
}

/******************************************************/
int gfx_shader_compile(

		GFXShader* shader)
{
	/* Already compiled */
	if(!shader->compiled)
	{
		if(!GFX_WND) return 0;
		struct GFX_Shader* internal = (struct GFX_Shader*)shader;

		/* Try to compile */
		GLint status;
		(GFX_RND).CompileShader(internal->handle);
		(GFX_RND).GetShaderiv(
			internal->handle,
			GL_COMPILE_STATUS,
			&status
		);

		/* Generate error */
		if(!status)
		{
			GLint len;
			(GFX_RND).GetShaderiv(
				internal->handle,
				GL_INFO_LOG_LENGTH,
				&len);

			char buff[len];
			(GFX_RND).GetShaderInfoLog(
				internal->handle,
				len,
				NULL,
				buff);

			gfx_errors_push(
				GFX_ERROR_COMPILE_FAIL,
				buff);

			return 0;
		}
	}

	/* Yep, compiled! */
	return shader->compiled = 1;
}
