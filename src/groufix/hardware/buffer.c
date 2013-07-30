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
GFXHardwareBuffer* gfx_hardware_buffer_create(GFXBufferTarget target, GFXBufferUsage use, size_t size, const void* data, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	/* Allocate */
	GFXHardwareBuffer* buff = (GFXHardwareBuffer*)malloc(sizeof(GFXHardwareBuffer));
	if(!buff) return NULL;

	GLuint handle;
	ext->GenBuffers(1, &handle);
	ext->BindBuffer(target, handle);
	ext->BufferData(target, size, data, use);

	buff->handle = handle;
	buff->size = size;

	return buff;
}

/******************************************************/
GFXHardwareBuffer* gfx_hardware_buffer_create_copy(GFXBufferTarget target, GFXBufferUsage use, GFXHardwareBuffer* src, const GFXHardwareContext cnt)
{
	GFXHardwareBuffer* buff = gfx_hardware_buffer_create(target, use, src->size, NULL, cnt);
	if(!buff) return NULL;

	gfx_hardware_buffer_copy(buff, src, 0, 0, src->size, cnt);

	return buff;
}

/******************************************************/
void gfx_hardware_buffer_free(GFXHardwareBuffer* buffer, const GFXHardwareContext cnt)
{
	if(buffer)
	{
		const GFX_Extensions* ext = VOID_TO_EXT(cnt);

		GLuint handle = buffer->handle;
		ext->DeleteBuffers(1, &handle);

		free(buffer);
	}
}

/******************************************************/
GFXBufferUsage gfx_hardware_buffer_get_usage(GFXHardwareBuffer* buffer, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	GLint usage;
	ext->BindBuffer(GL_ARRAY_BUFFER, buffer->handle);
	ext->GetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);

	return usage;
}

/******************************************************/
void gfx_hardware_buffer_realloc(GFXHardwareBuffer* buffer, GFXBufferUsage use, size_t size, const void* data, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	ext->BindBuffer(GL_ARRAY_BUFFER, buffer->handle);
	ext->BufferData(GL_ARRAY_BUFFER, size, data, use);

	buffer->size = size;
}

/******************************************************/
void gfx_hardware_buffer_write(GFXHardwareBuffer* buffer, size_t offset, size_t size, const void* data, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	ext->BindBuffer(GL_ARRAY_BUFFER, buffer->handle);
	ext->BufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

/******************************************************/
void gfx_hardware_buffer_read(GFXHardwareBuffer* buffer, size_t offset, size_t size, void* data, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	ext->BindBuffer(GL_ARRAY_BUFFER, buffer->handle);
	ext->GetBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

/******************************************************/
void gfx_hardware_buffer_copy(GFXHardwareBuffer* dest, GFXHardwareBuffer* src, size_t destOffset, size_t srcOffset, size_t size, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	ext->BindBuffer(GL_COPY_READ_BUFFER, src->handle);
	ext->BindBuffer(GL_COPY_WRITE_BUFFER, dest->handle);
	ext->CopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, srcOffset, destOffset, size);
}

/******************************************************/
void* gfx_hardware_buffer_map(GFXHardwareBuffer* buffer, GFXBufferAccess access, size_t offset, size_t length, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	ext->BindBuffer(GL_ARRAY_BUFFER, buffer->handle);
	GLvoid* ptr = ext->MapBufferRange(GL_ARRAY_BUFFER, offset, length, access);

	return ptr;
}

/******************************************************/
void* gfx_hardware_buffer_get_map(GFXHardwareBuffer* buffer, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	GLvoid* ptr;
	ext->BindBuffer(GL_ARRAY_BUFFER, buffer->handle);
	ext->GetBufferPointerv(GL_ARRAY_BUFFER, GL_BUFFER_MAP_POINTER, &ptr);

	return ptr;
}

/******************************************************/
void gfx_hardware_buffer_unmap(GFXHardwareBuffer* buffer, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	ext->BindBuffer(GL_ARRAY_BUFFER, buffer->handle);
	if(!ext->UnmapBuffer(GL_ARRAY_BUFFER)) gfx_errors_push(
		GFX_ERROR_MEMORY_CORRUPTION,
		"Mapping a buffer might have corrupted its memory."
	);
}
