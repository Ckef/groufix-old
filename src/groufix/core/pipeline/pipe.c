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

/* Internal pipe flags */
#define GFX_INT_PIPE_SORTED  0x01
#define GFX_INT_PIPE_RANGED  0x02

/******************************************************/
/* Internal pipe */
struct GFX_Internal_Pipe
{
	/* Super class */
	GFX_Pipe pipe;

	/* Callbacks */
	GFXVector      callbacks; /* Stores GFX_Callback */
	GFXVector      ranges;    /* Stores GFX_Range */
	unsigned char  flags;
};

/* Internal callback */
struct GFX_Callback
{
	GFXPipeCallback      callback; /* Super class */
	GFXPipeCallbackFunc  func;
};

/* Internal callback range */
struct GFX_Range
{
	unsigned char  key; /* Callback key */
	size_t         end; /* Upper bound */
};

/******************************************************/
static int _gfx_pipe_callback_comp(

		const void*  elem1,
		const void*  elem2)
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
static int _gfx_pipe_range_comp(

		const void*  key,
		const void*  elem)
{
	unsigned char k = GFX_VOID_TO_UINT(key);
	struct GFX_Range* range = (struct GFX_Range*)elem;

	if(range->key > k) return -1;
	if(range->key < k) return 1;

	return 0;
}

/******************************************************/
static void _gfx_pipe_sort(

		struct GFX_Internal_Pipe* pipe)
{
	/* Sort if necessary */
	if(!(pipe->flags & GFX_INT_PIPE_SORTED))
	{
		qsort(
			pipe->callbacks.begin,
			gfx_vector_get_size(&pipe->callbacks),
			sizeof(struct GFX_Callback),
			_gfx_pipe_callback_comp
		);
		pipe->flags |= GFX_INT_PIPE_SORTED;
	}
}

/******************************************************/
static void _gfx_pipe_range(

		struct GFX_Internal_Pipe* pipe)
{
	/* Range if necessary */
	if(!(pipe->flags & GFX_INT_PIPE_RANGED))
	{
		gfx_vector_clear(&pipe->ranges);

		if(pipe->callbacks.begin != pipe->callbacks.end)
		{
			/* Get first key */
			GFXVectorIterator it = pipe->callbacks.begin;

			struct GFX_Range range;
			range.key = ((GFXPipeCallback*)it)->key;
			range.end = 1;

			/* Iterate through keys and extract ranges */
			it = gfx_vector_next(&pipe->callbacks, it);
			while(it != pipe->callbacks.end)
			{
				/* Insert range if a new key */
				unsigned char key = ((GFXPipeCallback*)it)->key;
				if(key != range.key)
				{
					gfx_vector_insert(&pipe->ranges, &range, pipe->ranges.end);
					range.key = key;
				}

				it = gfx_vector_next(&pipe->callbacks, it);
				++range.end;
			}

			/* Insert last range */
			gfx_vector_insert(&pipe->ranges, &range, pipe->ranges.end);
		}

		pipe->flags |= GFX_INT_PIPE_RANGED;
	}
}

/******************************************************/
static GFXVectorIterator _gfx_pipe_find(

		struct GFX_Internal_Pipe*  pipe,
		unsigned char              key,
		size_t*                    num)
{
	/* Find range element */
	struct GFX_Range* it = bsearch(
		GFX_UINT_TO_VOID(key),
		pipe->ranges.begin,
		gfx_vector_get_size(&pipe->ranges),
		sizeof(struct GFX_Range),
		_gfx_pipe_range_comp
	);

	if(!it)
	{
		*num = 0;
		return NULL;
	}

	/* Get previous range and calculate number of elements */
	size_t begin = 0;
	if(it != pipe->ranges.begin)
		begin = ((struct GFX_Range*)gfx_vector_previous(&pipe->ranges, it))->end;

	*num = it->end - begin;
	return gfx_vector_at(&pipe->callbacks, begin);
}

