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

#include "groufix/containers/vector.h"
#include "groufix/internal.h"

#include <limits.h>
#include <string.h>

/******************************************************/
/* Reference count */
static size_t _gfx_binder_ref_count = 0;

/* Texture Unit & Buffer binder */
static struct GFX_Internal_Binder
{
	GFXVector uniformBuffers; /* Stores unsigned char + GFX_Internal_UniformBuffer */
	GFXVector textureUnits;   /* Stores unsigned char + GFX_Internal_TextureUnit */

} _gfx_binder;

/******************************************************/
/* Uniform buffer key */
struct GFX_Internal_UniformBuffer
{
	GLuint      buffer;
	GLintptr    offset;
	GLsizeiptr  size;
};

/* Texture unit key */
struct GFX_Internal_TextureUnit
{
	GLuint texture;
};

/******************************************************/
static void _gfx_binder_increase(GFXVector* vector)
{
	/* First increase all counters */
	GFXVectorIterator it;
	for(it = vector->begin; it != vector->end; it = gfx_vector_next(vector, it))
	{
		unsigned char* cnt = (unsigned char*)it;

		++(*cnt);
		*cnt = !(*cnt) ? UCHAR_MAX : *cnt;
	}
}

/******************************************************/
/* maxUnits is presumed to be > 0 */
static size_t _gfx_binder_request(GFXVector* vector, const void* data, int prioritize, int maxUnits, int* new)
{
	GFXVectorIterator pos = vector->end;
	*new = 1;

	size_t size = vector->elementSize - sizeof(unsigned char);
	size_t elem = gfx_vector_get_size(vector);

	/* First increase all counters */
	if(prioritize) _gfx_binder_increase(vector);

	/* Find highest or equal entry */
	GFXVectorIterator high = vector->end;
	short highCnt = -1;

	GFXVectorIterator it;
	for(it = vector->begin; it != vector->end; it = gfx_vector_next(vector, it))
	{
		unsigned char* cnt = (unsigned char*)it;

		/* Check if equal */
		if(!memcmp(data, cnt + 1, size))
		{
			pos = it;
			*new = 0;

			break;
		}

		/* Find highest */
		if(highCnt < (short)(*cnt))
		{
			high = it;
			highCnt = *cnt;
		}
	}

	/* Get new position */
	if(*new)
	{
		if(elem >= maxUnits) pos = high;
		else pos = gfx_vector_insert(vector, NULL, vector->end);
	}

	/* Prioritize itself and copy data */
	unsigned char* cnt = (unsigned char*)pos;
	*cnt = prioritize ? 0 : UCHAR_MAX;

	memcpy(cnt + 1, data, size);

	return gfx_vector_get_index(vector, pos);
}

/******************************************************/
void _gfx_binder_reference(int ref)
{
	/* Clear vector */
	if(_gfx_binder_ref_count <= -ref)
	{
		/* Make sure they were initialized */
		if(!_gfx_binder_ref_count) return;

		gfx_vector_clear(&_gfx_binder.uniformBuffers);
		gfx_vector_clear(&_gfx_binder.textureUnits);

		ref = -_gfx_binder_ref_count;
	}

	/* Initialize vectors */
	else if(!_gfx_binder_ref_count && ref)
	{
		gfx_vector_init(&_gfx_binder.uniformBuffers,
			sizeof(unsigned char) + sizeof(struct GFX_Internal_UniformBuffer));
		gfx_vector_init(&_gfx_binder.textureUnits,
			sizeof(unsigned char) + sizeof(struct GFX_Internal_TextureUnit));
	}

	_gfx_binder_ref_count += ref;
}

/******************************************************/
size_t _gfx_binder_bind_uniform_buffer(GLuint buffer, GLintptr offset, GLsizeiptr size, int prioritize, const GFX_Extensions* ext)
{
	/* Get unit to bind it to */
	struct GFX_Internal_UniformBuffer buff;
	buff.buffer = buffer;
	buff.offset = offset;
	buff.size = size;

	int new;
	size_t bind = _gfx_binder_request(
		&_gfx_binder.uniformBuffers,
		&buff,
		prioritize,
		ext->limits[GFX_LIM_MAX_BUFFER_PROPERTIES],
		&new
	);

	/* Bind the buffer */
	if(new) ext->BindBufferRange(GL_UNIFORM_BUFFER, bind, buffer, offset, size);

	return bind;
}

/******************************************************/
size_t _gfx_binder_bind_texture(GLuint texture, GLenum target, int prioritize, const GFX_Extensions* ext)
{
	/* Get unit to bind it to */
	struct GFX_Internal_TextureUnit unit;
	unit.texture = texture;

	int new;
	size_t bind = _gfx_binder_request(
		&_gfx_binder.textureUnits,
		&unit,
		prioritize,
		ext->limits[GFX_LIM_MAX_SAMPLER_PROPERTIES],
		&new
	);

	/* Bind the texture */
	ext->ActiveTexture(GL_TEXTURE0 + bind);
	if(new) ext->BindTexture(target, texture);

	return bind;
}
