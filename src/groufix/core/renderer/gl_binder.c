
/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/errors.h"
#include "groufix/core/renderer.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

/* Counter limits */
#define GFX_BINDER_COUNTER_EMPTY  UCHAR_MAX
#define GFX_BINDER_COUNTER_MIN    0
#define GFX_BINDER_COUNTER_MAX    (GFX_BINDER_COUNTER_EMPTY - 1)

/******************************************************/
/* Binder unit */
typedef struct GFX_Unit
{
	unsigned char counter;

} GFX_Unit;


/* Uniform buffer key */
typedef struct GFX_UniformBuffer
{
	GLuint      buffer; /* Super class */
	GLintptr    offset;
	GLsizeiptr  size;

} GFX_UniformBuffer;


/* Texture unit key */
typedef struct GFX_TextureUnit
{
	GLuint  texture; /* Super class */
	GLuint  sampler;

} GFX_TextureUnit;


/******************************************************/
static void* _gfx_binder_init(

		size_t  num,
		size_t  size)
{
	size_t unitSize = sizeof(GFX_Unit) + size;

	/* Allocate */
	void* data = calloc(num, unitSize);
	if(!data)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Internal binder could not be allocated."
		);
		return NULL;
	}

	void* bindings = data;

	/* Iterate and set to empty */
	while(num--)
	{
		GFX_Unit* unit = (GFX_Unit*)bindings;
		unit->counter = GFX_BINDER_COUNTER_EMPTY;

		/* Next unit */
		bindings = GFX_PTR_ADD_BYTES(bindings, unitSize);
	}

	return data;
}

/******************************************************/
static void _gfx_binder_increase(

		void*          bindings,
		size_t         num,
		size_t         size,
		int            sign,
		unsigned char  min)
{
	size_t unitSize = sizeof(GFX_Unit) + size;

	/* Iterate and increase */
	while(num--)
	{
		GFX_Unit* unit = (GFX_Unit*)bindings;

		/* Check against minimum and increase according to sign */
		if(unit->counter != GFX_BINDER_COUNTER_EMPTY && unit->counter >= min)
		{
			if(sign >= 0) unit->counter =
				(unit->counter < GFX_BINDER_COUNTER_MAX) ?
				unit->counter + 1 : GFX_BINDER_COUNTER_MAX;

			else unit->counter =
				(unit->counter > GFX_BINDER_COUNTER_MIN) ?
				unit->counter - 1 : GFX_BINDER_COUNTER_MIN;
		}

		/* Next unit */
		bindings = GFX_PTR_ADD_BYTES(bindings, unitSize);
	}
}

/******************************************************/
static void _gfx_binder_unbind(

		void*   bindings,
		size_t  num,
		size_t  size,
		size_t  cmpOffset,
		size_t  cmpSize,
		void*   cmp)
{
	GFX_Unit* curr = bindings;
	size_t unitSize = sizeof(GFX_Unit) + size;

	cmp = GFX_PTR_ADD_BYTES(cmp, cmpOffset);

	/* Iterate */
	size_t i;
	for(i = 0; i < num; ++i)
	{
		/* Empty current unit */
		if(!memcmp(
			GFX_PTR_ADD_BYTES(curr + 1, cmpOffset),
			cmp,
			cmpSize))
		{
			_gfx_binder_increase(
				bindings,
				num,
				size,
				-1,
				curr->counter
			);

			curr->counter = GFX_BINDER_COUNTER_EMPTY;
			memset(curr + 1, 0, size);
		}

		/* Next unit */
		curr = GFX_PTR_ADD_BYTES(curr, unitSize);
	}
}

/******************************************************/
static size_t _gfx_binder_request(

		void*   bindings,
		size_t  num,
		size_t  size,
		size_t  dataOffset,
		size_t  dataSize,
		void*   data,
		int     prioritize,
		int*    old)
{
	GFX_Unit* pos = NULL;
	size_t unitSize = sizeof(GFX_Unit) + size;

	data = GFX_PTR_ADD_BYTES(data, dataOffset);
	*old = 0;

	/* First increase all counters */
	if(prioritize) _gfx_binder_increase(
		bindings,
		num,
		size,
		1,
		0
	);

	/* Find highest or equal entry */
	GFX_Unit* high = (GFX_Unit*)bindings;
	GFX_Unit* curr = high;

	while(num--)
	{
		if(!memcmp(
			GFX_PTR_ADD_BYTES(curr + 1, dataOffset),
			data,
			dataSize))
		{
			pos = curr;
			*old = 1;

			break;
		}

		high = (high->counter < curr->counter) ? curr : high;

		/* Next unit */
		curr = GFX_PTR_ADD_BYTES(curr, unitSize);
	}

	/* Get new position */
	pos = pos ? pos : high;

	/* Prioritize itself and copy data */
	pos->counter = prioritize ?
		GFX_BINDER_COUNTER_MIN :
		(pos->counter < GFX_BINDER_COUNTER_MAX ?
		pos->counter : GFX_BINDER_COUNTER_MAX);

	memcpy(
		GFX_PTR_ADD_BYTES(pos + 1, dataOffset),
		data,
		dataSize);

	return GFX_PTR_DIFF(bindings, pos) / unitSize;
}

