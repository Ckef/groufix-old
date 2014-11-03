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

#include "groufix/core/errors.h"
#include "groufix/core/renderer.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* GLSL version strings */
static const char* _gfx_glsl_versions[] =
{
	"#version 150\n",
	"#version 300\n",
	"#version 310\n",
	"#version 330\n",
	"#version 400\n",
	"#version 410\n",
	"#version 420\n",
	"#version 430\n",
	"#version 440\n"
};

/* Internal Shader */
struct GFX_Shader
{
	/* Super class */
	GFXShader shader;

	/* Hidden data */
	GFX_RenderObjectID  id;
	GLuint              handle; /* OpenGL handle */
};

/******************************************************/
static int _gfx_shader_eval_stage(

		GFXShaderStage stage,
		GFX_WIND_ARG)
{
	switch(stage)
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

		/* Herp derp */
		case GFX_ALL_SHADERS :
			return 0;

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
static int _gfx_shader_parse(

		GFXShaderStage  stage,
		size_t*         count,
		char***         strings,
		GLint**         lengths,
		GFX_WIND_ARG)
{
	/* Eeeeh? */
	if(!*count) return 0;

	/* Check whether outputs need to be redefined */
	unsigned char hasOut =
		GFX_WIND_GET.ext[GFX_EXT_PROGRAM_MAP] &&
		stage == GFX_VERTEX_SHADER;

	/* First attempt to find version and/or per vertex built-ins */
	size_t ver;
	size_t out = 0;
	const char* verStr = NULL;

	for(ver = 0; ver < *count; ++ver)
	{
		verStr = strstr((*strings)[ver], "#version");
		if(verStr) break;
	}

	if(hasOut) for(out = 0; out < *count; ++out)
	{
		if(strstr((*strings)[out], "gl_PerVertex"))
			break;
	}

	/* Calculate number of strings to insert */
	size_t strs = (out >= *count) ? 2 : (!verStr ? 1 : 0);

	/* Done. */
	if(!strs) return 1;

	/* Get version string */
	if(!verStr)
	{
		verStr = _gfx_shader_eval_glsl(
			GFX_WIND_GET.version.major,
			GFX_WIND_GET.version.minor
		);

		if(!verStr) return 0;
	}

	/* Allocate more space */
	size_t newCount = *count + strs;
	char** newStrings = malloc(sizeof(char*) * newCount);
	GLint* newLengths = malloc(sizeof(GLint) * newCount);

	if(!newStrings || !newLengths)
	{
		free(newStrings);
		free(newLengths);

		return 0;
	}

	/* Copy all data */
	memcpy(newStrings + strs, *strings, sizeof(char*) * (*count));
	memcpy(newLengths + strs, *lengths, sizeof(GLint) * (*count));

	if(ver >= *count)
	{
		/* Insert new version string */
		newLengths[0] = -1;
		newStrings[0] = (char*)verStr;
	}
	if(out >= *count)
	{
		/* Extract the version string */
		if(ver < *count)
		{
			/* Move everything up to version back to start */
			memmove(
				newStrings,
				newStrings + strs,
				sizeof(char*) * (ver + 1));

			memmove(
				newLengths,
				newLengths + strs,
				sizeof(GLint) * (ver + 1));

			/* Move everything after version to new position */
			size_t spn =
				strcspn(verStr, "\n");
			size_t len =
				GFX_PTR_DIFF(newStrings[ver], verStr) / sizeof(char) +
				spn + 1;

			newStrings[ver + 2] = newStrings[ver] + len;
			newLengths[ver + 2] = newLengths[ver];
			newLengths[ver + 2] -= (verStr[spn] == '\0') ?
				newLengths[ver + 2] :
				(newLengths[ver + 2] >= 0 ? len : 0);

			newLengths[ver] = len;
			out = ver + 1;
		}
		else out = 1;

		/* Insert per vertex output */
		newLengths[out] = -1;
		newStrings[out] =
			"out gl_PerVertex{"
			"vec4 gl_Position;"
			"float gl_PointSize;"
			"float gl_ClipDistance[];"
			"};"
			"\n";
	}

	/* Copy new data */
	free(*strings);
	free(*lengths);

	*count = newCount;
	*strings = newStrings;
	*lengths = newLengths;

	return 1;
}


/******************************************************/
static void _gfx_shader_obj_free(

		void*               object,
		GFX_RenderObjectID  id)
{
	struct GFX_Shader* shader = (struct GFX_Shader*)object;

	shader->id = id;
	shader->shader.compiled = 0;
	shader->handle = 0;
}

/******************************************************/
static void _gfx_shader_obj_save_restore(

		void*               object,
		GFX_RenderObjectID  id)
{
	struct GFX_Shader* shader = (struct GFX_Shader*)object;
	shader->id = id;
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

		GFXShaderStage stage)
{
	GFX_WIND_INIT(NULL);

	if(!_gfx_shader_eval_stage(stage, GFX_WIND_AS_ARG))
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
	shader->id = _gfx_render_object_register(
		&GFX_WIND_GET.objects,
		shader,
		&_gfx_shader_obj_funcs
	);

	if(!shader->id.id)
	{
		free(shader);
		return NULL;
	}

	/* Create OpenGL resources */
	shader->shader.stage = stage;
	shader->handle = GFX_REND_GET.CreateShader(stage);

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

		/* Unregister as object */
		_gfx_render_object_unregister(internal->id);

		/* Delete shader */
		if(!GFX_WIND_EQ(NULL))
			GFX_REND_GET.DeleteShader(internal->handle);

		free(shader);
	}
}

/******************************************************/
int gfx_shader_set_source(

		GFXShader*    shader,
		size_t        num,
		const char**  src,
		const int*    lens)
{
	GFX_WIND_INIT(0);

	/* Allocate new arrays */
	char** strings = malloc(sizeof(char*) * num);
	GLint* lengths = malloc(sizeof(GLint) * num);

	if(strings && lengths)
	{
		/* Fill with data */
		memcpy(strings, src, sizeof(char*) * num);

		size_t n;
		if(lens) for(n = 0; n < num; ++n)
			lengths[n] = lens[n];

		else for(n = 0; n < num; ++n)
			lengths[n] = -1;

		/* Parse source */
		if(_gfx_shader_parse(
			shader->stage,
			&num,
			&strings,
			&lengths,
			GFX_WIND_AS_ARG))
		{
			/* Set source */
			GFX_REND_GET.ShaderSource(
				((struct GFX_Shader*)shader)->handle,
				num,
				(const GLchar**)strings,
				(const GLint*)lengths
			);

			shader->compiled = 0;

			free(strings);
			free(lengths);

			return 1;
		}
	}

	/* Failed */
	free(strings);
	free(lengths);

	return 0;
}

/******************************************************/
char* gfx_shader_get_source(

		GFXShader*  shader,
		size_t*     length)
{
	GFX_WIND_INIT((*length = 0, NULL));

	struct GFX_Shader* internal = (struct GFX_Shader*)shader;

	/* Get source length */
	GLint len;
	GFX_REND_GET.GetShaderiv(
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
