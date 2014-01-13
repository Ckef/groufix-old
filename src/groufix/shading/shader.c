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

#include "groufix/internal.h"
#include "groufix/errors.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Internal Shader */
struct GFX_Internal_Shader
{
	/* Super class */
	GFXShader shader;

	/* Hidden data */
	GLuint  handle; /* OpenGL handle */
};

/******************************************************/
static int _gfx_shader_eval_type(GFXShaderType type, const GFX_Extensions* ext)
{
	switch(type)
	{
		/* GFX_EXT_TESSELLATION_SHADER */
		case GFX_TESS_CONTROL_SHADER :
		case GFX_TESS_EVAL_SHADER :

			if(!ext->flags[GFX_EXT_TESSELLATION_SHADER])
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

			if(!ext->flags[GFX_EXT_GEOMETRY_SHADER])
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
static const char* _gfx_shader_eval_glsl(int major, int minor)
{
	const char* glsl = _gfx_platform_context_get_glsl(major, minor);
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
static void _gfx_shader_obj_free(void* object, GFX_Extensions* ext)
{
	struct GFX_Internal_Shader* shader = (struct GFX_Internal_Shader*)object;

	ext->DeleteShader(shader->handle);
	shader->handle = 0;
	shader->shader.compiled = 0;

	shader->shader.id = 0;
}

/******************************************************/
/* vtable for hardware part of the shader */
static GFX_Hardware_Funcs _gfx_shader_obj_funcs =
{
	_gfx_shader_obj_free,
	NULL,
	NULL
};

/******************************************************/
GLuint _gfx_shader_get_handle(const GFXShader* shader)
{
	return ((struct GFX_Internal_Shader*)shader)->handle;
}

/******************************************************/
GFXShader* gfx_shader_create(GFXShaderType type)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	if(!_gfx_shader_eval_type(type, &window->extensions)) return NULL;

	/* Create new shader */
	struct GFX_Internal_Shader* shader = calloc(1, sizeof(struct GFX_Internal_Shader));
	if(!shader) return NULL;

	/* Register as object */
	shader->shader.id = _gfx_hardware_object_register(shader, &_gfx_shader_obj_funcs);
	if(!shader->shader.id)
	{
		free(shader);
		return NULL;
	}

	/* Create OpenGL resources */
	shader->shader.type = type;
	shader->handle = window->extensions.CreateShader(type);

	return (GFXShader*)shader;
}

/******************************************************/
void gfx_shader_free(GFXShader* shader)
{
	if(shader)
	{
		struct GFX_Internal_Shader* internal = (struct GFX_Internal_Shader*)shader;

		/* Unregister as object */
		_gfx_hardware_object_unregister(shader->id);

		/* Get current window and context */
		GFX_Internal_Window* window = _gfx_window_get_current();
		if(window) window->extensions.DeleteShader(internal->handle);

		free(shader);
	}
}

/******************************************************/
int gfx_shader_set_source(GFXShader* shader, size_t num, const int* length, const char** src)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window || !num) return 0;

	struct GFX_Internal_Shader* internal = (struct GFX_Internal_Shader*)shader;

	size_t newNum = num;
	int*   newLen = NULL;
	char** newSrc = NULL;

	/* Search for #version preprocessor */
	const char* vers = "#version";
	size_t n;

	for(n = 0; n < num; ++n)
		if(strstr(src[n], vers)) break;

	/* Append it if not found */
	n = (n >= num) ? 1 : 0;
	if(n)
	{
		/* Fetch version string */
		const char* glsl = _gfx_shader_eval_glsl(window->context.major, window->context.minor);
		if(!glsl) return 0;

		size_t size = strlen(glsl);

		/* Allocate new lengths */
		++newNum;
		if(length)
		{
			newLen = malloc(sizeof(int) * newNum);
			if(!newLen) return 0;

			memcpy(newLen + 1, length, sizeof(int) * num);
			*newLen = -1;
		}

		/* Allocate new sources */
		newSrc = malloc(sizeof(char*) * newNum);
		if(!newSrc)
		{
			free(newLen);
			return 0;
		}
		memcpy(newSrc + 1, src, sizeof(char*) * num);

		/* Construct preprocessor */
		*newSrc = malloc(sizeof(char) * (11 + size));
		if(!*newSrc)
		{
			free(newLen);
			free(newSrc);

			return 0;
		}

		(*newSrc)[8] = ' ';
		(*newSrc)[9 + size] = '\n';
		(*newSrc)[10 + size] = '\0';

		memcpy(*newSrc, vers, sizeof(char) << 3);
		memcpy(*newSrc + 9, glsl, sizeof(char) * size);
	}

	/* Set the source */
	window->extensions.ShaderSource(
		internal->handle,
		newNum,
		n ? (const char**)newSrc : src,
		n ? (const int*)newLen : length
	);
	shader->compiled = 0;

	/* Deallocate new data */
	if(newSrc) free(*newSrc);

	free(newLen);
	free(newSrc);

	return 1;
}

/******************************************************/
char* gfx_shader_get_source(GFXShader* shader, size_t* length)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window)
	{
		if(length) *length = 0;
		return NULL;
	}

	struct GFX_Internal_Shader* internal = (struct GFX_Internal_Shader*)shader;

	/* Get source length */
	GLint len;
	window->extensions.GetShaderiv(internal->handle, GL_SHADER_SOURCE_LENGTH, &len);
	if(!len)
	{
		if(length) *length = 0;
		return NULL;
	}

	/* Get actual source */
	char* buff = malloc(len);
	window->extensions.GetShaderSource(internal->handle, len, NULL, buff);

	if(length) *length = len - 1;

	return buff;
}

/******************************************************/
int gfx_shader_compile(GFXShader* shader)
{
	/* Already compiled */
	if(!shader->compiled)
	{
		/* Get current window and context */
		GFX_Internal_Window* window = _gfx_window_get_current();
		if(!window) return 0;

		struct GFX_Internal_Shader* internal = (struct GFX_Internal_Shader*)shader;

		/* Try to compile */
		GLint status;
		window->extensions.CompileShader(internal->handle);
		window->extensions.GetShaderiv(internal->handle, GL_COMPILE_STATUS, &status);

		/* Generate error */
		if(!status)
		{
			GLint len;
			window->extensions.GetShaderiv(internal->handle, GL_INFO_LOG_LENGTH, &len);

			char buff[len];
			window->extensions.GetShaderInfoLog(internal->handle, len, NULL, buff);
			gfx_errors_push(GFX_ERROR_COMPILE_FAIL, buff);

			return 0;
		}
	}

	/* Yep, compiled! */
	return shader->compiled = 1;
}
