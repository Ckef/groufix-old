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

#include "groufix/containers/deque.h"
#include "groufix/pipeline/internal.h"
#include "groufix/internal.h"

#include <stdlib.h>

/******************************************************/
/** Internal Pipe */
struct GFX_Internal_Pipe
{
	GFXPipeProcessFunc  process;
	GFXPipeState        state;
	GFXPipe             pipe;
};

/** Internal Pipeline */
struct GFX_Internal_Pipeline
{
	/* Super class */
	GFXPipeline pipeline;

	/* Hidden data */
	GLuint    fbo;   /* OpenGL handle */
	GFXDeque  pipes; /* Stores GFX_Internal_Pipe */

	/* Not a shared resource */
	GFX_Extensions* ext;
};

/******************************************************/
static void _gfx_pipe_free(struct GFX_Internal_Pipe* pipe)
{
	if(pipe->process) free(pipe->pipe.data);
	else _gfx_bucket_free(pipe->pipe.bucket);
}

/******************************************************/
static int _gfx_pipe_create_bucket(struct GFX_Internal_Pipe* pipe, unsigned char bits, GFXBatchProcessFunc process)
{
	/* Create bucket */
	GFXBucket* bucket = _gfx_bucket_create(bits, process);
	if(!bucket) return 0;

	/* Fill the pipe */
	pipe->process = NULL;
	pipe->state = GFX_STATE_DEFAULT;
	pipe->pipe.bucket = bucket;

	return 1;
}

/******************************************************/
static int _gfx_pipe_create_process(struct GFX_Internal_Pipe* pipe, GFXPipeProcessFunc process, size_t dataSize)
{
	/* Allocate data */
	void* data = NULL;
	if(dataSize)
	{
		data = malloc(dataSize);
		if(!data) return 0;
	}

	/* Fill the pipe */
	pipe->process = process;
	pipe->state = GFX_STATE_DEFAULT;
	pipe->pipe.data = data;

	return 1;
}

/******************************************************/
static void _gfx_pipeline_set_pipe(struct GFX_Internal_Pipeline* pipeline, struct GFX_Internal_Pipe pipe, unsigned short index)
{
	/* Free previous pipe & replace with new */
	struct GFX_Internal_Pipe* p = (struct GFX_Internal_Pipe*)gfx_deque_at(&pipeline->pipes, index - 1);

	_gfx_pipe_free(p);
	*p = pipe;
}

/******************************************************/
static GFXPipeState _gfx_pipeline_get_state(struct GFX_Internal_Pipeline* pipeline, unsigned short index)
{
	/* Return default state if index 0 */
	if(!index) return GFX_STATE_DEFAULT;

	return ((struct GFX_Internal_Pipe*)gfx_deque_at(&pipeline->pipes, index - 1))->state;
}

/******************************************************/
static void _gfx_pipeline_obj_free(void* object, GFX_Extensions* ext)
{
	struct GFX_Internal_Pipeline* pipeline = (struct GFX_Internal_Pipeline*)object;

	/* Destroy framebuffer */
	ext->DeleteFramebuffers(1, &pipeline->fbo);

	pipeline->ext = NULL;
	pipeline->fbo = 0;
	pipeline->pipeline.id = 0;
}

/******************************************************/
static void _gfx_pipeline_obj_save(void* object, GFX_Extensions* ext)
{
	struct GFX_Internal_Pipeline* pipeline = (struct GFX_Internal_Pipeline*)object;

	/* Destroy framebuffer */
	ext->DeleteFramebuffers(1, &pipeline->fbo);

	pipeline->ext = NULL;
	pipeline->fbo = 0;
}

/******************************************************/
static void _gfx_pipeline_obj_restore(void* object, GFX_Extensions* ext)
{
	struct GFX_Internal_Pipeline* pipeline = (struct GFX_Internal_Pipeline*)object;

	/* Create FBO */
	pipeline->ext = ext;
	ext->GenFramebuffers(1, &pipeline->fbo);
}

