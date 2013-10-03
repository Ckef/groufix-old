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
/** Internal Program */
struct GFX_Internal_Program
{
	/* Super class */
	GFXProgram program;

	/* Hidden data */
	GLuint  handle; /* OpenGL handle */
};

/******************************************************/
static void _gfx_program_obj_free(void* object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Program* program = (struct GFX_Internal_Program*)object;

	ext->DeleteProgram(program->handle);
	program->handle = 0;

	program->program.id = 0;
}

/******************************************************/
/* vtable for hardware part of the program */
static GFX_Hardware_Funcs _gfx_program_obj_funcs =
{
	_gfx_program_obj_free,
	NULL,
	NULL
};

/******************************************************/
GLuint _gfx_program_get_handle(const GFXProgram* program)
{
	return ((struct GFX_Internal_Program*)program)->handle;
}

/******************************************************/
GFXProgram* gfx_program_create(void)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Create new program */
	struct GFX_Internal_Program* prog = calloc(1, sizeof(struct GFX_Internal_Program));
	if(!prog) return NULL;

	/* Register as object */
	prog->program.id = _gfx_hardware_object_register(prog, &_gfx_program_obj_funcs);
	if(!prog->program.id)
	{
		free(prog);
		return NULL;
	}

	/* Create OpenGL program */
	prog->handle = window->extensions.CreateProgram();

	return (GFXProgram*)prog;
}

/******************************************************/
void gfx_program_free(GFXProgram* program)
{
	if(program)
	{
		struct GFX_Internal_Program* internal = (struct GFX_Internal_Program*)program;

		/* Unregister as object */
		_gfx_hardware_object_unregister(program->id);

		/* Get current window and context */
		GFX_Internal_Window* window = _gfx_window_get_current();
		if(window) window->extensions.DeleteProgram(internal->handle);

		free(program);
	}
}

/******************************************************/
int gfx_program_link(GFXProgram* program, size_t num, GFXShader** shaders)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return 0;

	struct GFX_Internal_Program* internal = (struct GFX_Internal_Program*)program;

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
	window->extensions.GetProgramiv(internal->handle, GL_LINK_STATUS, &status);

	/* Detach all shaders */
	for(i = 0; i < num; ++i) window->extensions.DetachShader(
		internal->handle,
		_gfx_shader_get_handle(shaders[i])
	);

	/* Generate error */
	if(!status)
	{
		GLint len;
		window->extensions.GetProgramiv(internal->handle, GL_INFO_LOG_LENGTH, &len);

		char buff[len];
		window->extensions.GetProgramInfoLog(internal->handle, len, NULL, buff);
		gfx_errors_push(GFX_ERROR_LINK_FAIL, buff);
	}

	return status;
}
