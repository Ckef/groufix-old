/**
 * Groufix  :  Graphics Engine produced by Ckef Worx
 * www      :  http://www.ejb.ckef-worx.com
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "groufix/internal.h"
#include "groufix/errors.h"

#include <stdlib.h>

/******************************************************/
/** Internal Shader */
struct GFX_Internal_Shader
{
	/* Super class */
	GFXShader shader;

	/* Hidden data */
	GLuint  handle; /* OpenGL handle */
	size_t  id;     /* Unique id */
};

/******************************************************/
static int _gfx_shader_eval_type(GFXShaderType type, const GFX_Extensions* ext)
{
	switch(type)
	{
		case GFX_TESS_CONTROL_SHADER :
		case GFX_TESS_EVAL_SHADER :
			if(!ext->flags[GFX_EXT_TESSELLATION_SHADER]) gfx_errors_push(
				GFX_ERROR_INCOMPATIBLE_CONTEXT,
				"GFX_EXT_TESSELLATION_SHADER is incompatible with this context."
			);
			return 0;

		case GFX_GEOMETRY_SHADER :
			if(!ext->flags[GFX_EXT_GEOMETRY_SHADER]) gfx_errors_push(
				GFX_ERROR_INCOMPATIBLE_CONTEXT,
				"GFX_EXT_GEOMETRY_SHADER is incompatible with this context."
			);
			return 0;

		default :
			return 1;
	}
}

/******************************************************/
static void _gfx_shader_obj_free(void* object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Shader* shader = (struct GFX_Internal_Shader*)object;

	ext->DeleteShader(shader->handle);
	shader->handle = 0;

	shader->id = 0;
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

	/* Evaluate shader type */
	if(!_gfx_shader_eval_type(type, &window->extensions)) return NULL;

	/* Create new shader */
	struct GFX_Internal_Shader* shader = calloc(1, sizeof(struct GFX_Internal_Shader));
	if(!shader) return NULL;

	/* Register as object */
	shader->id = _gfx_hardware_object_register(shader, &_gfx_shader_obj_funcs);
	if(!shader->id)
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
		_gfx_hardware_object_unregister(internal->id);

		/* Get current window and context */
		GFX_Internal_Window* window = _gfx_window_get_current();
		if(window) window->extensions.DeleteShader(internal->handle);

		free(shader);
	}
}
