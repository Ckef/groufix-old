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

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Internal callback */
struct GFX_Callback
{
	GFXPipeCallback      callback; /* Super class */
	GFXPipeCallbackFunc  func;
};

/******************************************************/
static int _gfx_pipe_callback_comp(const void* elem1, const void* elem2)
{
	GFXPipeCallback* c1 = (GFXPipeCallback*)elem1;
	GFXPipeCallback* c2 = (GFXPipeCallback*)elem2;

	if(c1->key < c2->key) return -1;
	if(c1->key > c2->key) return 1;

	if(GFX_PTR_LESS(c1->data, c2->data)) return -1;
	if(GFX_PTR_LESS(c2->data, c1->data)) return 1;

	return 0;
}

/******************************************************/
static int _gfx_pipe_callback_comp_key(const void* key, const void* elem)
{
	unsigned char k = GFX_VOID_TO_UINT(key);
	GFXPipeCallback* call = (GFXPipeCallback*)elem;

	if(call->key > k) return -1;
	if(call->key < k) return 1;

	return 0;
}

/******************************************************/
static void _gfx_pipe_sort(GFX_Pipe* pipe)
{
	/* Sort if necessary */
	if(!pipe->sorted)
	{
		qsort(
			pipe->callbacks.begin,
			gfx_vector_get_size(&pipe->callbacks),
			sizeof(struct GFX_Callback),
			_gfx_pipe_callback_comp
		);
		pipe->sorted = 1;
	}
}

/******************************************************/
static GFXVectorIterator _gfx_pipe_find(GFX_Pipe* pipe, unsigned char key, size_t* num)
{
	/* Find first an callback with the key*/
	GFXVectorIterator it = bsearch(
		GFX_UINT_TO_VOID(key),
		pipe->callbacks.begin,
		gfx_vector_get_size(&pipe->callbacks),
		sizeof(struct GFX_Callback),
		_gfx_pipe_callback_comp_key
	);

	if(!it)
	{
		*num = 0;
		return NULL;
	}

	/* Find first callback with the key */
	GFXVectorIterator first = it;
	while(first != pipe->callbacks.begin)
	{
		GFXVectorIterator prev = gfx_vector_previous(&pipe->callbacks, first);
		if(((GFXPipeCallback*)prev)->key != key) break;

		first = prev;
	}

	/* Find last callback with the key */
	GFXVectorIterator last = gfx_vector_next(&pipe->callbacks, it);
	while(last != pipe->callbacks.end)
	{
		if(((GFXPipeCallback*)last)->key != key) break;
		last = gfx_vector_next(&pipe->callbacks, last);
	}

	/* Return the range */
	*num = gfx_vector_get_index(&pipe->callbacks, last) - gfx_vector_get_index(&pipe->callbacks, first);
	return first;
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
	/* Sort if necessary */
	GFX_Pipe* internal = GFX_PTR_SUB_BYTES(pipe, offsetof(GFX_Pipe, pipe));
	_gfx_pipe_sort(internal);

	/* Find the callback object */
	GFXVectorIterator it = bsearch(
		&callback,
		internal->callbacks.begin,
		gfx_vector_get_size(&internal->callbacks),
		sizeof(struct GFX_Callback),
		_gfx_pipe_callback_comp
	);

	if(it)
	{
		/* Find first equivalent callback */
		GFXVectorIterator first = it;
		while(first != internal->callbacks.begin)
		{
			GFXVectorIterator prev = gfx_vector_previous(&internal->callbacks, first);
			if(memcmp(prev, &callback, sizeof(GFXPipeCallback))) break;

			first = prev;
		}

		/* Find last equivalent callback */
		GFXVectorIterator last = gfx_vector_next(&internal->callbacks, it);
		while(last != internal->callbacks.end)
		{
			if(memcmp(last, &callback, sizeof(GFXPipeCallback))) break;
			last = gfx_vector_next(&internal->callbacks, last);
		}

		/* Erase the range */
		size_t num = gfx_vector_get_index(&internal->callbacks, last) - gfx_vector_get_index(&internal->callbacks, first);
		gfx_vector_erase_range(&internal->callbacks, num, first);
	}
}

/******************************************************/
GFXPipeCallback* gfx_pipe_find(GFXPipe* pipe, unsigned char key, size_t* num)
{
	/* First make sure it's sorted */
	GFX_Pipe* internal = GFX_PTR_SUB_BYTES(pipe, offsetof(GFX_Pipe, pipe));
	_gfx_pipe_sort(internal);

	return _gfx_pipe_find(internal, key, num);
}

/******************************************************/
GFXPipeCallback* gfx_pipe_next(GFXPipeCallback* prev)
{
	return GFX_PTR_ADD_BYTES(prev, sizeof(struct GFX_Callback));
}
