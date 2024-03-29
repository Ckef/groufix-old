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

#include <string.h>

/******************************************************/
static GFX_Pipe* _gfx_pipe_create(

		GFXPipeType   type,
		GFXPipeline*  pipeline)
{
	/* Create the pipe */
	GFX_Pipe* pipe = (GFX_Pipe*)gfx_list_create(sizeof(GFX_Pipe));
	if(!pipe) return NULL;

	memset(pipe, 0, sizeof(GFX_Pipe));
	pipe->type = type;
	pipe->pipeline = pipeline;

	/* Some defaults */
	_gfx_states_set_default(&pipe->state);

	return pipe;
}

/******************************************************/
GFX_Pipe* _gfx_pipe_create_bucket(

		GFXPipeline*    pipeline,
		unsigned char   bits)
{
	GFX_Pipe* pipe = _gfx_pipe_create(GFX_PIPE_BUCKET, pipeline);
	if(!pipe) return NULL;

	/* Create bucket */
	GFXBucket* bucket = _gfx_bucket_create(bits);
	if(!bucket)
	{
		_gfx_pipe_free((GFX_Pipe*)pipe);
		return NULL;
	}
	pipe->ptr.bucket = bucket;

	return pipe;
}

/******************************************************/
GFX_Pipe* _gfx_pipe_create_process(

		GFXPipeline*  pipeline,
		GFXWindow*    target,
		int           swap)
{
	GFX_Pipe* pipe = _gfx_pipe_create(GFX_PIPE_PROCESS, pipeline);
	if(!pipe) return NULL;

	/* Allocate process */
	GFXPipeProcess process = _gfx_pipe_process_create(target, swap);
	if(!process)
	{
		_gfx_pipe_free((GFX_Pipe*)pipe);
		return NULL;
	}
	pipe->ptr.process = process;

	return pipe;
}

/******************************************************/
GFX_Pipe* _gfx_pipe_free(

		GFX_Pipe* pipe)
{
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

		const GFXPipe* pipe)
{
	return ((const GFX_Pipe*)GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr)))->type;
}

/******************************************************/
GFXPipeState* gfx_pipe_get_state(

		GFXPipe* pipe)
{
	return &((GFX_Pipe*)GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr)))->state;
}
