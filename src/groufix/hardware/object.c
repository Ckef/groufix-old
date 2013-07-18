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

#include "groufix/hardware.h"
#include "groufix/internal.h"

/******************************************************/
GFXHardwareObject gfx_hardware_object_create(const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	GLuint vao;
	ext->GenVertexArrays(1, &vao);
	ext->BindVertexArray(vao);

	return vao;
}

/******************************************************/
void gfx_hardware_object_free(GFXHardwareObject object, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	GLuint vao = object;
	ext->DeleteVertexArrays(1, &vao);
}

/******************************************************/
void gfx_hardware_object_bind(GFXHardwareObject object, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	ext->BindVertexArray(object);
}

/******************************************************/
int gfx_hardware_object_enable_attribute(unsigned int index, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	/* Validate index */
	GLint max;
	ext->GetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max);

	if(index >= max) return 0;

	ext->EnableVertexAttribArray(index);

	return 1;
}

/******************************************************/
int gfx_hardware_object_disable_attribute(unsigned int index, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	/* Validate index */
	GLint max;
	ext->GetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max);

	if(index >= max) return 0;

	ext->DisableVertexAttribArray(index);

	return 1;
}
