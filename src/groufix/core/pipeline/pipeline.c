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

#include <stdlib.h>

/******************************************************/
/* Internal Attachment */
struct GFX_Attachment
{
	GLenum         attachment; /* Key to sort on */
	GLuint         texture;
	GLenum         target;
	unsigned char  mipmap;
	unsigned int   layer;
};

/* Internal Pipeline */
struct GFX_Pipeline
{
	/* Super class */
	GFXPipeline pipeline;

	/* Framebuffer */
	GLuint     fbo;         /* OpenGL handle */
	GFXVector  attachments; /* Stores GFX_Attachment */
	size_t     numTargets;
	GLenum*    targets;     /* OpenGL draw buffers */

	GFX_Pipe*  first;
	GFX_Pipe*  last;
	GFX_Pipe*  unlinked;

	/* Viewport */
	unsigned int width;
	unsigned int height;

	/* Not a shared resource */
	GFX_Window* win;
};

/******************************************************/
void _gfx_pipeline_bind(GLuint fbo, GFX_Extensions* ext)
{
	/* Prevent binding it twice */
	if(ext->pipeline != fbo)
	{
		ext->pipeline = fbo;
		ext->BindFramebuffer(GL_FRAMEBUFFER, fbo);
	}
}

/******************************************************/
static void _gfx_pipeline_init_attachment(GLuint fbo, struct GFX_Attachment* attach, GFX_Extensions* ext)
{
	/* Check texture handle */
	if(ext->IsTexture(attach->texture))
	{
		/* Bind framebuffer and attach texture */
		_gfx_pipeline_bind(fbo, ext);

		switch(attach->target)
		{
			case GL_TEXTURE_BUFFER :
				ext->FramebufferTexture1D(
					GL_FRAMEBUFFER,
					attach->attachment,
					GL_TEXTURE_BUFFER,
					attach->texture,
					0
				);
				break;

			case GL_TEXTURE_1D :
				ext->FramebufferTexture1D(
					GL_FRAMEBUFFER,
					attach->attachment,
					GL_TEXTURE_1D,
					attach->texture,
					attach->mipmap
				);
				break;

			case GL_TEXTURE_2D :
				ext->FramebufferTexture2D(
					GL_FRAMEBUFFER,
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
					GL_FRAMEBUFFER,
					attach->attachment,
					attach->texture,
					attach->mipmap,
					attach->layer
				);
				break;

			case GL_TEXTURE_CUBE_MAP :
				ext->FramebufferTexture2D(
					GL_FRAMEBUFFER,
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
static void _gfx_pipeline_push_pipe(GFX_Pipe* pipe)
{
	struct GFX_Pipeline* pipeline = (struct GFX_Pipeline*)pipe->pipeline;

	if(!pipeline->first)
	{
		/* Default state, first pipe */
		pipeline->first = pipe;
		pipe->state = GFX_STATE_DEFAULT;
	}
	else
	{
		/* Strip off clearing bits, put at end */
		gfx_list_splice_after((GFXList*)pipe, (GFXList*)pipeline->last);
		pipe->state = pipeline->last->state & ~GFX_CLEAR_ALL;
	}
	pipeline->last = pipe;
}

/******************************************************/
static void _gfx_pipeline_obj_free(void* object, GFX_Extensions* ext)
{
	struct GFX_Pipeline* pipeline = (struct GFX_Pipeline*)object;

	pipeline->win = NULL;
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
	struct GFX_Pipeline* pipeline = (struct GFX_Pipeline*)object;

	/* Don't clear the attachments vector or target array */
	ext->DeleteFramebuffers(1, &pipeline->fbo);

	pipeline->win = NULL;
	pipeline->fbo = 0;
}

/******************************************************/
static void _gfx_pipeline_obj_restore(void* object, GFX_Extensions* ext)
{
	struct GFX_Pipeline* pipeline = (struct GFX_Pipeline*)object;

	/* Create FBO */
	pipeline->win = _gfx_window_get_current();
	ext->GenFramebuffers(1, &pipeline->fbo);

	/* Restore attachments */
	GFXVectorIterator it = pipeline->attachments.begin;
	while(it != pipeline->attachments.end)
	{
		_gfx_pipeline_init_attachment(pipeline->fbo, (struct GFX_Attachment*)it, ext);
		it = gfx_vector_next(&pipeline->attachments, it);
	}

	/* Restore targets */
	if(pipeline->numTargets)
	{
		_gfx_pipeline_bind(pipeline->fbo, ext);
		ext->DrawBuffers(pipeline->numTargets, pipeline->targets);
	}
}

/******************************************************/
/* vtable for hardware part of the pipeline */
static GFX_HardwareFuncs _gfx_pipeline_obj_funcs =
{
	_gfx_pipeline_obj_free,
	_gfx_pipeline_obj_save,
	_gfx_pipeline_obj_restore
};

/******************************************************/
GLuint _gfx_pipeline_get_handle(const GFXPipeline* pipeline)
{
	return ((struct GFX_Pipeline*)pipeline)->fbo;
}

/******************************************************/
GFXPipeline* gfx_pipeline_create(void)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Create new pipeline */
	struct GFX_Pipeline* pl = calloc(1, sizeof(struct GFX_Pipeline));
	if(!pl) return NULL;

	/* Register as object */
	pl->pipeline.id = _gfx_hardware_object_register(pl, &_gfx_pipeline_obj_funcs);
	if(!pl->pipeline.id)
	{
		free(pl);
		return NULL;
	}

	/* Create OpenGL resources */
	pl->win = window;
	pl->win->extensions.GenFramebuffers(1, &pl->fbo);
	pl->width = 0;
	pl->height = 0;

	gfx_vector_init(&pl->attachments, sizeof(struct GFX_Attachment));

	return (GFXPipeline*)pl;
}

/******************************************************/
void gfx_pipeline_free(GFXPipeline* pipeline)
{
	if(pipeline)
	{
		struct GFX_Pipeline* internal = (struct GFX_Pipeline*)pipeline;

		/* Unregister as object */
		_gfx_hardware_object_unregister(pipeline->id);

		/* Delete FBO */
		if(internal->win)
		{
			if(internal->win->extensions.program == internal->fbo)
				internal->win->extensions.program = 0;

			internal->win->extensions.DeleteFramebuffers(1, &internal->fbo);
		}

		/* Free all pipes */
		while(internal->first) gfx_pipeline_remove(&internal->first->ptr);
		while(internal->unlinked) gfx_pipeline_remove(&internal->unlinked->ptr);

		/* Free pipeline */
		gfx_vector_clear(&internal->attachments);
		free(internal->targets);

		free(pipeline);
	}
}

/******************************************************/
size_t gfx_pipeline_target(GFXPipeline* pipeline, unsigned int width, unsigned int height, size_t num, const char* indices)
{
	struct GFX_Pipeline* internal = (struct GFX_Pipeline*)pipeline;
	if(!num || !internal->win) return 0;

	GFX_Extensions* ext = &internal->win->extensions;

	/* Limit number of targets */
	num = (num > ext->limits[GFX_LIM_MAX_COLOR_TARGETS]) ?
		ext->limits[GFX_LIM_MAX_COLOR_TARGETS] : num;

	/* Construct attachment buffer */
	GLenum* targets = malloc(sizeof(GLenum) * num);
	if(!targets) return 0;

	free(internal->targets);
	internal->targets = targets;
	internal->numTargets = num;

	size_t i;
	int max = ext->limits[GFX_LIM_MAX_COLOR_ATTACHMENTS];

	for(i = 0; i < num; ++i) internal->targets[i] = (indices[i] < 0 || indices[i] >= max)
		? GL_NONE : GFX_COLOR_ATTACHMENT + indices[i];

	/* Pass to OGL */
	_gfx_pipeline_bind(internal->fbo, ext);
	ext->DrawBuffers(num, internal->targets);

	internal->width = width;
	internal->height = height;

	return num;
}

/******************************************************/
static int _gfx_pipeline_attachment_comp(const void* key, const void* elem)
{
	GLenum attach = GFX_VOID_TO_UINT(key);
	GLenum found = ((struct GFX_Attachment*)elem)->attachment;

	if(found < attach) return 1;
	if(found > attach) return -1;

	return 0;
}

/******************************************************/
int gfx_pipeline_attach(GFXPipeline* pipeline, GFXTextureImage image, GFXPipelineAttachment attach, unsigned char index)
{
	struct GFX_Pipeline* internal = (struct GFX_Pipeline*)pipeline;
	if(!internal->win) return 0;

	GFX_Extensions* ext = &internal->win->extensions;

	/* Check attachment limit */
	if(attach != GFX_COLOR_ATTACHMENT) index = 0;
	else if(index >= ext->limits[GFX_LIM_MAX_COLOR_ATTACHMENTS]) return 0;

	/* Init attachment */
	struct GFX_Attachment att;
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
	GFXVectorIterator it = bsearch(
		GFX_UINT_TO_VOID(attach),
		internal->attachments.begin,
		gfx_vector_get_size(&internal->attachments),
		sizeof(struct GFX_Attachment),
		_gfx_pipeline_attachment_comp
	);
	int new;

	if(!it)
	{
		it = internal->attachments.end;
		new = 1;
	}
	else new = ((struct GFX_Attachment*)it)->attachment != att.attachment;

	/* Now actually insert one or replace the old one */
	if(new) gfx_vector_insert(&internal->attachments, &att, it);
	else *((struct GFX_Attachment*)it) = att;

	/* Send attachment to OGL */
	_gfx_pipeline_init_attachment(internal->fbo, &att, ext);

	return 1;
}

/******************************************************/
GFXPipe* gfx_pipeline_push_bucket(GFXPipeline* pipeline, unsigned char bits, GFXBucketFlags flags)
{
	/* Create the pipe and push it */
	GFX_Pipe* pipe = _gfx_pipe_create_bucket(pipeline, bits, flags);
	if(!pipe) return 0;

	_gfx_pipeline_push_pipe(pipe);

	return &pipe->ptr;
}

/******************************************************/
GFXPipe* gfx_pipeline_push_process(GFXPipeline* pipeline)
{
	/* Create the pipe and push it */
	GFX_Pipe* pipe = _gfx_pipe_create_process(pipeline);
	if(!pipe) return 0;

	_gfx_pipeline_push_pipe(pipe);

	return &pipe->ptr;
}

/******************************************************/
void gfx_pipeline_unlink(GFXPipe* pipe)
{
	/* Unsplice and replace if necessary */
	GFX_Pipe* internal = GFX_PTR_SUB_BYTES(pipe, offsetof(GFX_Pipe, ptr));
	struct GFX_Pipeline* pipeline = (struct GFX_Pipeline*)internal->pipeline;

	GFX_Pipe* new = (GFX_Pipe*)gfx_list_unsplice((GFXList*)internal, (GFXList*)internal);

	if(pipeline->first == internal) pipeline->first = new;
	if(pipeline->last == internal) pipeline->last = new;
	if(pipeline->unlinked == internal) pipeline->unlinked = new;

	/* Splice into unlinked pipes */
	if(!pipeline->unlinked) pipeline->unlinked = internal;
	else gfx_list_splice_after((GFXList*)internal, (GFXList*)pipeline->unlinked);
}

/******************************************************/
void gfx_pipeline_move(GFXPipe* pipe, GFXPipe* after)
{
	if(pipe != after)
	{
		GFX_Pipe* int1 = GFX_PTR_SUB_BYTES(pipe, offsetof(GFX_Pipe, ptr));
		GFX_Pipe* int2 = after ? GFX_PTR_SUB_BYTES(after, offsetof(GFX_Pipe, ptr)) : NULL;

		/* Check if it can be moved */
		if(int2) if(int1->pipeline != int2->pipeline) return;

		struct GFX_Pipeline* pl = (struct GFX_Pipeline*)int1->pipeline;

		/* Reconstruct pointers */
		if(pl->first == int1) pl->first = (GFX_Pipe*)int1->node.next;
		if(pl->last == int1) pl->last = (GFX_Pipe*)int1->node.previous;
		if(pl->unlinked == int1) pl->unlinked = (GFX_Pipe*)int1->node.next;

		/* Move it */
		if(int2)
		{
			gfx_list_splice_after((GFXList*)int1, (GFXList*)int2);
			if(pl->last == int2) pl->last = int1;
		}
		else if(pl->first)
		{
			gfx_list_splice_before((GFXList*)int1, (GFXList*)pl->first);
			pl->first = int1;
		}
		else
		{
			gfx_list_unsplice((GFXList*)int1, (GFXList*)int1);
			pl->first = int1;
			pl->last = int1;
		}
	}
}

/******************************************************/
void gfx_pipeline_swap(GFXPipe* pipe1, GFXPipe* pipe2)
{
	GFX_Pipe* int1 = GFX_PTR_SUB_BYTES(pipe1, offsetof(GFX_Pipe, ptr));
	GFX_Pipe* int2 = GFX_PTR_SUB_BYTES(pipe2, offsetof(GFX_Pipe, ptr));

	/* Check if they can be swapped */
	if(int1->pipeline == int2->pipeline)
	{
		struct GFX_Pipeline* pl = (struct GFX_Pipeline*)int1->pipeline;

		/* Reconstruct pointers */
		if(pl->first == int1) pl->first = int2;
		else if(pl->first == int2) pl->first = int1;

		if(pl->last == int1) pl->last = int2;
		else if(pl->last == int2) pl->last = int1;

		if(pl->unlinked == int1) pl->unlinked = int2;
		else if(pl->unlinked == int2) pl->unlinked = int1;

		/* Swap list elements */
		gfx_list_swap((GFXList*)int1, (GFXList*)int2);
	}
}

/******************************************************/
void gfx_pipeline_remove(GFXPipe* pipe)
{
	/* Erase and replace if necessary */
	GFX_Pipe* internal = GFX_PTR_SUB_BYTES(pipe, offsetof(GFX_Pipe, ptr));
	struct GFX_Pipeline* pipeline = (struct GFX_Pipeline*)internal->pipeline;

	GFX_Pipe* new = _gfx_pipe_free(internal);

	if(pipeline->first == internal) pipeline->first = new;
	if(pipeline->last == internal) pipeline->last = new;
	if(pipeline->unlinked == internal) pipeline->unlinked = new;
}

/******************************************************/
void gfx_pipeline_execute(GFXPipeline* pipeline)
{
	struct GFX_Pipeline* internal = (struct GFX_Pipeline*)pipeline;
	if(!internal->win) return;

	/* Bind as framebuffer */
	GFX_Extensions* ext = &internal->win->extensions;
	_gfx_pipeline_bind(internal->fbo, ext);

	/* Iterate over all pipes */
	GFX_Pipe* pipe;
	for(pipe = internal->first; pipe; pipe = (GFX_Pipe*)pipe->node.next)
	{
		/* Set viewport */
		_gfx_states_set_viewport(internal->width, internal->height, ext);

		/* Process pipe */
		switch(pipe->type)
		{
			case GFX_PIPE_BUCKET :
				_gfx_bucket_process(pipe->ptr.bucket, pipe->state, ext);
				break;

			case GFX_PIPE_PROCESS :
				_gfx_pipe_process_execute(pipe->ptr.process, pipe->state, internal->win);
				break;
		}
	}
}