/******************************************************/
/* vtable for hardware part of the pipeline */
static GFX_Hardware_Funcs _gfx_pipeline_obj_funcs =
{
	_gfx_pipeline_obj_free,
	_gfx_pipeline_obj_save,
	_gfx_pipeline_obj_restore
};

/******************************************************/
GLuint _gfx_pipeline_get_handle(const GFXPipeline* pipeline)
{
	return ((struct GFX_Internal_Pipeline*)pipeline)->fbo;
}

/******************************************************/
GFXPipeline* gfx_pipeline_create(void)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Create new pipeline */
	struct GFX_Internal_Pipeline* pl = malloc(sizeof(struct GFX_Internal_Pipeline));
	if(!pl) return NULL;

	/* Register as object */
	pl->pipeline.id = _gfx_hardware_object_register(pl, &_gfx_pipeline_obj_funcs);
	if(!pl->pipeline.id)
	{
		free(pl);
		return NULL;
	}

	/* Create OpenGL resources */
	pl->ext = &window->extensions;
	pl->ext->GenFramebuffers(1, &pl->fbo);

	gfx_deque_init(&pl->pipes, sizeof(struct GFX_Internal_Pipe));

	return (GFXPipeline*)pl;
}

/******************************************************/
void gfx_pipeline_free(GFXPipeline* pipeline)
{
	if(pipeline)
	{
		struct GFX_Internal_Pipeline* internal = (struct GFX_Internal_Pipeline*)pipeline;

		/* Unregister as object */
		_gfx_hardware_object_unregister(pipeline->id);

		/* Free all pipes */
		GFXDequeIterator it;
		for(it = internal->pipes.begin; it != internal->pipes.end; it = gfx_deque_next(&internal->pipes, it))
			_gfx_pipe_free((struct GFX_Internal_Pipe*)it);

		/* Delete FBO */
		if(internal->ext) internal->ext->DeleteFramebuffers(1, &internal->fbo);

		gfx_deque_clear(&internal->pipes);
		free(pipeline);
	}
}

/******************************************************/
unsigned short gfx_pipeline_push_bucket(GFXPipeline* pipeline, unsigned char bits, GFXBatchProcessFunc process)
{
	struct GFX_Internal_Pipeline* internal = (struct GFX_Internal_Pipeline*)pipeline;

	/* Check for overflow (so many pipes, unbelievable!) */
	unsigned short index = gfx_deque_get_size(&internal->pipes) + 1;
	if(!index) return 0;

	/* Create pipe */
	struct GFX_Internal_Pipe pipe;

	if(!_gfx_pipe_create_bucket(&pipe, bits, process)) return 0;
	pipe.state = _gfx_pipeline_get_state(internal, index - 1);

	/* Insert and return actual index + 1 */
	if(gfx_deque_push_back(&internal->pipes, &pipe) == internal->pipes.end)
	{
		_gfx_pipe_free(&pipe);
		return 0;
	}
	return index;
}

/******************************************************/
unsigned short gfx_pipeline_push_process(GFXPipeline* pipeline, GFXPipeProcessFunc process, size_t dataSize)
{
	struct GFX_Internal_Pipeline* internal = (struct GFX_Internal_Pipeline*)pipeline;

	/* Check for overflow (moar pipes!) */
	unsigned short index = gfx_deque_get_size(&internal->pipes) + 1;
	if(!index) return 0;

	/* Create pipe */
	struct GFX_Internal_Pipe pipe;

	if(!_gfx_pipe_create_process(&pipe, process, dataSize)) return 0;
	pipe.state = _gfx_pipeline_get_state(internal, index - 1);

	/* Insert and return actual index + 1 */
	if(gfx_deque_push_back(&internal->pipes, &pipe) == internal->pipes.end)
	{
		_gfx_pipe_free(&pipe);
		return 0;
	}
	return index;
}