/******************************************************/
static struct GFX_Internal_Pipe* _gfx_pipe_create(

		GFXPipeType   type,
		GFXPipeline*  pipeline)
{
	/* Create the pipe */
	struct GFX_Internal_Pipe* pipe = (struct GFX_Internal_Pipe*)gfx_list_create(sizeof(struct GFX_Internal_Pipe));
	if(!pipe) return NULL;

	pipe->pipe.type = type;
	pipe->pipe.pipeline = pipeline;

	gfx_vector_init(&pipe->callbacks, sizeof(struct GFX_Callback));
	gfx_vector_init(&pipe->ranges, sizeof(struct GFX_Range));

	/* Some defaults */
	_gfx_states_set_default(&pipe->pipe.state);

	return pipe;
}

/******************************************************/
GFX_Pipe* _gfx_pipe_create_bucket(

		GFXPipeline*    pipeline,
		unsigned char   bits,
		GFXBucketFlags  flags)
{
	struct GFX_Internal_Pipe* pipe = _gfx_pipe_create(GFX_PIPE_BUCKET, pipeline);
	if(!pipe) return NULL;

	/* Create bucket */
	GFXBucket* bucket = _gfx_bucket_create(bits, flags);
	if(!bucket)
	{
		_gfx_pipe_free((GFX_Pipe*)pipe);
		return 0;
	}
	pipe->pipe.ptr.bucket = bucket;

	return (GFX_Pipe*)pipe;
}

/******************************************************/
GFX_Pipe* _gfx_pipe_create_process(

		GFXPipeline* pipeline)
{
	struct GFX_Internal_Pipe* pipe = _gfx_pipe_create(GFX_PIPE_PROCESS, pipeline);
	if(!pipe) return NULL;

	/* Allocate process */
	GFXPipeProcess process = _gfx_pipe_process_create();
	if(!process)
	{
		_gfx_pipe_free((GFX_Pipe*)pipe);
		return 0;
	}
	pipe->pipe.ptr.process = process;

	return (GFX_Pipe*)pipe;
}

/******************************************************/
GFX_Pipe* _gfx_pipe_free(

		GFX_Pipe* pipe)
{
	struct GFX_Internal_Pipe* internal = (struct GFX_Internal_Pipe*)pipe;

	/* Issue all callbacks */
	GFXVectorIterator it;
	for(
		it = internal->callbacks.begin;
		it != internal->callbacks.end;
		it = gfx_vector_next(&internal->callbacks, it))
	{
		struct GFX_Callback* call = (struct GFX_Callback*)it;
		if(call->func) call->func(&pipe->ptr, &call->callback);
	}

	gfx_vector_clear(&internal->callbacks);
	gfx_vector_clear(&internal->ranges);

	/* Free the actual pipe */
	switch(pipe->type)
	{
		case GFX_PIPE_BUCKET :
			_gfx_bucket_free(pipe->ptr.bucket);
			break;

		case GFX_PIPE_PROCESS :
			_gfx_pipe_process_free(pipe->ptr.process);
			break;
	}

	return (GFX_Pipe*)gfx_list_erase((GFXList*)pipe);
}

/******************************************************/
GFXPipeType gfx_pipe_get_type(

		GFXPipe* pipe)
{
	return ((GFX_Pipe*)GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr)))->type;
}

/******************************************************/
GFXPipeState gfx_pipe_get_state(

		GFXPipe* pipe)
{
	return ((GFX_Pipe*)GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr)))->state.state;
}

/******************************************************/
void gfx_pipe_set_state(

		GFXPipe*      pipe,
		GFXPipeState  state)
{
	((GFX_Pipe*)GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr)))->state.state = state;
}

/******************************************************/
void gfx_pipe_set_depth_test(

		GFXPipe*         pipe,
		GFXFragmentTest  test)
{
	((GFX_Pipe*)GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr)))->state.depthFunc = test;
}

/******************************************************/
void gfx_pipe_set_blend_state_rgb(

		GFXPipe*       pipe,
		GFXBlendState  state)
{
	((GFX_Pipe*)GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr)))->state.blendRGB = state;
}

/******************************************************/
void gfx_pipe_set_blend_state_alpha(

		GFXPipe*       pipe,
		GFXBlendState  state)
{
	((GFX_Pipe*)GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr)))->state.blendAlpha = state;
}

/******************************************************/
void gfx_pipe_set_blend_function_rgb(

		GFXPipe*      pipe,
		GFXBlendFunc  source,
		GFXBlendFunc  buffer)
{
	GFX_Pipe* internal = GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr));

	internal->state.blendSourceRGB = source;
	internal->state.blendBufferRGB = buffer;
}

