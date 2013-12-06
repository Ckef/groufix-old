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

#include "groufix/containers/vector.h"
#include "groufix/memory/internal.h"
#include "groufix/pipeline/internal.h"

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
	GLuint                program;
	GFX_Internal_Window*  target;
};

/******************************************************/
static int _gfx_pipe_process_create_buffer(void)
{
	if(_gfx_process_buffer) return 1;

	/* Create the buffer */
	float data[] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f
	};
	_gfx_process_buffer = gfx_buffer_create(GFX_BUFFER_WRITE, GFX_VERTEX_BUFFER, sizeof(data), data, 0, 0);

	return _gfx_process_buffer ? 1 : 0;
}

/******************************************************/
static void _gfx_pipe_process_create_layout(GFX_Internal_Window* target)
{
	if(!target->layout)
	{
		GFX_Extensions* ext = &target->extensions;

		/* Make target current */
		GFX_Internal_Window* fallback = _gfx_window_get_current();
		_gfx_window_make_current(target);

		/* Create layout */
		ext->GenVertexArrays(1, &target->layout);
		ext->BindVertexArray(target->layout);
		_gfx_layout_force_rebind();

		ext->BindBuffer(GL_ARRAY_BUFFER, _gfx_buffer_get_handle(_gfx_process_buffer));

		ext->EnableVertexAttribArray(0);
		ext->EnableVertexAttribArray(1);

		ext->VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) << 2, (GLvoid*)0);
		ext->VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) << 2, (GLvoid*)(sizeof(float) << 1));

		/* Switch back to previous window */
		_gfx_window_make_current(fallback);
	}
}

/******************************************************/
void _gfx_pipe_process_untarget(GFX_Internal_Window* target)
{
	if(_gfx_pipes)
	{
		GFXVectorIterator it;
		for(it = _gfx_pipes->begin; it != _gfx_pipes->end; it = gfx_vector_next(_gfx_pipes, it))
		{
			/* Check for equal target, if equal, reset post processing */
			struct GFX_Internal_Process* proc = *(struct GFX_Internal_Process**)it;
			if(target == proc->target)
			{
				proc->program = 0;
				proc->target = NULL;
			}
		}
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

	if(!_gfx_pipe_process_create_buffer())
	{
		free(proc);
		return NULL;
	}

	/* Create vector if it doesn't exist yet */
	if(!_gfx_pipes)
	{
		_gfx_pipes = gfx_vector_create(sizeof(struct GFX_Internal_Process*));
		if(!_gfx_pipes)
		{
			gfx_buffer_free(_gfx_process_buffer);
			_gfx_process_buffer = NULL;

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

			gfx_buffer_free(_gfx_process_buffer);
			_gfx_process_buffer = NULL;
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

			gfx_buffer_free(_gfx_process_buffer);
			_gfx_process_buffer = NULL;
		}
	}
}

/******************************************************/
void* gfx_pipe_process_get_data(GFXPipeProcess* process)
{
	return (void*)(((struct GFX_Internal_Process*)process) + 1);
}

/******************************************************/
void gfx_pipe_process_set_target(GFXPipeProcess* process, GFXProgram* program, GFXWindow* target)
{
	struct GFX_Internal_Process* internal = (struct GFX_Internal_Process*)process;

	if(program && target)
	{
		internal->program = _gfx_program_get_handle(program);
		internal->target = (GFX_Internal_Window*)target;

		/* Create layout */
		_gfx_pipe_process_create_layout(internal->target);
	}
	else
	{
		internal->program = 0;
		internal->target = NULL;
	}
}

/******************************************************/
void _gfx_pipe_process_execute(GFXPipeProcess* process, GFXPipeline* pipeline, GFX_Internal_Window* fallback)
{
	struct GFX_Internal_Process* internal = (struct GFX_Internal_Process*)process;

	/* Execute custom pre-process */
	void* data = (void*)(internal + 1);
	if(process->preprocess) process->preprocess(pipeline, data);

	/* Perform post-processing */
	if(internal->target)
	{
		/* Make target current */
		_gfx_window_make_current(internal->target);

		/* Use given program and draw using the target layout */
		_gfx_program_force_use(internal->program, &internal->target->extensions);
		_gfx_program_force_reuse();

		internal->target->extensions.BindVertexArray(internal->target->layout);
		internal->target->extensions.DrawArrays(GL_TRIANGLE_FAN, 0, 4);
		_gfx_layout_force_rebind();

		/* Switch back to given fallback window */
		_gfx_window_make_current(fallback);
	}

	/* Execute custom post-process */
	if(process->postprocess) process->postprocess(pipeline, data);
}
