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

/******************************************************/
/* Internal Program */
struct GFX_Program
{
	/* Super class */
	GFXProgram program;

	/* Hidden data */
	GLuint           handle; /* OpenGL handle */
	GFXPropertyMap*  map;    /* Last used map on this program, not used internally */
};

/******************************************************/
static void _gfx_program_obj_free(void* object, GFX_Extensions* ext)
{
	struct GFX_Program* program = (struct GFX_Program*)object;

	ext->DeleteProgram(program->handle);
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
int _gfx_program_target(GFXProgram* program, GFXPropertyMap* map)
{
	struct GFX_Program* internal = (struct GFX_Program*)program;

	if(internal->map == map) return 0;
	internal->map = map;

	return 1;
}

/******************************************************/
void _gfx_program_untarget(GFXProgram* program, GFXPropertyMap* map)
{
	struct GFX_Program* internal = (struct GFX_Program*)program;

	if(internal->map == map) internal->map = NULL;
}

/******************************************************/
GLuint _gfx_program_get_handle(const GFXProgram* program)
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

		free(program);
	}
}

/******************************************************/
int gfx_program_set_attribute(GFXProgram* program, unsigned int index, const char* name)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return 0;
	
	if(index >= window->extensions.limits[GFX_LIM_MAX_VERTEX_ATTRIBS]) return 0;

	struct GFX_Program* internal = (struct GFX_Program*)program;
	
	/* Set the attribute */
	window->extensions.BindAttribLocation(internal->handle, index, name);
	
	return 1;
}

/******************************************************/
int gfx_program_link(GFXProgram* program, size_t num, GFXShader** shaders)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return 0;

	struct GFX_Program* internal = (struct GFX_Program*)program;

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

/******************************************************/
void* gfx_program_get_binary(GFXProgram* program, GFXProgramFormat* format, size_t* size)
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
	window->extensions.GetProgramiv(internal->handle, GL_PROGRAM_BINARY_LENGTH, &bytes);
	*size = bytes;

	if(!bytes) return NULL;

	/* Get the data */
	void* data = malloc(bytes);
	window->extensions.GetProgramBinary(internal->handle, bytes, NULL, format, data);

	return data;
}

/******************************************************/
int gfx_program_set_binary(GFXProgram* program, GFXProgramFormat format, size_t size, const void* data)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return 0;

	struct GFX_Program* internal = (struct GFX_Program*)program;

	/* Set binary representation */
	GLint status;
	window->extensions.ProgramBinary(internal->handle, format, data, size);
	window->extensions.GetProgramiv(internal->handle, GL_LINK_STATUS, &status);

	return status;
}