/******************************************************/
void gfx_pipe_set_blend_function_alpha(

		GFXPipe*      pipe,
		GFXBlendFunc  source,
		GFXBlendFunc  buffer)
{
	GFX_Pipe* internal = GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr));

	internal->state.blendSourceAlpha = source;
	internal->state.blendBufferAlpha = buffer;
}

/******************************************************/
void gfx_pipe_set_stencil_test_front(

		GFXPipe*         pipe,
		GFXFragmentTest  test,
		int              ref,
		unsigned int     mask)
{
	GFX_Pipe* internal = GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr));

	internal->state.stencilFuncFront = test;
	internal->state.stencilRefFront = ref;
	internal->state.stencilMaskFront = mask;
}

/******************************************************/
void gfx_pipe_set_stencil_test_back(

		GFXPipe*         pipe,
		GFXFragmentTest  test,
		int              ref,
		unsigned int     mask)
{
	GFX_Pipe* internal = GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr));

	internal->state.stencilFuncBack = test;
	internal->state.stencilRefBack = ref;
	internal->state.stencilMaskBack = mask;
}

/******************************************************/
void gfx_pipe_set_stencil_func_front(

		GFXPipe*        pipe,
		GFXStencilFunc  fail,
		GFXStencilFunc  depth,
		GFXStencilFunc  pass)
{
	GFX_Pipe* internal = GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr));

	internal->state.stencilFailFront = fail;
	internal->state.depthFailFront = depth;
	internal->state.stencilPassFront = pass;
}

/******************************************************/
void gfx_pipe_set_stencil_func_back(

		GFXPipe*        pipe,
		GFXStencilFunc  fail,
		GFXStencilFunc  depth,
		GFXStencilFunc  pass)
{
	GFX_Pipe* internal = GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr));

	internal->state.stencilFailBack = fail;
	internal->state.depthFailBack = depth;
	internal->state.stencilPassBack = pass;
}

/******************************************************/
int gfx_pipe_register(

		GFXPipe*             pipe,
		GFXPipeCallback      callback,
		GFXPipeCallbackFunc  func)
{
	struct GFX_Internal_Pipe* internal = GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr));

	/* Insert the callback object */
	struct GFX_Callback call;
	call.callback = callback;
	call.func = func;

	if(gfx_vector_insert(&internal->callbacks, &call, internal->callbacks.end) == internal->callbacks.end)
		return 0;

	internal->flags = 0;

	return 1;
}

/******************************************************/
void gfx_pipe_unregister(

		GFXPipe*         pipe,
		GFXPipeCallback  callback)
{
	/* Sort if necessary */
	struct GFX_Internal_Pipe* internal = GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr));

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
		size_t num = gfx_vector_get_index(&internal->callbacks, last);
		num -= gfx_vector_get_index(&internal->callbacks, first);

		gfx_vector_erase_range(&internal->callbacks, num, first);

		internal->flags &= ~GFX_INT_PIPE_RANGED;
	}
}

/******************************************************/
int gfx_pipe_exists(

		GFXPipe*         pipe,
		GFXPipeCallback  callback)
{
	/* Sort if necessary */
	struct GFX_Internal_Pipe* internal = GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr));

	_gfx_pipe_sort(internal);

	/* Find the callback object */
	return bsearch(
		&callback,
		internal->callbacks.begin,
		gfx_vector_get_size(&internal->callbacks),
		sizeof(struct GFX_Callback),
		_gfx_pipe_callback_comp) ? 1 : 0;
}

/******************************************************/
GFXPipeCallback* gfx_pipe_find(

		GFXPipe*       pipe,
		unsigned char  key,
		size_t*        num)
{
	/* First make sure it's sorted and ranged */
	struct GFX_Internal_Pipe* internal = GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr));

	_gfx_pipe_sort(internal);
	_gfx_pipe_range(internal);

	return _gfx_pipe_find(internal, key, num);
}

/******************************************************/
GFXPipeCallback* gfx_pipe_at(

		GFXPipeCallback*  arr,
		size_t            index)
{
	return (GFXPipeCallback*)(((struct GFX_Callback*)arr) + index);
}
