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
	"#version 440\n",
	"#version 450\n"
};


/* Internal Shader */
typedef struct GFX_Shader
{
	/* Super class */
	GFXShader shader;

	/* Hidden data */
	GFX_RenderObjectID  id;
	GLuint              handle; /* OpenGL handle */

} GFX_Shader;


/******************************************************/
static int _gfx_shader_eval_stage(

		GFXShaderStage stage,
		GFX_CONT_ARG)
{
	switch(stage)
	{
		/* GFX_EXT_TESSELLATION_SHADER */
		case GFX_TESS_CONTROL_SHADER :
		case GFX_TESS_EVAL_SHADER :

			if(!GFX_CONT_GET.ext[GFX_EXT_TESSELLATION_SHADER])
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

			if(!GFX_CONT_GET.ext[GFX_EXT_GEOMETRY_SHADER])
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
			case 5 : return _gfx_glsl_versions[9];
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
static const char* _gfx_shader_find(

		const char*  haystack,
		const char*  needle,
		size_t       len)
{
	size_t s;
	for(s = len; len; s = --len)
	{
		/* Test current position */
		const char* h = haystack;
		const char* n = needle;

		while(s-- && *n)
			if(*(n++) != *(h++)) break;

		if(!(*n)) return haystack;

		/* Next position */
		++haystack;
	}

	return NULL;
}

/******************************************************/
static char* _gfx_shader_parse(

		GFXShaderStage  stage,
		size_t          num,
		const char**    src,
		const int*      lens,
		GLint*          len,
		GFX_CONT_ARG)
{
	/* Say what now? */
	if(!num) return NULL;

	/* Get all lengths and total length */
	size_t lengths[num];
	size_t total = 0;

	size_t i;
	for(i = 0; i < num; ++i)
	{
		if(lens && lens[i] >= 0) lengths[i] = lens[i];
		else lengths[i] = strlen(src[i]);

		total += lengths[i];
	}

	/* Attempt to find version */
	const char* verStr = NULL;
	size_t ver = 0;

	for(ver = 0; ver < num; ++ver)
		if((verStr = _gfx_shader_find(src[ver], "#version", lengths[ver])))
			break;

	/* Get version string */
	if(!verStr)
	{
		verStr = _gfx_shader_eval_glsl(
			GFX_CONT_GET.version.major,
			GFX_CONT_GET.version.minor
		);

		if(!verStr) return 0;
	}

	/* Reconstruct the source */
	/* We do this by simply copying the right sources in the right order */
	/* Also check whether outputs need to be redefined */
	unsigned char hasOut =
		GFX_CONT_GET.ext[GFX_EXT_PROGRAM_MAP] &&
		stage == GFX_VERTEX_SHADER;

	const char* outStr =
		"out vec4 gl_Position;";

	/* Firstly recalculate total length */
	if(ver >= num) total += strlen(verStr);
	if(hasOut) total += strlen(outStr);

	char* shader = malloc(total);
	if(!shader) return NULL;

	*len = total;

	/* Keep track of where we're copying */
	i = 0;
	size_t offset = 0;
	total = 0;

	if(ver >= num)
	{
		/* Copy new version */
		size_t l = strlen(verStr);
		memcpy(shader + total, verStr, l);
		total += l;
	}
	else
	{
		/* Copy up until existing version */
		while(i < ver)
		{
			offset = lengths[i];
			memcpy(shader + total, src[i], offset);
			total += offset;

			++i;
		}

		/* Copy existing version */
		offset = GFX_PTR_DIFF(src[i], verStr);
		while(offset < lengths[i]) if(src[i][offset++] == '\n') break;

		memcpy(shader + total, src[i], offset);
		total += offset;
	}

	if(hasOut)
	{
		/* Copy outputs */
		size_t l = strlen(outStr);
		memcpy(shader + total, outStr, l);
		total += l;
	}

	if(offset < lengths[i])
	{
		/* Copy remaining parts after existing version */
		size_t l = lengths[i] - offset;
		memcpy(shader + total, src[i] + offset, l);
		total += l;
	}

	/* Copy remaining sources */
	while(++i < num)
	{
		offset = lengths[i];
		memcpy(shader + total, src[i], offset);
		total += offset;
	}

	return shader;
}

/******************************************************/
static void _gfx_shader_obj_free(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_Shader* shader = (GFX_Shader*)object;

	shader->id = id;
	shader->shader.compiled = 0;
	shader->handle = 0;
}

/******************************************************/
static void _gfx_shader_obj_save_restore(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_Shader* shader = (GFX_Shader*)object;
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
	return ((const GFX_Shader*)shader)->handle;
}

/******************************************************/
GFXShader* gfx_shader_create(

		GFXShaderStage stage)
{
	GFX_CONT_INIT(NULL);

	if(!_gfx_shader_eval_stage(stage, GFX_CONT_AS_ARG))
		return NULL;

	/* Create new shader */
	GFX_Shader* shader = calloc(1, sizeof(GFX_Shader));
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
		&GFX_CONT_GET.objects,
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
		GFX_CONT_INIT_UNSAFE;

		GFX_Shader* internal = (GFX_Shader*)shader;

		/* Unregister as object */
		_gfx_render_object_unregister(internal->id);

		/* Delete shader */
		if(!GFX_CONT_EQ(NULL))
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
	GFX_CONT_INIT(0);

	/* Parse source */
	GLint len;
	char* source = _gfx_shader_parse(
		shader->stage,
		num,
		src,
		lens,
		&len,
		GFX_CONT_AS_ARG
	);

	if(!source) return 0;

	/* Set source */
	GFX_REND_GET.ShaderSource(
		((GFX_Shader*)shader)->handle,
		1,
		(const char**)&source,
		(const GLint*)&len
	);

	shader->compiled = 0;
	free(source);

	return 1;
}

/******************************************************/
char* gfx_shader_get_source(

		const GFXShader*  shader,
		size_t*           length)
{
	GFX_CONT_INIT((*length = 0, NULL));

	const GFX_Shader* internal = (const GFX_Shader*)shader;

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
		GFX_CONT_INIT(0);

		GFX_Shader* internal = (GFX_Shader*)shader;

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
