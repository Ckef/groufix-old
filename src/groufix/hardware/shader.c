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
GFXHardwareShader* gfx_hardware_shader_create(GFXShaderStage stage, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	/* Check shader stage right away */
	switch(stage)
	{
		case GFX_SHADER_GEOMETRY :
		{
			if(!gfx_hardware_is_extension_supported(GFX_EXT_GEOMETRY_SHADER, cnt) gfx_errors_push(
				GFX_ERROR_INCOMPATIBLE_CONTEXT,
				"GFX_EXT_GEOMETRY_SHADER is incompatible with this context."
			);
			return NULL;
		}
	}

	/* Allocate */
	GFXHardwareShader* shader = (GFXHardwareShader*)malloc(sizeof(GFXHardwareShader));
	if(!shader) return NULL;

	shader->handle = ext->CreateShader(stage);

	return shader;
}

/******************************************************/
void gfx_hardware_shader_free(GFXHardwareShader* shader, const GFXHardwareContext cnt)
{
	if(shader)
	{
		const GFX_Extensions* ext = VOID_TO_EXT(cnt);

		ext->DeleteShader(shader->handle);
		free(shader);
	}
}

/******************************************************/
GFXShaderStage gfx_hardware_shader_get_stage(GFXHardwareShader* shader, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	GLint stage;
	ext->GetShaderiv(shader->handle, GL_SHADER_TYPE, &stage);

	return stage;
}

/******************************************************/
void gfx_hardware_shader_set_source(GFXHardwareShader* shader, size_t count, const char** src, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	ext->ShaderSource(shader->handle, count, src, NULL);
}

/******************************************************/
char* gfx_hardware_shader_get_source(GFXHardwareShader* shader, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	GLint length;
	ext->GetShaderiv(shader->handle, GL_SHADER_SOURCE_LENGTH, &length);

	if(!length) return NULL;

	char* src = (char*)malloc(length);
	ext->GetShaderSource(shader->handle, length, NULL, src);

	return src;
}

/******************************************************/
int gfx_hardware_shader_compile(GFXHardwareShader* shader, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	ext->CompileShader(shader->handle);

	GLint status;
	ext->GetShaderiv(shader->handle, GL_COMPILE_STATUS, &status);

	return status;
}

/******************************************************/
char* gfx_hardware_shader_get_info_log(GFXHardwareShader* shader, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	GLint length;
	ext->GetShaderiv(shader->handle, GL_INFO_LOG_LENGTH, &length);

	if(!length) return NULL;

	char* log = (char*)malloc(length);
	ext->GetShaderInfoLog(shader->handle, length, NULL, log);

	return log;
}
