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

#include "groufix/core/pipeline/internal.h"
#include "groufix/core/memory/internal.h"
#include "groufix/core/shading/internal.h"
#include "groufix/core/errors.h"

#include <stdlib.h>

/******************************************************/
/* All pipe processes */
static GFXVector* _gfx_pipes = NULL;

/* Shared vertex buffer */
static GLuint _gfx_process_buffer = 0;

/* Internal Pipe Process */
struct GFX_Process
{
	/* Post Processing */
	GFXPropertyMap*  map;
	unsigned int     copy;   /* Copy of the property map to use */
	GFX_Window*      target;
	unsigned char    swap;   /* Whether to swap window buffers or not */

	/* Viewport */
	unsigned int width;
	unsigned int height;
};

/******************************************************/
static inline void _gfx_pipe_process_draw(

		GFXPipeState*    state,
		GFXPropertyMap*  map,
		unsigned int     copy,
		GFX_WIND_ARG)
{
	_gfx_states_set(
		state, GFX_WIND_AS_ARG);
	_gfx_property_map_use(
		map, copy, 0, GFX_WIND_AS_ARG);
	_gfx_vertex_layout_bind(
		GFX_REND_GET.post, GFX_WIND_AS_ARG);

	GFX_REND_GET.DrawArrays(
		GL_TRIANGLE_FAN, 0, 4);
}

/******************************************************/
int _gfx_pipe_process_prepare(void)
{
	GFX_WIND_INIT(0);

	/* First make sure the buffer exists */
	if(!_gfx_process_buffer)
	{
		GLbyte data[] = {
			-1, -1, 0, 0,
			 1, -1, 1, 0,
			 1,  1, 1, 1,
			-1,  1, 0, 1
		};

		GFX_REND_GET.CreateBuffers(
			1, &_gfx_process_buffer);
		GFX_REND_GET.NamedBufferData(
			_gfx_process_buffer, sizeof(data), data, GL_STATIC_DRAW);

		if(!_gfx_process_buffer) return 0;
	}

	/* Next create the layout */
	if(!GFX_REND_GET.post)
	{
		GFX_REND_GET.CreateVertexArrays(
			1, &GFX_REND_GET.post);
		GFX_REND_GET.EnableVertexArrayAttrib(
			GFX_REND_GET.post, 0);

		_gfx_vertex_layout_bind(
			GFX_REND_GET.post, GFX_WIND_AS_ARG);
		GFX_REND_GET.BindBuffer(
			GL_ARRAY_BUFFER, _gfx_process_buffer);
		GFX_REND_GET.VertexAttribIPointer(
			0, 4, GL_BYTE, 0, (GLvoid*)0);

		return GFX_REND_GET.post;
	}

	return 1;
}

/******************************************************/
void _gfx_pipe_process_unprepare(

		int last)
{
	GFX_WIND_INIT();

	/* Reset pipes if not retargeting */
	GFXVectorIterator it;
	if(_gfx_pipes) for(
		it = _gfx_pipes->begin;
		it != _gfx_pipes->end;
		it = gfx_vector_next(_gfx_pipes, it))
	{
		/* Check for equal target, if equal, reset post processing */
		struct GFX_Process* proc = *(struct GFX_Process**)it;
		if(GFX_WIND_EQ(proc->target))
		{
			proc->map = NULL;
			proc->target = NULL;
		}
	}
	if(last)
	{
		/* If last, destroy buffer as well */
		GFX_REND_GET.DeleteBuffers(1, &_gfx_process_buffer);
		_gfx_process_buffer = 0;
	}

	/* Also, destroy layout while we're at it */
	GFX_REND_GET.DeleteVertexArrays(1, &GFX_REND_GET.post);
	GFX_REND_GET.post = 0;
}

/******************************************************/
void _gfx_pipe_process_resize(

		GFX_Window*   target,
		unsigned int  width,
		unsigned int  height)
{
	GFXVectorIterator it;
	if(_gfx_pipes) for(
		it = _gfx_pipes->begin;
		it != _gfx_pipes->end;
		it = gfx_vector_next(_gfx_pipes, it))
	{
		/* Check for equal target, if equal, resize! */
		struct GFX_Process* proc = *(struct GFX_Process**)it;
		if(target == proc->target)
		{
			proc->width = width;
			proc->height = height;
		}
	}
}