/******************************************************/
size_t _gfx_binder_bind_uniform_buffer(

		GLuint      buffer,
		GLintptr    offset,
		GLsizeiptr  size,
		int         prioritize,
		GFX_WIND_ARG)
{
	size_t bind = 0;
	int old = 0;

	/* Allocate binding points */
	if(!GFX_REND_GET.uniformBuffers)
		GFX_REND_GET.uniformBuffers = _gfx_binder_init(
			GFX_WIND_GET.lim[GFX_LIM_MAX_BUFFER_PROPERTIES],
			sizeof(GFX_UniformBuffer)
		);

	if(GFX_REND_GET.uniformBuffers)
	{
		/* Get unit to bind it to */
		GFX_UniformBuffer buff =
		{
			.buffer = buffer,
			.offset = offset,
			.size = size
		};

		bind = _gfx_binder_request(
			GFX_REND_GET.uniformBuffers,
			GFX_WIND_GET.lim[GFX_LIM_MAX_BUFFER_PROPERTIES],
			sizeof(GFX_UniformBuffer),
			0,
			sizeof(GFX_UniformBuffer),
			&buff,
			prioritize,
			&old
		);
	}

	/* Bind the buffer */
	if(!old) GFX_REND_GET.BindBufferRange(
		GL_UNIFORM_BUFFER,
		bind,
		buffer,
		offset,
		size
	);

	return bind;
}

/******************************************************/
void _gfx_binder_unbind_uniform_buffer(

		GLuint buffer,
		GFX_WIND_ARG)
{
	if(GFX_REND_GET.uniformBuffers)
	{
		GFX_UniformBuffer buff;
		buff.buffer = buffer;

		_gfx_binder_unbind(
			GFX_REND_GET.uniformBuffers,
			GFX_WIND_GET.lim[GFX_LIM_MAX_BUFFER_PROPERTIES],
			sizeof(GFX_UniformBuffer),
			offsetof(GFX_UniformBuffer, buffer),
			sizeof(GLuint),
			&buff
		);
	}
}

/******************************************************/
static size_t _gfx_binder_bind_texture_unit(

		GFX_TextureUnit  unit,
		size_t           offset,
		size_t           size,
		int              prioritize,
		int*             old,
		GFX_WIND_ARG)
{
	size_t bind = 0;
	*old = 0;

	/* Allocate binding points */
	if(!GFX_REND_GET.textureUnits)
	{
		GFX_REND_GET.textureUnits = _gfx_binder_init(
			GFX_WIND_GET.lim[GFX_LIM_MAX_SAMPLER_PROPERTIES],
			sizeof(GFX_TextureUnit)
		);

		if(!GFX_REND_GET.textureUnits) return 0;
	}

	/* Get unit to bind it to */
	bind = _gfx_binder_request(
		GFX_REND_GET.textureUnits,
		GFX_WIND_GET.lim[GFX_LIM_MAX_SAMPLER_PROPERTIES],
		sizeof(GFX_TextureUnit),
		offset,
		size,
		&unit,
		prioritize,
		old
	);

	return bind;
}

/******************************************************/
size_t _gfx_binder_bind_texture(

		GLuint  texture,
		GLenum  target,
		int     prioritize,
		GFX_WIND_ARG)
{
	/* Get unit to bind it to */
	GFX_TextureUnit unit;
	unit.texture = texture;

	int old;
	size_t bind = _gfx_binder_bind_texture_unit(
		unit,
		offsetof(GFX_TextureUnit, texture),
		sizeof(GLuint),
		prioritize,
		&old,
		GFX_WIND_AS_ARG
	);

	/* Bind the texture */
	if(GFX_WIND_GET.ext[GFX_EXT_DIRECT_STATE_ACCESS])
	{
		if(!old) GFX_REND_GET.BindTextureUnit(bind, texture);
	}
	else
	{
		GFX_REND_GET.ActiveTexture(GL_TEXTURE0 + bind);
		if(!old) GFX_REND_GET.BindTexture(target, texture);
	}

	return bind;
}

