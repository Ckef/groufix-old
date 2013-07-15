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
GFXHardwareBuffer gfx_hardware_buffer_create(GFXBufferTarget target, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = CONTEXT_TO_EXT(cnt);

	GLuint buff;
	ext->GenBuffers(1, &buff);
	ext->BindBuffer(target, buff);

	return buff;
}

/******************************************************/
void gfx_hardware_buffer_free(GFXHardwareBuffer buffer, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = CONTEXT_TO_EXT(cnt);

	GLuint buff = buffer;
	ext->DeleteBuffers(1, &buff);
}

/******************************************************/
void gfx_hardware_buffer_bind(GFXBufferTarget target, GFXHardwareBuffer buffer, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = CONTEXT_TO_EXT(cnt);

	ext->BindBuffer(target, buffer);
}

/******************************************************/
GFXHardwareBuffer gfx_hardware_buffer_get(GFXBufferTarget target, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = CONTEXT_TO_EXT(cnt);

	GLint buff = 0;
	switch(target)
	{
		case GFX_BUFFER_TARGET_VERTEX_ARRAY :
			ext->GetIntegerv(GL_ARRAY_BUFFER_BINDING, &buff);
			break;

		case GFX_BUFFER_TARGET_ELEMENT_ARRAY :
			ext->GetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &buff);
			break;
	}

	return buff;
}

/******************************************************/
size_t gfx_hardware_buffer_get_size(GFXBufferTarget target, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = CONTEXT_TO_EXT(cnt);

	GLint size;
	ext->GetBufferParameteriv(target, GL_BUFFER_SIZE, &size);

	return size;
}

/******************************************************/
GFXBufferUsage gfx_hardware_buffer_get_usage(GFXBufferTarget target, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = CONTEXT_TO_EXT(cnt);

	GLint usage;
	ext->GetBufferParameteriv(target, GL_BUFFER_USAGE, &usage);

	return usage;
}

/******************************************************/
void gfx_hardware_buffer_allocate(GFXBufferTarget target, size_t size, const void* data, GFXBufferUsage use, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = CONTEXT_TO_EXT(cnt);

	ext->BufferData(target, size, data, use);
}

/******************************************************/
size_t gfx_hardware_buffer_write(GFXBufferTarget target, size_t offset, size_t size, const void* data, const GFXHardwareContext cnt)
{
	if(!size || !data) return 0;

	const GFX_Extensions* ext = CONTEXT_TO_EXT(cnt);

	/* Boundaries! */
	size_t writeSize = gfx_hardware_buffer_get_size(target, cnt);
	if(offset >= writeSize) return 0;

	writeSize -= offset;
	if(writeSize > size) writeSize = size;

	ext->BufferSubData(target, offset, writeSize, data);

	return writeSize;
}