/******************************************************/
void _gfx_pipe_process_retarget(

		GFX_Window*  replace,
		GFX_Window*  target)
{
	GFXVectorIterator it;
	if(_gfx_pipes) for(
		it = _gfx_pipes->begin;
		it != _gfx_pipes->end;
		it = gfx_vector_next(_gfx_pipes, it))
	{
		/* Check for equal target, if equal, retarget */
		struct GFX_Process* proc = *(struct GFX_Process**)it;
		if(replace == proc->target) proc->target = target;
	}
}

/******************************************************/
GFXPipeProcess _gfx_pipe_process_create(void)
{
	/* Allocate */
	struct GFX_Process* proc = calloc(1, sizeof(struct GFX_Process));
	if(!proc)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Pipe Process could not be allocated."
		);
		return NULL;
	}

	/* Create vector if it doesn't exist yet */
	if(!_gfx_pipes)
	{
		_gfx_pipes = gfx_vector_create(sizeof(GFXPipeProcess));
		if(!_gfx_pipes)
		{
			free(proc);
			return NULL;
		}
	}

	/* Try to insert */
	GFXVectorIterator it = gfx_vector_insert(
		_gfx_pipes,
		&proc,
		_gfx_pipes->end
	);

	if(it == _gfx_pipes->end)
	{
		if(_gfx_pipes->begin == _gfx_pipes->end)
		{
			gfx_vector_free(_gfx_pipes);
			_gfx_pipes = NULL;
		}
		free(proc);

		return NULL;
	}

	return proc;
}

/******************************************************/
void _gfx_pipe_process_free(

		GFXPipeProcess process)
{
	if(process)
	{
		free(process);

		/* Erase self from vector */
		GFXVectorIterator it;
		for(
			it = _gfx_pipes->begin;
			it != _gfx_pipes->end;
			it = gfx_vector_next(_gfx_pipes, it))
		{
			if(process == *(GFXPipeProcess*)it)
			{
				gfx_vector_erase(_gfx_pipes, it);
				break;
			}
		}

		/* No more processes */
		if(_gfx_pipes->begin == _gfx_pipes->end)
		{
			gfx_vector_free(_gfx_pipes);
			_gfx_pipes = NULL;
		}
	}
}

/******************************************************/
void gfx_pipe_process_set_source(

		GFXPipeProcess   process,
		GFXPropertyMap*  map,
		unsigned int     copy)
{
	struct GFX_Process* internal = (struct GFX_Process*)process;

	internal->map = map;
	internal->copy = copy;
}

/******************************************************/
void gfx_pipe_process_set_target(

		GFXPipeProcess  process,
		GFXWindow*      target,
		int             swap)
{
	struct GFX_Process* internal = (struct GFX_Process*)process;
	internal->target = (GFX_Window*)target;

	/* Set viewport & swap buffers */
	if(target)
	{
		gfx_window_get_size(
			target,
			&internal->width,
			&internal->height
		);

		internal->swap = swap ? 1 : 0;
	}
}

/******************************************************/
void _gfx_pipe_process_execute(

		GFXPipeProcess  process,
		GFXPipeState*   state,
		GFX_WIND_ARG)
{
	struct GFX_Process* internal = (struct GFX_Process*)process;

	/* Perform post-processing */
	if(internal->map)
	{
		if(internal->target)
		{
			GLuint fbo = internal->target->renderer.fbos[0];

			/* Set to new state of window to draw to */
			_gfx_window_make_current(internal->target);

			_gfx_pipeline_bind(
				GL_DRAW_FRAMEBUFFER,
				0,
				GFX_WIND_INT_AS_ARG(internal->target));

			_gfx_states_set_viewport(
				0, 0,
				internal->width,
				internal->height,
				GFX_WIND_INT_AS_ARG(internal->target));

			/* Draw to the window and swap buffers */
			_gfx_pipe_process_draw(
				state,
				internal->map,
				internal->copy,
				GFX_WIND_INT_AS_ARG(internal->target));

			if(internal->swap) _gfx_window_swap_buffers();

			/* Restore previous state */
			_gfx_pipeline_bind(
				GL_DRAW_FRAMEBUFFER,
				fbo,
				GFX_WIND_INT_AS_ARG(internal->target));

			_gfx_window_make_current(&GFX_WIND_GET);
		}

		/* If no windowed rendering, just draw */
		else _gfx_pipe_process_draw(
			state,
			internal->map,
			internal->copy,
			GFX_WIND_AS_ARG
		);
	}
}
