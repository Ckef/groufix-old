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
	unsigned int  id;     /* Render Object ID */
	GLuint        handle; /* OpenGL handle */
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
static void _gfx_shader_obj_free(

		void*         object,
		unsigned int  id)
{
	struct GFX_Shader* shader = (struct GFX_Shader*)object;

	/* If it was already freed, free memory */
	if(!shader->handle)
		free(shader);

	else
	{
		shader->id = 0;
		shader->shader.compiled = 0;
		shader->handle = 0;
	}
}

/******************************************************/
static void _gfx_shader_obj_save_restore(

		void*         object,
		unsigned int  id)
{
	struct GFX_Shader* shader = (struct GFX_Shader*)object;
	if(!shader->id) return;

	shader->id = id ? id : shader->id;
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

	if(!shader->id)
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
		internal->id = 0;

		/* If it was already freed as render object, free memory */
		if(!internal->handle)
			free(shader);

		else
		{
			if(!GFX_WIND_EQ(NULL))
				GFX_REND_GET.DeleteShader(internal->handle);

			internal->handle = 0;
		}
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

	/* Fetch version string */
	const char* glsl = _gfx_shader_eval_glsl(
		GFX_WIND_GET.version.major,
		GFX_WIND_GET.version.minor
	);

	if(!glsl) return 0;

	/* Check whether outputs need to be redefined */
	unsigned char out =
		GFX_WIND_GET.ext[GFX_EXT_PROGRAM_MAP] &&
		shader->stage == GFX_VERTEX_SHADER;

	size_t new = out ? 2 : 1;

	/* Copy sources for some preprocessing */
	const char** newSrc = malloc(sizeof(char*) * (num + new));
	if(!newSrc) return 0;

	memcpy(newSrc + new, src, sizeof(char*) * num);
	newSrc[0] = glsl;

	/* Insert vertex output */
	if(out) newSrc[1] =
		"out gl_PerVertex{"
		"vec4 gl_Position;"
		"float gl_PointSize;"
		"float gl_ClipDistance[];"
		"};"
		"\n\0";

	/* Set the actual source */
	GFX_REND_GET.ShaderSource(
		((struct GFX_Shader*)shader)->handle,
		num + new,
		newSrc,
		NULL
	);

	shader->compiled = 0;

	/* Deallocate sources */
	free(newSrc);

	return 1;
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
