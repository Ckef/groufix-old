/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/internal.h"

#include <stdlib.h>

/******************************************************/
/** Internal Pipe Process */
typedef struct GFX_Process
{
	GFXProgramMap*   progs;
	GFXPropertyMap*  map;
	unsigned int     copy;    /* Copy of the property map to use */

	GFX_Context*     target;
	unsigned char    swap;    /* Whether to swap buffers or not */
	GFXViewport      viewport;

} GFX_Process;


/** All pipe processes */
static GFXVector* _gfx_pipes = NULL;


/** Shared vertex buffer */
static GLuint _gfx_process_buffer = 0;


/******************************************************/
static inline void _gfx_pipe_process_draw(

		const GFXPipeState*    state,
		const GFXPropertyMap*  map,
		unsigned int           copy,
		GFX_CONT_ARG)
{
	_gfx_states_set(
		state, GFX_CONT_AS_ARG);
	_gfx_property_map_use(
		map, copy, 0, GFX_CONT_AS_ARG);
	_gfx_gl_vertex_layout_bind(
		GFX_REND_GET.post, GFX_CONT_AS_ARG);

	GFX_REND_GET.DrawArrays(
		GL_TRIANGLE_FAN, 0, 4);
}

/******************************************************/
int _gfx_pipe_process_prepare(void)
{
	GFX_CONT_INIT(0);

	/* First make sure the buffer exists */
	if(!_gfx_process_buffer)
	{
		GLbyte data[] = {
			-1, -1,  0,  0,
			 1, -1,  1,  0,
			 1,  1,  1,  1,
			-1,  1,  0,  1
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

		_gfx_gl_vertex_layout_bind(
			GFX_REND_GET.post, GFX_CONT_AS_ARG);
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
	GFX_CONT_INIT();

	/* Reset pipes if not retargeting */
	GFXVectorIterator it;
	if(_gfx_pipes) for(
		it = _gfx_pipes->begin;
		it != _gfx_pipes->end;
		it = gfx_vector_next(_gfx_pipes, it))
	{
		/* Check for equal target, if equal, reset post processing */
		GFX_Process* proc = *(GFX_Process**)it;
		if(GFX_CONT_EQ(proc->target))
		{
			gfx_property_map_free(proc->map);
			gfx_program_map_free(proc->progs);

			proc->progs = NULL;
			proc->map = NULL;
			proc->target = NULL;
		}
	}

	/* Also, destroy layout while we're at it */
	GFX_REND_GET.DeleteVertexArrays(1, &GFX_REND_GET.post);
	GFX_REND_GET.post = 0;

	if(last)
	{
		/* If last, destroy buffer as well */
		GFX_REND_GET.DeleteBuffers(1, &_gfx_process_buffer);
		_gfx_process_buffer = 0;
	}
}

/******************************************************/
void _gfx_pipe_process_retarget(

		GFX_Context* target)
{
	GFX_CONT_INIT();

	if(!GFX_CONT_EQ(target) && _gfx_pipes)
	{
		/* First remove resources from current context */
		GFXVectorIterator it;
		for(
			it = _gfx_pipes->begin;
			it != _gfx_pipes->end;
			it = gfx_vector_next(_gfx_pipes, it))
		{
			GFX_Process* proc = *(GFX_Process**)it;
			if(GFX_CONT_EQ(proc->target))
				_gfx_program_map_save(proc->progs);
		}

		/* Then restore them to the new target */
		_gfx_context_make_current(target);

		for(
			it = _gfx_pipes->begin;
			it != _gfx_pipes->end;
			it = gfx_vector_next(_gfx_pipes, it))
		{
			GFX_Process* proc = *(GFX_Process**)it;
			if(GFX_CONT_EQ(proc->target))
			{
				/* Also make sure to set the target */
				proc->target = target;
				_gfx_program_map_restore(proc->progs, &target->objects);
			}
		}

		/* And back to the old context */
		_gfx_context_make_current(&GFX_CONT_GET);
	}
}

/******************************************************/
void _gfx_pipe_process_resize(

		const GFX_Context*  target,
		unsigned int        width,
		unsigned int        height)
{
	GFXVectorIterator it;
	if(_gfx_pipes) for(
		it = _gfx_pipes->begin;
		it != _gfx_pipes->end;
		it = gfx_vector_next(_gfx_pipes, it))
	{
		/* Check for equal target, if equal, resize! */
		GFX_Process* proc = *(GFX_Process**)it;
		if(target == proc->target)
		{
			proc->viewport.width = width;
			proc->viewport.height = height;
		}
	}
}

/******************************************************/
GFXPipeProcess _gfx_pipe_process_create(

		GFXWindow*  target,
		int         swap)
{
	GFX_CONT_INIT(NULL);

	/* Allocate */
	GFX_Process* proc = calloc(1, sizeof(GFX_Process));
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

	/* Create context-bound objects */
	if(target)
	{
		gfx_window_get_size(
			target,
			&proc->viewport.width,
			&proc->viewport.height
		);

		proc->target = (GFX_Context*)target;
		proc->swap = swap ? proc->target->swap : 0;

		_gfx_context_make_current(proc->target);
	}

	proc->progs = gfx_program_map_create();

	if(proc->progs)
	{
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

			gfx_program_map_free(proc->progs);
			free(proc);

			proc = NULL;
		}
	}
	else
	{
		free(proc);
		proc = NULL;
	}

	/* Back to previous context */
	_gfx_context_make_current(&GFX_CONT_GET);

	return proc;
}

/******************************************************/
void _gfx_pipe_process_free(

		GFXPipeProcess process)
{
	if(process)
	{
		GFX_CONT_INIT_UNSAFE;

		GFX_Process* internal = (GFX_Process*)process;

		/* Free context-bound objects */
		if(internal->target)
			_gfx_context_make_current(internal->target);

		gfx_property_map_free(internal->map);
		gfx_program_map_free(internal->progs);

		if(GFX_CONT_EQ(NULL))
			_gfx_context_make_current(NULL);
		else
			_gfx_context_make_current(&GFX_CONT_GET);

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

		free(process);
	}
}

/******************************************************/
void _gfx_pipe_process_execute(

		GFXPipeProcess       process,
		const GFXPipeState*  state,
		GFX_CONT_ARG)
{
	GFX_Process* internal = (GFX_Process*)process;

	if(!internal->map) return;

	/* Perform post-processing */
	if(internal->target)
	{
		GLuint fbo = GFX_REND_GET.fbos[0];
		GFXViewport vp = GFX_REND_GET.viewport;

		_gfx_context_make_current(internal->target);

		/* Set to new state of window to draw to */
		_gfx_gl_pipeline_bind(
			GL_DRAW_FRAMEBUFFER,
			0,
			GFX_CONT_INT_AS_ARG(internal->target));

		_gfx_states_set_viewport(
			internal->viewport,
			GFX_CONT_INT_AS_ARG(internal->target));

		/* Draw to the window */
		_gfx_pipe_process_draw(
			state,
			internal->map,
			internal->copy,
			GFX_CONT_INT_AS_ARG(internal->target));

		/* Swap buffers */
		/* If not swapping, at least flush all commands */
		if(internal->swap)
			_gfx_context_swap_buffers();
		else
			GFX_REND_GET.Flush();

		/* Restore previous state if main context */
		if(GFX_CONT_EQ(internal->target))
		{
			_gfx_gl_pipeline_bind(
				GL_DRAW_FRAMEBUFFER,
				fbo,
				GFX_CONT_AS_ARG);

			_gfx_states_set_viewport(
				vp,
				GFX_CONT_AS_ARG);
		}

		/* Restore context if not main context */
		else _gfx_context_make_current(&GFX_CONT_GET);
	}

	/* If no windowed rendering, just draw */
	else _gfx_pipe_process_draw(
		state,
		internal->map,
		internal->copy,
		GFX_CONT_AS_ARG
	);
}

/******************************************************/
GFXPropertyMap* gfx_pipe_process_get_map(

		GFXPipeProcess  process,
		unsigned char   properties)
{
	GFX_Process* internal = (GFX_Process*)process;

	if(!internal->map)
	{
		GFX_CONT_INIT(NULL);

		/* Make the target current so the program map will block properly */
		if(internal->target)
			_gfx_context_make_current(internal->target);

		/* And create the property map */
		internal->map = gfx_property_map_create(
			internal->progs,
			properties
		);

		/* Back to old context */
		_gfx_context_make_current(&GFX_CONT_GET);
	}

	return internal->map;
}

/******************************************************/
void gfx_pipe_process_set_copy(

		GFXPipeProcess  process,
		unsigned int    copy)
{
	((GFX_Process*)process)->copy = copy;
}

/******************************************************/
GFXProgram* gfx_pipe_process_add(

		GFXPipeProcess  process,
		GFXShaderStage  stage,
		size_t          instances)
{
	return gfx_program_map_add(
		((GFX_Process*)process)->progs,
		stage,
		instances
	);
}

/******************************************************/
int gfx_pipe_process_add_share(

		GFXPipeProcess  process,
		GFXShaderStage  stage,
		GFXProgram*     share)
{
	return gfx_program_map_add_share(
		((GFX_Process*)process)->progs,
		stage,
		share
	);
}

/******************************************************/
GFXProgram* gfx_pipe_process_get(

		const GFXPipeProcess  process,
		GFXShaderStage        stage)
{
	return gfx_program_map_get(
		((GFX_Process*)process)->progs,
		stage
	);
}
