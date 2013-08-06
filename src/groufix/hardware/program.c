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

#include <stdlib.h>

/******************************************************/
static int _gfx_hardware_program_find_shader(const VectorIterator it, const void* value)
{
	return *(GFXHardwareShader**)it == value;
}

/******************************************************/
static int _gfx_hardware_program_set_link_status(GFXHardwareProgram* program, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	GLint status;
	ext->GetProgramiv(program->handle, GL_LINK_STATUS, &status);

	program->linked = status;
	if(status) vector_clear(program->shaders);

	return status;
}

/******************************************************/
GFXHardwareProgram* gfx_hardware_program_create(const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	/* Allocate */
	GFXHardwareProgram* program = (GFXHardwareProgram*)calloc(1, sizeof(GFXHardwareProgram));
	if(!program) return NULL;

	program->shaders = vector_create(sizeof(GFXHardwareProgram*));
	if(!program->shaders)
	{
		free(program);
		return NULL;
	}
	program->handle = ext->CreateProgram();

	return program;
}

/******************************************************/
void gfx_hardware_program_free(GFXHardwareProgram* program, const GFXHardwareContext cnt)
{
	if(program)
	{
		const GFX_Extensions* ext = VOID_TO_EXT(cnt);

		ext->DeleteProgram(program->handle);
		vector_free(program->shaders);
		free(program);
	}
}

/******************************************************/
void gfx_hardware_program_use(GFXHardwareProgram* program, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	ext->UseProgram(program->handle);
}

/******************************************************/
int gfx_hardware_program_attach_shader(GFXHardwareProgram* program, GFXHardwareShader* shader, const GFXHardwareContext cnt)
{
	/* First check if it is already attached */
	if(vector_find(program->shaders, shader, _gfx_hardware_program_find_shader) != program->shaders->end) return 1;

	/* And insert the shader */
	if(vector_insert(program->shaders, &shader, program->shaders->end) == program->shaders->end) return 0;

	program->linked = 0;

	return 1;
}

/******************************************************/
int gfx_hardware_program_detach_shader(GFXHardwareProgram* program, GFXHardwareShader* shader, const GFXHardwareContext cnt)
{
	/* Find the shader and erase */
	VectorIterator it = vector_find(program->shaders, shader, _gfx_hardware_program_find_shader);
	int ret = (it != program->shaders->end);

	vector_erase(program->shaders, it);

	return ret;
}

/******************************************************/
void gfx_hardware_program_set_attribute(GFXHardwareProgram* program, unsigned int index, const char* name, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	ext->BindAttribLocation(program->handle, index, name);
}

/******************************************************/
int gfx_hardware_program_get_attribute(GFXHardwareProgram* program, const char* name, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	return ext->GetAttribLocation(program->handle, name);
}

/******************************************************/
int gfx_hardware_program_link(GFXHardwareProgram* program, int binary, const GFXHardwareContext cnt)
{
	/* Already linked with latest shaders */
	if(program->linked) return 1;

	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	/* Compile and attach all shaders */
	int compiled = 1;
	VectorIterator it;
	for(it = program->shaders->begin; it != program->shaders->end; it = vector_next(program->shaders, it))
	{
		GFXHardwareShader* shader = *(GFXHardwareShader**)it;

		compiled = !(!compiled | !gfx_hardware_shader_compile(shader, cnt));
		ext->AttachShader(program->handle, shader->handle);
	}
	if(compiled)
	{
		/* Set hints */
		if(ext->extensions[GFX_EXT_PROGRAM_BINARY])
			ext->ProgramParameteri(program->handle, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, binary);

		/* Link all shaders */
		ext->LinkProgram(program->handle);
	}

	/* Detach all shaders, regardless of being compiled correctly */
	for(it = program->shaders->begin; it != program->shaders->end; it = vector_next(program->shaders, it))
		ext->DetachShader(program->handle, (*(GFXHardwareShader**)it)->handle);

	return _gfx_hardware_program_set_link_status(program, cnt);
}

/******************************************************/
char* gfx_hardware_program_get_info_log(GFXHardwareProgram* program, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	GLint length;
	ext->GetProgramiv(program->handle, GL_INFO_LOG_LENGTH, &length);

	if(!length) return NULL;

	char* log = (char*)malloc(sizeof(char) * length);
	ext->GetProgramInfoLog(program->handle, length, NULL, log);

	return log;
}

/******************************************************/
void* gfx_hardware_program_get_binary(GFXHardwareProgram* program, unsigned int* length, GFXProgramFormat* format, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	/* Validate length */
	GLint len;
	ext->GetProgramiv(program->handle, GL_PROGRAM_BINARY_LENGTH, &len);

	if(!len) return NULL;

	/* Get binary */
	GLenum form;
	void* buff = malloc(len);
	ext->GetProgramBinary(program->handle, len, NULL, &form, buff);

	*length = len;
	*format = form;

	return buff;
}

/******************************************************/
int gfx_hardware_program_set_binary(GFXHardwareProgram* program, const void* bin, unsigned int length, GFXProgramFormat format, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	ext->ProgramBinary(program->handle, format, bin, length);

	return _gfx_hardware_program_set_link_status(program, cnt);
}
