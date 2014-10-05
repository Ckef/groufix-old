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
	GFXShader  shader; /* Super class */
	GLuint     handle; /* OpenGL handle */
};

/******************************************************/
static int _gfx_shader_eval_type(

		GFXShaderType type,
		GFX_WIND_ARG)
{
	switch(type)
	{
		/* GFX_EXT_TESSELLATION_SHADER */
		case GFX_TESS_CONTROL_SHADER :
		case GFX_TESS_EVAL_SHADER :

			if(!GFX_WIND_GET.ext[GFX_EXT_TESSELLATION_SHADER])
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

			if(!GFX_WIND_GET.ext[GFX_EXT_GEOMETRY_SHADER])
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
		const char***  src,
		GFX_WIND_ARG)
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
			GFX_WIND_GET.context.major,
			GFX_WIND_GET.context.minor
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

		void*         object,
		unsigned int  id)
{
	struct GFX_Shader* shader = (struct GFX_Shader*)object;

	shader->shader.id = id;
	shader->handle = 0;
	shader->shader.compiled = 0;
}

/******************************************************/
static void _gfx_shader_obj_save_restore(

		void*         object,
		unsigned int  id)
{
	struct GFX_Shader* shader = (struct GFX_Shader*)object;
	shader->shader.id = id;
}

/******************************************************/
/* vtable for render object part of the shader */
static GFX_RenderObjectFuncs _gfx_shader_obj_funcs =
{
	_gfx_shader_obj_free,
	_gfx_shader_obj_save_restore,
	_gfx_shader_obj_save_restore
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
	GFX_WIND_INIT(NULL);

	if(!_gfx_shader_eval_type(type, GFX_WIND_AS_ARG))
		return NULL;

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
	shader->shader.id = _gfx_render_object_register(
		&GFX_WIND_GET.objects,
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
	shader->handle = GFX_REND_GET.CreateShader(type);

	return (GFXShader*)shader;
}

/******************************************************/
void gfx_shader_free(

		GFXShader* shader)
{
	if(shader)
	{
		GFX_WIND_INIT_UNSAFE;

		struct GFX_Shader* internal = (struct GFX_Shader*)shader;

		if(!GFX_WIND_EQ(NULL))
		{
			GFX_REND_GET.DeleteShader(internal->handle);

			/* Unregister as object */
			_gfx_render_object_unregister(
				&GFX_WIND_GET.objects,
				shader->id
			);
		}

		free(shader);
	}
}

/******************************************************/
int gfx_shader_set_source(

		GFXShader*    shader,
		size_t        num,
		const char**  src)
{
	GFX_WIND_INIT(0);

	if(!num) return 0;
	struct GFX_Shader* internal = (struct GFX_Shader*)shader;

	/* Copy sources for preprocessing */
	const char** newSrc = malloc(sizeof(char*) * num);
	if(!newSrc) return 0;

	memcpy(newSrc, src, sizeof(char*) * num);

	/* Preprocess */
	int success = _gfx_shader_preprocess_version(
		&num,
		&newSrc,
		GFX_WIND_AS_ARG
	);

	if(success)
	{
		/* Set the source */
		GFX_REND_GET.ShaderSource(internal->handle, num, newSrc, NULL);
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
	if(length) *length = 0;
	GFX_WIND_INIT(NULL);

	struct GFX_Shader* internal = (struct GFX_Shader*)shader;

	/* Get source length */
	GLint len;
	GFX_REND_GET.GetShaderiv(
		internal->handle,
		GL_SHADER_SOURCE_LENGTH,
		&len
	);

	if(!len) return NULL;

	/* Get actual source */
	char* buff = malloc(len);
	GFX_REND_GET.GetShaderSource(
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
		GFX_WIND_INIT(0);

		struct GFX_Shader* internal = (struct GFX_Shader*)shader;

		/* Try to compile */
		GLint status;
		GFX_REND_GET.CompileShader(internal->handle);
		GFX_REND_GET.GetShaderiv(
			internal->handle,
			GL_COMPILE_STATUS,
			&status
		);

		/* Generate error */
		if(!status)
		{
			GLint len;
			GFX_REND_GET.GetShaderiv(
				internal->handle,
				GL_INFO_LOG_LENGTH,
				&len);

			char buff[len];
			GFX_REND_GET.GetShaderInfoLog(
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
