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

#define GFX_INT_UCHAR_MSB ~(~((unsigned char)0) >> 1)

/******************************************************/
struct GFX_Callback
{
	GFXPipeCallback      callback;
	GFXPipeCallbackFunc  func;
};

/******************************************************/
static inline void _gfx_pipe_swap_callbacks(struct GFX_Callback* first, struct GFX_Callback* second)
{
	struct GFX_Callback temp = *first;
	*first = *second;
	*second = temp;
}

/******************************************************/
static void _gfx_pipe_sort(GFX_Pipe* pipe)
{
	/* TODO: Everything */
}

/******************************************************/
static GFXVectorIterator _gfx_pipe_find(GFX_Pipe* pipe, GFXPipeCallback callback)
{
	/* TODO: Everything, again */

	return NULL;
}

/******************************************************/
static GFX_Pipe* _gfx_pipe_create(GFXPipeType type, GFXPipeline* pipeline)
{
	/* Create the pipe */
	GFX_Pipe* pipe = (GFX_Pipe*)gfx_list_create(sizeof(GFX_Pipe));
	if(!pipe) return NULL;

	pipe->type = type;
	pipe->pipeline = pipeline;

	gfx_vector_init(&pipe->callbacks, sizeof(struct GFX_Callback));

	return pipe;
}

/******************************************************/
GFX_Pipe* _gfx_pipe_create_bucket(GFXPipeline* pipeline, unsigned char bits, GFXBucketFlags flags)
{
	GFX_Pipe* pipe = _gfx_pipe_create(GFX_PIPE_BUCKET, pipeline);
	if(!pipe) return NULL;

	/* Create bucket */
	GFXBucket* bucket = _gfx_bucket_create(bits, flags);
	if(!bucket)
	{
		_gfx_pipe_free(pipe);
		return 0;
	}
	pipe->pipe.bucket = bucket;

	return pipe;
}

/******************************************************/
GFX_Pipe* _gfx_pipe_create_process(GFXPipeline* pipeline)
{
	GFX_Pipe* pipe = _gfx_pipe_create(GFX_PIPE_PROCESS, pipeline);
	if(!pipe) return NULL;

	/* Allocate process */
	GFXPipeProcess process = _gfx_pipe_process_create();
	if(!process)
	{
		_gfx_pipe_free(pipe);
		return 0;
	}
	pipe->pipe.process = process;

	return pipe;
}

/******************************************************/
GFX_Pipe* _gfx_pipe_free(GFX_Pipe* pipe)
{
	/* Issue all callbacks */
	GFXVectorIterator it;
	for(it = pipe->callbacks.begin; it != pipe->callbacks.end; it = gfx_vector_next(&pipe->callbacks, it))
	{
		struct GFX_Callback* call = (struct GFX_Callback*)it;
		call->func(&pipe->pipe, &call->callback);
	}

	gfx_vector_clear(&pipe->callbacks);

	/* Free the actual pipe */
	switch(pipe->type)
	{
		case GFX_PIPE_BUCKET :
			_gfx_bucket_free(pipe->pipe.bucket);
			break;

		case GFX_PIPE_PROCESS :
			_gfx_pipe_process_free(pipe->pipe.process);
			break;
	}

	return (GFX_Pipe*)gfx_list_erase((GFXList*)pipe);
}

/******************************************************/
GFXPipeType gfx_pipe_get_type(GFXPipe* pipe)
{
	return ((GFX_Pipe*)GFX_PTR_SUB_BYTES(pipe, offsetof(GFX_Pipe, pipe)))->type;
}

/******************************************************/
GFXPipeState gfx_pipe_get_state(GFXPipe* pipe)
{
	return ((GFX_Pipe*)GFX_PTR_SUB_BYTES(pipe, offsetof(GFX_Pipe, pipe)))->state;
}

/******************************************************/
void gfx_pipe_set_state(GFXPipe* pipe, GFXPipeState state)
{
	((GFX_Pipe*)GFX_PTR_SUB_BYTES(pipe, offsetof(GFX_Pipe, pipe)))->state = state;
}

/******************************************************/
int gfx_pipe_register(GFXPipe* pipe, GFXPipeCallback callback, GFXPipeCallbackFunc func)
{
	GFX_Pipe* internal = GFX_PTR_SUB_BYTES(pipe, offsetof(GFX_Pipe, pipe));

	/* Insert the callback object */
	struct GFX_Callback call;
	call.callback = callback;
	call.func = func;

	if(gfx_vector_insert(&internal->callbacks, &call, internal->callbacks.end) == internal->callbacks.end) return 0;
	internal->sorted = 0;

	return 1;
}

/******************************************************/
void gfx_pipe_unregister(GFXPipe* pipe, GFXPipeCallback callback)
{
	GFX_Pipe* internal = GFX_PTR_SUB_BYTES(pipe, offsetof(GFX_Pipe, pipe));

	/* Sort if necessary */
	if(!internal->sorted)
	{
		_gfx_pipe_sort(internal);
		internal->sorted = 1;
	}

	/* Find the callback object and erase it */
	GFXVectorIterator it = _gfx_pipe_find(internal, callback);
	if(it) gfx_vector_erase(&internal->callbacks, it);
}