/******************************************************/
int gfx_pipeline_set_bucket(GFXPipeline* pipeline, unsigned short index, unsigned char bits, GFXBatchProcessFunc process)
{
	struct GFX_Internal_Pipeline* internal = (struct GFX_Internal_Pipeline*)pipeline;

	/* Validate index */
	size_t size = gfx_deque_get_size(&internal->pipes);
	if(!index || index > size) return 0;

	/* Create new pipe */
	struct GFX_Internal_Pipe pipe;
	if(!_gfx_pipe_create_bucket(&pipe, bits, process)) return 0;

	pipe.state = _gfx_pipeline_get_state(internal, index);
	_gfx_pipeline_set_pipe(internal, pipe, index);

	return 1;
}

/******************************************************/
int gfx_pipeline_set_process(GFXPipeline* pipeline, unsigned short index, GFXPipeProcessFunc process, size_t dataSize)
{
	struct GFX_Internal_Pipeline* internal = (struct GFX_Internal_Pipeline*)pipeline;

	/* Validate index */
	size_t size = gfx_deque_get_size(&internal->pipes);
	if(!index || index > size) return 0;

	/* Create new pipe */
	struct GFX_Internal_Pipe pipe;
	if(!_gfx_pipe_create_process(&pipe, process, dataSize)) return 0;

	pipe.state = _gfx_pipeline_get_state(internal, index);
	_gfx_pipeline_set_pipe(internal, pipe, index);

	return 1;
}

/******************************************************/
int gfx_pipeline_set_state(GFXPipeline* pipeline, unsigned short index, GFXPipeState state)
{
	struct GFX_Internal_Pipeline* internal = (struct GFX_Internal_Pipeline*)pipeline;

	/* Validate index */
	size_t size = gfx_deque_get_size(&internal->pipes);
	if(!index || index > size) return 0;

	/* Set state of pipe */
	((struct GFX_Internal_Pipe*)gfx_deque_at(&internal->pipes, index - 1))->state = state;

	return 1;
}

/******************************************************/
int gfx_pipeline_get(GFXPipeline* pipeline, unsigned short index, GFXPipeType* type, GFXPipeState* state, GFXPipe* pipe)
{
	struct GFX_Internal_Pipeline* internal = (struct GFX_Internal_Pipeline*)pipeline;

	/* Validate index */
	size_t size = gfx_deque_get_size(&internal->pipes);
	if(!index || index > size) return 0;

	/* Retrieve data */
	struct GFX_Internal_Pipe* p = (struct GFX_Internal_Pipe*)gfx_deque_at(&internal->pipes, index - 1);

	if(type) *type = p->process ? GFX_PIPE_PROCESS : GFX_PIPE_BUCKET;
	if(state) *state = p->state;
	if(pipe) *pipe = p->pipe;

	return 1;
}

/******************************************************/
unsigned short gfx_pipeline_pop(GFXPipeline* pipeline)
{
	struct GFX_Internal_Pipeline* internal = (struct GFX_Internal_Pipeline*)pipeline;

	/* Get index of last call */
	unsigned short index = gfx_deque_get_size(&internal->pipes);

	/* Free the pipe */
	_gfx_pipe_free((struct GFX_Internal_Pipe*)gfx_deque_at(&internal->pipes, index - 1));

	/* Try to pop the last element */
	gfx_deque_pop_back(&internal->pipes);

	return index;
}

/******************************************************/
void gfx_pipeline_execute(GFXPipeline* pipeline)
{
	struct GFX_Internal_Pipeline* internal = (struct GFX_Internal_Pipeline*)pipeline;
	if(!internal->ext) return;

	/* Bind as framebuffer */
	internal->ext->BindFramebuffer(GL_FRAMEBUFFER, internal->fbo);

	/* Iterate over all pipes */
	GFXDequeIterator it;
	for(it = internal->pipes.begin; it != internal->pipes.end; it = gfx_deque_next(&internal->pipes, it))
	{
		struct GFX_Internal_Pipe* pipe = (struct GFX_Internal_Pipe*)it;

		/* Set states */
		_gfx_states_set(pipe->state, internal->ext);

		/* Process pipe */
		if(pipe->process) pipe->process(pipe->pipe.data);
		else _gfx_bucket_process(pipe->pipe.bucket);
	}
}
