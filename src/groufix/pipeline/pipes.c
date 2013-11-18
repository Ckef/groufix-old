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
#include "groufix/containers/vector.h"
#include "groufix/memory/datatypes.h"
#include "groufix/pipeline/internal.h"

#include <stdlib.h>

/******************************************************/
/** Internal Attachment */
struct GFX_Internal_Attachment
{
	GLenum         attachment; /* Key to sort on */
	GLuint         texture;
	GLenum         target;
	unsigned char  mipmap;
	unsigned int   layer;
};

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
	GLuint     fbo;         /* OpenGL handle */
	GFXVector  attachments; /* Stores GFX_Internal_Attachment */
	size_t     numTargets;
	GLenum*    targets;     /* OpenGL draw buffers */
	GFXDeque   pipes;       /* Stores GFX_Internal_Pipe */

	/* Not a shared resource */
	GFX_Extensions* ext;
};

/******************************************************/
static GFXVectorIterator _gfx_pipeline_find_attachment(struct GFX_Internal_Pipeline* pipeline, GLenum attach)
{
	/* Binary search for the attachment */
	size_t min = 0;
	size_t max = gfx_vector_get_size(&pipeline->attachments);
	GFXVectorIterator it = pipeline->attachments.end;

	while(max > min)
	{
		/* Get mid point */
		size_t mid = min + ((max - min) >> 1);

		it = gfx_vector_at(&pipeline->attachments, mid);
		GLenum found = ((struct GFX_Internal_Attachment*)it)->attachment;

		/* Compare against key */
		if(found < attach)
			min = mid + 1;
		else if(found > attach)
			max = mid;

		else return it;
	}
	return it;
}

/******************************************************/
static void _gfx_pipeline_init_attachment(GLuint fbo, struct GFX_Internal_Attachment* attach, const GFX_Extensions* ext)
{
	/* Check texture handle */
	if(ext->IsTexture(attach->texture))
	{
		/* Bind framebuffer and attach texture */
		ext->BindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

		switch(attach->target)
		{
			case GL_TEXTURE_BUFFER :
				ext->FramebufferTexture1D(
					GL_DRAW_FRAMEBUFFER,
					attach->attachment,
					GL_TEXTURE_BUFFER,
					attach->texture,
					0
				);
				break;

			case GL_TEXTURE_1D :
				ext->FramebufferTexture1D(
					GL_DRAW_FRAMEBUFFER,
					attach->attachment,
					GL_TEXTURE_1D,
					attach->texture,
					attach->mipmap
				);
				break;

			case GL_TEXTURE_2D :
				ext->FramebufferTexture2D(
					GL_DRAW_FRAMEBUFFER,
					attach->attachment,
					GL_TEXTURE_2D,
					attach->texture,
					attach->mipmap
				);
				break;

			case GL_TEXTURE_3D :
			case GL_TEXTURE_1D_ARRAY :
			case GL_TEXTURE_2D_ARRAY :
			case GL_TEXTURE_CUBE_MAP_ARRAY :
				ext->FramebufferTextureLayer(
					GL_DRAW_FRAMEBUFFER,
					attach->attachment,
					attach->texture,
					attach->mipmap,
					attach->layer
				);
				break;

			case GL_TEXTURE_CUBE_MAP :
				ext->FramebufferTexture2D(
					GL_DRAW_FRAMEBUFFER,
					attach->attachment,
					attach->layer,
					attach->texture,
					attach->mipmap
				);
				break;
		}
	}
}

/******************************************************/
static inline void _gfx_pipe_free(struct GFX_Internal_Pipe* pipe)
{
	if(pipe->process) free(pipe->pipe.data);
	else _gfx_bucket_free(pipe->pipe.bucket);
}

/******************************************************/
static int _gfx_pipe_create_bucket(struct GFX_Internal_Pipe* pipe, unsigned char bits, GFXBucketFlags flags)
{
	/* Create bucket */
	GFXBucket* bucket = _gfx_bucket_create(bits, flags);
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
static int _gfx_pipeline_push_pipe(struct GFX_Internal_Pipeline* pipeline, struct GFX_Internal_Pipe* pipe)
{
	/* Check for overflow (moar pipes!) */
	unsigned short index = gfx_deque_get_size(&pipeline->pipes);
	if(!(index + 1)) return 0;

	/* Get state of the pipe */
	if(!index) pipe->state = GFX_STATE_DEFAULT;
	else pipe->state = ((struct GFX_Internal_Pipe*)gfx_deque_at(&pipeline->pipes, index - 1))->state;

	/* Insert and return actual index + 1 */
	return gfx_deque_push_back(&pipeline->pipes, pipe) == pipeline->pipes.end ? 0 : index + 1;
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

	gfx_vector_clear(&pipeline->attachments);
	free(pipeline->targets);

	pipeline->targets = NULL;
	pipeline->numTargets = 0;
}

/******************************************************/
static void _gfx_pipeline_obj_save(void* object, GFX_Extensions* ext)
{
	struct GFX_Internal_Pipeline* pipeline = (struct GFX_Internal_Pipeline*)object;

	/* Don't clear the attachments vector or target array */
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

	/* Restore attachments */
	GFXVectorIterator it = pipeline->attachments.begin;
	while(it != pipeline->attachments.end)
	{
		_gfx_pipeline_init_attachment(pipeline->fbo, (struct GFX_Internal_Attachment*)it, ext);
		it = gfx_vector_next(&pipeline->attachments, it);
	}

	/* Restore targets */
	if(pipeline->numTargets)
	{
		ext->BindFramebuffer(GL_FRAMEBUFFER, pipeline->fbo);
		ext->DrawBuffers(pipeline->numTargets, pipeline->targets);
	}
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
	struct GFX_Internal_Pipeline* pl = calloc(1, sizeof(struct GFX_Internal_Pipeline));
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

	gfx_vector_init(&pl->attachments, sizeof(struct GFX_Internal_Attachment));
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

		gfx_vector_clear(&internal->attachments);
		gfx_deque_clear(&internal->pipes);

		free(internal->targets);
		free(pipeline);
	}
}

