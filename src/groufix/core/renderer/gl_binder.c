
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

#ifdef GFX_RENDERER_GL

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
	GLuint texture; /* Super class */

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

		int            sign,
		unsigned char  min,
		void*          bindings,
		size_t         num,
		size_t         size)
{
	size_t unitSize = sizeof(GFX_Unit) + size;

	/* Iterate and increase */
	while(num--)
	{
		GFX_Unit* unit = (GFX_Unit*)bindings;

		/* Check against minimum and increase according to sign */
		if(unit->counter >= min)
		{
			if(sign >= 0) unit->counter =
				(unit->counter != GFX_BINDER_COUNTER_MAX) ?
				unit->counter + 1 : unit->counter;

			else unit->counter =
				(unit->counter != GFX_BINDER_COUNTER_MIN) ?
				unit->counter - 1 : unit->counter;
		}

		/* Next unit */
		bindings = GFX_PTR_ADD_BYTES(bindings, unitSize);
	}
}

/******************************************************/
static void _gfx_binder_unbind(

		void*        bindings,
		size_t       num,
		size_t       size,
		size_t       cmpSize,
		const void*  cmp)
{
	GFX_Unit* curr = bindings;
	size_t unitSize = sizeof(GFX_Unit) + size;

	/* Iterate */
	size_t i;
	for(i = 0; i < num; ++i)
	{
		/* Empty current unit */
		if(!memcmp(curr + 1, cmp, cmpSize))
		{
			_gfx_binder_increase(
				-1,
				curr->counter,
				bindings,
				num,
				size
			);

			memset(curr, 0, unitSize);
			curr->counter = GFX_BINDER_COUNTER_EMPTY;
		}

		/* Next unit */
		curr = GFX_PTR_ADD_BYTES(curr, unitSize);
	}
}

/******************************************************/
static size_t _gfx_binder_request(

		void*        bindings,
		size_t       num,
		size_t       size,
		const void*  data,
		int          prioritize,
		int*         old)
{
	*old = 0;

	/* First increase all counters */
	if(prioritize) _gfx_binder_increase(
		1, 0,
		bindings,
		num,
		size
	);

	GFX_Unit* pos = NULL;
	size_t unitSize = sizeof(GFX_Unit) + size;

	/* Find highest or equal entry */
	GFX_Unit* high = (GFX_Unit*)bindings;
	GFX_Unit* curr = high;

	while(num--)
	{
		if(!memcmp(data, curr + 1, size))
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
		GFX_BINDER_COUNTER_MIN : GFX_BINDER_COUNTER_MAX;

	memcpy(pos + 1, data, size);

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
	/* Allocate binding points */
	if(!GFX_REND_GET.uniformBuffers)
	{
		GFX_REND_GET.uniformBuffers = _gfx_binder_init(
			GFX_WIND_GET.lim[GFX_LIM_MAX_BUFFER_PROPERTIES],
			sizeof(GFX_UniformBuffer)
		);

		if(!GFX_REND_GET.uniformBuffers) return 0;
	}

	/* Get unit to bind it to */
	GFX_UniformBuffer buff =
	{
		.buffer = buffer,
		.offset = offset,
		.size = size
	};

	int old;
	size_t bind = _gfx_binder_request(
		GFX_REND_GET.uniformBuffers,
		GFX_WIND_GET.lim[GFX_LIM_MAX_BUFFER_PROPERTIES],
		sizeof(GFX_UniformBuffer),
		&buff,
		prioritize,
		&old
	);

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
			sizeof(GLuint),
			&buff
		);
	}
}

/******************************************************/
size_t _gfx_binder_bind_texture(

		GLuint  texture,
		GLenum  target,
		int     prioritize,
		GFX_WIND_ARG)
{
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
	GFX_TextureUnit unit;
	unit.texture = texture;

	int old;
	size_t bind = _gfx_binder_request(
		GFX_REND_GET.textureUnits,
		GFX_WIND_GET.lim[GFX_LIM_MAX_SAMPLER_PROPERTIES],
		sizeof(GFX_TextureUnit),
		&unit,
		prioritize,
		&old
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
			sizeof(GLuint),
			&unit
		);
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

#endif // GFX_RENDERER_GL
