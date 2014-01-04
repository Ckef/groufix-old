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
#include "groufix/memory/internal.h"
#include "groufix/pipeline/internal.h"
#include "groufix/shading/internal.h"

#include <stdlib.h>

/******************************************************/
/* All pipe processes */
static GFXVector* _gfx_pipes = NULL;

/* Shared vertex buffer */
static GFXBuffer* _gfx_process_buffer = NULL;

/* Internal Pipe Process */
struct GFX_Internal_Process
{
	/* Super class */
	GFXPipeProcess process;

	/* Post Processing */
	GFXPropertyMap*       map;
	GFX_Internal_Window*  target;
};

/******************************************************/
static inline void _gfx_pipe_process_draw(GFXPipeState state, GFXPropertyMap* map, GLuint layout, GFX_Extensions* ext)
{
	_gfx_states_set(state, ext);
	_gfx_property_map_use(map, ext);
	_gfx_layout_bind(layout, ext);

	ext->DrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

/******************************************************/
int _gfx_pipe_process_prepare(GFX_Internal_Window* target)
{
	/* First make sure the buffer exists */
	if(!_gfx_process_buffer)
	{
		float data[] = {
			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 1.0f
		};
		_gfx_process_buffer = gfx_buffer_create(GFX_BUFFER_WRITE, GFX_VERTEX_BUFFER, sizeof(data), data, 0, 0);

		if(!_gfx_process_buffer) return 0;
	}

	/* Next create the layout */
	if(!target->layout)
	{
		GFX_Extensions* ext = &target->extensions;

		ext->GenVertexArrays(1, &target->layout);
		_gfx_layout_bind(target->layout, ext);

		ext->BindBuffer(GL_ARRAY_BUFFER, _gfx_buffer_get_handle(_gfx_process_buffer));

		ext->EnableVertexAttribArray(0);
		ext->EnableVertexAttribArray(1);

		ext->VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) << 2, (GLvoid*)0);
		ext->VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) << 2, (GLvoid*)(sizeof(float) << 1));

		return target->layout ? 1 : 0;
	}

	return 1;
}

/******************************************************/
void _gfx_pipe_process_retarget(GFX_Internal_Window* replace, GFX_Internal_Window* target)
{
	GFXVectorIterator it;
	if(_gfx_pipes) for(it = _gfx_pipes->begin; it != _gfx_pipes->end; it = gfx_vector_next(_gfx_pipes, it))
	{
		/* Check for equal target, if equal, retarget */
		struct GFX_Internal_Process* proc = *(struct GFX_Internal_Process**)it;
		if(replace == proc->target) proc->target = target;
	}
}

/******************************************************/
void _gfx_pipe_process_untarget(GFX_Internal_Window* target, int last)
{
	/* Reset pipes if not retargeting */
	GFXVectorIterator it;
	if(_gfx_pipes) for(it = _gfx_pipes->begin; it != _gfx_pipes->end; it = gfx_vector_next(_gfx_pipes, it))
	{
		/* Check for equal target, if equal, reset post processing */
		struct GFX_Internal_Process* proc = *(struct GFX_Internal_Process**)it;
		if(target == proc->target)
		{
			proc->map = NULL;
			proc->target = NULL;
		}
	}
	if(last)
	{
		/* If last, destroy buffer as well */
		gfx_buffer_free(_gfx_process_buffer);
		_gfx_process_buffer = NULL;
	}

	/* Also, destroy layout while we're at it */
	target->extensions.DeleteVertexArrays(1, &target->layout);
	target->layout = 0;
}

/******************************************************/
GFXPipeProcess* _gfx_pipe_process_create(size_t dataSize)
{
	/* Allocate */
	struct GFX_Internal_Process* proc = calloc(1, sizeof(struct GFX_Internal_Process) + dataSize);
	if(!proc) return NULL;

	/* Create vector if it doesn't exist yet */
	if(!_gfx_pipes)
	{
		_gfx_pipes = gfx_vector_create(sizeof(struct GFX_Internal_Process*));
		if(!_gfx_pipes)
		{
			free(proc);
			return NULL;
		}
	}

	/* Try to insert */
	if(gfx_vector_insert(_gfx_pipes, &proc, _gfx_pipes->end) == _gfx_pipes->end)
	{
		if(_gfx_pipes->begin == _gfx_pipes->end)
		{
			gfx_vector_free(_gfx_pipes);
			_gfx_pipes = NULL;
		}
		free(proc);

		return NULL;
	}

	return (GFXPipeProcess*)proc;
}

/******************************************************/
void _gfx_pipe_process_free(GFXPipeProcess* process)
{
	if(process)
	{
		free(process);

		/* Erase self from vector */
		GFXVectorIterator it;
		for(it = _gfx_pipes->begin; it != _gfx_pipes->end; it = gfx_vector_next(_gfx_pipes, it))
			if(process == *(GFXPipeProcess**)it)
			{
				gfx_vector_erase(_gfx_pipes, it);
				break;
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
void* gfx_pipe_process_get_data(GFXPipeProcess* process)
{
	return (void*)(((struct GFX_Internal_Process*)process) + 1);
}

/******************************************************/
void gfx_pipe_process_set_source(GFXPipeProcess* process, GFXPropertyMap* map)
{
	((struct GFX_Internal_Process*)process)->map = map;
}

/******************************************************/
void gfx_pipe_process_set_target(GFXPipeProcess* process, GFXWindow* target)
{
	((struct GFX_Internal_Process*)process)->target = (GFX_Internal_Window*)target;
}

/******************************************************/
void _gfx_pipe_process_execute(GFXPipeProcess* process, GFXPipeline* pipeline, GFXPipeState state, GFX_Internal_Window* active)
{
	struct GFX_Internal_Process* internal = (struct GFX_Internal_Process*)process;

	/* Execute custom pre-process */
	void* data = (void*)(internal + 1);
	if(process->preprocess) process->preprocess(pipeline, data);

	/* Perform post-processing */
	if(internal->map)
	{
		if(internal->target)
		{
			GFX_Extensions* ext = &internal->target->extensions;
			GLuint fbo = ext->pipeline;

			/* Make target current, draw, and switch back to previously active */
			_gfx_window_make_current(internal->target);
			_gfx_pipeline_bind(0, ext);

			_gfx_pipe_process_draw(state, internal->map, internal->target->layout, ext);

			_gfx_pipeline_bind(fbo, ext);
			_gfx_window_make_current(active);
		}

		/* If no windowed rendering, just draw */
		else _gfx_pipe_process_draw(state, internal->map, active->layout, &active->extensions);
	}

	/* Execute custom post-process */
	if(process->postprocess) process->postprocess(pipeline, data);
}
