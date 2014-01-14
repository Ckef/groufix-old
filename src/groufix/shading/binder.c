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

#include <limits.h>
#include <stdlib.h>
#include <string.h>

/* Counter limits */
#define GFX_BINDER_COUNTER_EMPTY  UCHAR_MAX
#define GFX_BINDER_COUNTER_MIN    0
#define GFX_BINDER_COUNTER_MAX    (GFX_BINDER_COUNTER_EMPTY - 1)

/******************************************************/
/* Binder unit */
struct GFX_Internal_Unit
{
	unsigned char counter;
};

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
static void* _gfx_binder_init(size_t num, size_t size)
{
	size_t unitSize = sizeof(struct GFX_Internal_Unit) + size;

	/* Allocate */
	void* data = calloc(num, unitSize);
	if(!data) return NULL;

	void* bindings = data;

	/* Iterate and set to empty */
	while(num--)
	{
		struct GFX_Internal_Unit* unit = (struct GFX_Internal_Unit*)bindings;
		unit->counter = GFX_BINDER_COUNTER_EMPTY;

		/* Next unit */
		bindings = GFX_PTR_ADD_BYTES(bindings, unitSize);
	}

	return data;
}

/******************************************************/
static void _gfx_binder_increase(int sign, unsigned char min, void* bindings, size_t num, size_t size)
{
	size_t unitSize = sizeof(struct GFX_Internal_Unit) + size;

	/* Iterate and increase */
	while(num--)
	{
		struct GFX_Internal_Unit* unit = (struct GFX_Internal_Unit*)bindings;

		/* Check against minimum and increase according to sign */
		if(unit->counter >= min)
		{
			if(sign >= 0) unit->counter = (unit->counter != GFX_BINDER_COUNTER_MAX) ?
				unit->counter + 1 : unit->counter;

			else unit->counter = (unit->counter != GFX_BINDER_COUNTER_MIN) ?
				unit->counter - 1 : unit->counter;
		}

		/* Next unit */
		bindings = GFX_PTR_ADD_BYTES(bindings, unitSize);
	}
}

/******************************************************/
static void _gfx_binder_unbind(void* bindings, size_t num, size_t size, size_t cmpSize, const void* cmp)
{
	struct GFX_Internal_Unit* curr = bindings;
	size_t unitSize = sizeof(struct GFX_Internal_Unit) + size;

	/* Iterate */
	size_t i;
	for(i = 0; i < num; ++i)
	{
		/* Empty current unit */
		if(!memcmp(curr + 1, cmp, cmpSize))
		{
			_gfx_binder_increase(-1, curr->counter, bindings, num, size);

			memset(curr, 0, unitSize);
			curr->counter = GFX_BINDER_COUNTER_EMPTY;
		}

		/* Next unit */
		curr = GFX_PTR_ADD_BYTES(curr, unitSize);
	}
}

/******************************************************/
static size_t _gfx_binder_request(void* bindings, size_t num, size_t size, const void* data, int prioritize, int* new)
{
	*new = 1;

	/* Erm, nothing to find */
	if(!bindings) return 0;

	/* First increase all counters */
	if(prioritize) _gfx_binder_increase(1, 0, bindings, num, size);

	struct GFX_Internal_Unit* pos = NULL;
	size_t unitSize = sizeof(struct GFX_Internal_Unit) + size;

	/* Find highest or equal entry */
	struct GFX_Internal_Unit* high = (struct GFX_Internal_Unit*)bindings;
	struct GFX_Internal_Unit* curr = high;

	while(num--)
	{
		/* Check for empty */
		if(curr->counter == GFX_BINDER_COUNTER_EMPTY)
		{
			pos = curr;
			break;
		}

		/* Check if equal & find highest */
		if(!memcmp(data, curr + 1, size))
		{
			pos = curr;
			*new = 0;

			break;
		}
		high = (high->counter < curr->counter) ? curr : high;

		/* Next unit */
		curr = GFX_PTR_ADD_BYTES(curr, unitSize);
	}

	/* Get new position */
	pos = pos ? pos : high;

	/* Prioritize itself and copy data */
	pos->counter = prioritize ? GFX_BINDER_COUNTER_MIN : GFX_BINDER_COUNTER_MAX;
	memcpy(pos + 1, data, size);

	return GFX_PTR_DIFF(bindings, pos) / unitSize;
}

/******************************************************/
size_t _gfx_binder_bind_uniform_buffer(GLuint buffer, GLintptr offset, GLsizeiptr size, int prioritize, GFX_Extensions* ext)
{
	/* Allocate binding points */
	if(!ext->uniformBuffers) ext->uniformBuffers = _gfx_binder_init(
		ext->limits[GFX_LIM_MAX_BUFFER_PROPERTIES],
		sizeof(struct GFX_Internal_UniformBuffer)
	);

	/* Get unit to bind it to */
	struct GFX_Internal_UniformBuffer buff;
	buff.buffer = buffer;
	buff.offset = offset;
	buff.size = size;

	int new;
	size_t bind = _gfx_binder_request(
		ext->uniformBuffers,
		ext->limits[GFX_LIM_MAX_BUFFER_PROPERTIES],
		sizeof(struct GFX_Internal_UniformBuffer),
		&buff,
		prioritize,
		&new
	);

	/* Bind the buffer */
	if(new) ext->BindBufferRange(GL_UNIFORM_BUFFER, bind, buffer, offset, size);

	return bind;
}

/******************************************************/
void _gfx_binder_unbind_uniform_buffer(GLuint buffer, GFX_Extensions* ext)
{
	if(ext->uniformBuffers)
	{
		struct GFX_Internal_UniformBuffer buff;
		buff.buffer = buffer;

		_gfx_binder_unbind(
			ext->uniformBuffers,
			ext->limits[GFX_LIM_MAX_BUFFER_PROPERTIES],
			sizeof(struct GFX_Internal_UniformBuffer),
			offsetof(struct GFX_Internal_UniformBuffer, buffer) + sizeof(GLuint),
			&buff
		);
	}
}

/******************************************************/
size_t _gfx_binder_bind_texture(GLuint texture, GLenum target, int prioritize, GFX_Extensions* ext)
{
	/* Allocate binding points */
	if(!ext->textureUnits) ext->textureUnits = _gfx_binder_init(
		ext->limits[GFX_LIM_MAX_SAMPLER_PROPERTIES],
		sizeof(struct GFX_Internal_TextureUnit)
	);

	/* Get unit to bind it to */
	struct GFX_Internal_TextureUnit unit;
	unit.texture = texture;

	int new;
	size_t bind = _gfx_binder_request(
		ext->textureUnits,
		ext->limits[GFX_LIM_MAX_SAMPLER_PROPERTIES],
		sizeof(struct GFX_Internal_TextureUnit),
		&unit,
		prioritize,
		&new
	);

	/* Bind the texture */
	ext->ActiveTexture(GL_TEXTURE0 + bind);
	if(new) ext->BindTexture(target, texture);

	return bind;
}

/******************************************************/
void _gfx_binder_unbind_texture(GLuint texture, GFX_Extensions* ext)
{
	if(ext->textureUnits)
	{
		struct GFX_Internal_TextureUnit unit;
		unit.texture = texture;

		_gfx_binder_unbind(
			ext->textureUnits,
			ext->limits[GFX_LIM_MAX_SAMPLER_PROPERTIES],
			sizeof(struct GFX_Internal_TextureUnit),
			offsetof(struct GFX_Internal_TextureUnit, texture) + sizeof(GLuint),
			&unit
		);
	}
}