/******************************************************/
size_t _gfx_binder_bind_sampler(

		GLuint  sampler,
		GLuint  texture,
		GLenum  target,
		int     prioritize,
		GFX_WIND_ARG)
{
	/* Get unit to bind it to */
	GFX_TextureUnit unit =
	{
		.texture = texture,
		.sampler = sampler
	};

	int old;
	size_t bind = _gfx_binder_bind_texture_unit(
		unit,
		0,
		sizeof(GFX_TextureUnit),
		prioritize,
		&old,
		GFX_WIND_AS_ARG
	);

	/* Activate texture unit */
	if(!GFX_WIND_GET.ext[GFX_EXT_DIRECT_STATE_ACCESS])
		GFX_REND_GET.ActiveTexture(GL_TEXTURE0 + bind);

	if(!old)
	{
		/* Bind sampler */
		GFX_REND_GET.BindSampler(bind, sampler);

		/* Bind texture */
		if(GFX_WIND_GET.ext[GFX_EXT_DIRECT_STATE_ACCESS])
			GFX_REND_GET.BindTextureUnit(bind, texture);
		else
			GFX_REND_GET.BindTexture(target, texture);
	}

	return bind;
}

/******************************************************/
void _gfx_binder_unbind_texture(

		GLuint texture,
		GFX_WIND_ARG)
{
	if(GFX_REND_GET.textureUnits)
	{
		GFX_TextureUnit unit;
		unit.texture = texture;

		_gfx_binder_unbind(
			GFX_REND_GET.textureUnits,
			GFX_WIND_GET.lim[GFX_LIM_MAX_SAMPLER_PROPERTIES],
			sizeof(GFX_TextureUnit),
			offsetof(GFX_TextureUnit, texture),
			sizeof(GLuint),
			&unit
		);
	}
}

/******************************************************/
void _gfx_binder_unbind_sampler(

		GLuint sampler,
		GFX_WIND_ARG)
{
	if(GFX_REND_GET.textureUnits)
	{
		GFX_TextureUnit unit;
		unit.sampler = sampler;

		_gfx_binder_unbind(
			GFX_REND_GET.textureUnits,
			GFX_WIND_GET.lim[GFX_LIM_MAX_SAMPLER_PROPERTIES],
			sizeof(GFX_TextureUnit),
			offsetof(GFX_TextureUnit, sampler),
			sizeof(GLuint),
			&unit
		);
	}
}

/******************************************************/
void _gfx_pipeline_bind(

		GLenum  target,
		GLuint  framebuffer,
		GFX_WIND_ARG)
{
	switch(target)
	{
		/* Bind as both read and draw fbo */
		case GL_FRAMEBUFFER :

			if(
				GFX_REND_GET.fbos[0] != framebuffer ||
				GFX_REND_GET.fbos[1] != framebuffer)
			{
				GFX_REND_GET.BindFramebuffer(GL_FRAMEBUFFER, framebuffer);
				GFX_REND_GET.fbos[0] = framebuffer;
				GFX_REND_GET.fbos[1] = framebuffer;
			}

			break;

		/* Bind as draw fbo */
		case GL_DRAW_FRAMEBUFFER :

			if(GFX_REND_GET.fbos[0] != framebuffer)
			{
				GFX_REND_GET.BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
				GFX_REND_GET.fbos[0] = framebuffer;
			}

			break;

		/* Bind as read fbo */
		case GL_READ_FRAMEBUFFER :

			if(GFX_REND_GET.fbos[1] != framebuffer)
			{
				GFX_REND_GET.BindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
				GFX_REND_GET.fbos[1] = framebuffer;
			}

			break;
	}
}

/******************************************************/
void _gfx_program_map_bind(

		GLuint id,
		GFX_WIND_ARG)
{
	/* Prevent binding it twice */
	if(GFX_REND_GET.program != id)
	{
		GFX_REND_GET.program = id;
		GFX_REND_GET.BindProgramPipeline(id);
	}
}

/******************************************************/
void _gfx_vertex_layout_bind(

		GLuint vao,
		GFX_WIND_ARG)
{
	/* Prevent binding it twice */
	if(GFX_REND_GET.vao != vao)
	{
		GFX_REND_GET.vao = vao;
		GFX_REND_GET.BindVertexArray(vao);
	}
}