/******************************************************/
int gfx_pipeline_attach(GFXPipeline* pipeline, GFXTextureImage image, GFXPipelineAttachment attach, unsigned char index)
{
	struct GFX_Internal_Pipeline* internal = (struct GFX_Internal_Pipeline*)pipeline;

	/* Check attachment limit */
	if(attach != GFX_COLOR_ATTACHMENT) index = 0;
	else if(index >= internal->ext->limits[GFX_LIM_MAX_COLOR_ATTACHMENTS]) return 0;

	/* Init attachment */
	struct GFX_Internal_Attachment att;
	att.attachment = attach + index;
	att.texture    = _gfx_texture_get_handle(image.texture);
	att.target     = _gfx_texture_get_internal_target(image.texture);
	att.mipmap     = image.mipmap;

	/* Calculate appropriate layer */
	switch(att.target)
	{
		case GL_TEXTURE_CUBE_MAP :
			att.layer = image.face + GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			break;

		case GL_TEXTURE_CUBE_MAP_ARRAY :
			att.layer = (image.layer * 6) + image.face;
			break;

		default :
			att.layer = image.layer;
			break;
	}

	/* Determine whether to insert a new one or replace the old one */
	GFXVectorIterator it = _gfx_pipeline_find_attachment(internal, att.attachment);
	int new;

	if(it == internal->attachments.end) new = 1;
	else new = ((struct GFX_Internal_Attachment*)it)->attachment != att.attachment ? 1 : 0;

	if(new) gfx_vector_insert(&internal->attachments, &att, it);
	else *((struct GFX_Internal_Attachment*)it) = att;

	/* Send attachment to OGL */
	_gfx_pipeline_init_attachment(internal->fbo, &att, internal->ext);

	return 1;
}

/******************************************************/
size_t gfx_pipeline_target(GFXPipeline* pipeline, size_t num, const char* indices)
{
	struct GFX_Internal_Pipeline* internal = (struct GFX_Internal_Pipeline*)pipeline;
	if(!num) return 0;

	/* Limit number of targets */
	num = (num > internal->ext->limits[GFX_LIM_MAX_COLOR_TARGETS]) ?
		internal->ext->limits[GFX_LIM_MAX_COLOR_TARGETS] : num;

	/* Construct attachment buffer */
	free(internal->targets);
	internal->targets = malloc(sizeof(GLenum) * num);
	internal->numTargets = num;

	size_t i;
	int max = internal->ext->limits[GFX_LIM_MAX_COLOR_ATTACHMENTS];

	for(i = 0; i < num; ++i) internal->targets[i] = (indices[i] < 0 || indices[i] >= max)
		? GL_NONE : GFX_COLOR_ATTACHMENT + indices[i];

	/* Pass to OGL */
	internal->ext->BindFramebuffer(GL_FRAMEBUFFER, internal->fbo);
	internal->ext->DrawBuffers(num, internal->targets);

	return num;
}

/******************************************************/
unsigned short gfx_pipeline_push_bucket(GFXPipeline* pipeline, unsigned char bits, GFXBucketFlags flags)
{
	struct GFX_Internal_Pipeline* internal = (struct GFX_Internal_Pipeline*)pipeline;

	/* Create pipe */
	struct GFX_Internal_Pipe pipe;
	if(!_gfx_pipe_create_bucket(&pipe, bits, flags)) return 0;

	/* Insert pipe */
	unsigned short index = _gfx_pipeline_push_pipe(internal, &pipe);
	if(!index) _gfx_pipe_free(&pipe);

	return index;
}

/******************************************************/
unsigned short gfx_pipeline_push_process(GFXPipeline* pipeline, GFXPipeProcessFunc process, size_t dataSize)
{
	struct GFX_Internal_Pipeline* internal = (struct GFX_Internal_Pipeline*)pipeline;

	/* Create pipe */
	struct GFX_Internal_Pipe pipe;
	if(!_gfx_pipe_create_process(&pipe, process, dataSize)) return 0;

	/* Insert pipe */
	unsigned short index = _gfx_pipeline_push_pipe(internal, &pipe);
	if(!index) _gfx_pipe_free(&pipe);

	return index;
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
	/* Temporarily disabled for testing purposes
	internal->ext->BindFramebuffer(GL_FRAMEBUFFER, internal->fbo);*/

	/* Iterate over all pipes */
	GFXDequeIterator it;
	for(it = internal->pipes.begin; it != internal->pipes.end; it = gfx_deque_next(&internal->pipes, it))
	{
		struct GFX_Internal_Pipe* pipe = (struct GFX_Internal_Pipe*)it;

		/* Set states */
		_gfx_states_set(pipe->state, internal->ext);

		/* Process pipe */
		if(pipe->process) pipe->process(pipe->pipe.data);
		else _gfx_bucket_process(pipe->pipe.bucket, internal->ext);
	}
}
