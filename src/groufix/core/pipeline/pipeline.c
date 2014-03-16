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
	GFX_Pipe*  current;
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
static GFXVectorIterator _gfx_pipeline_find_attachment(struct GFX_Pipeline* pipeline, GLenum attach)
{
	/* Binary search for the attachment */
	size_t min = 0;
	size_t max = gfx_vector_get_size(&pipeline->attachments);

	while(max > min)
	{
		size_t mid = min + ((max - min) >> 1);

		GFXVectorIterator it = gfx_vector_at(&pipeline->attachments, mid);
		GLenum found = ((struct GFX_Attachment*)it)->attachment;

		/* Compare against key */
		if(found < attach)
			min = mid + 1;
		else if(found > attach)
			max = mid;

		else return it;
	}

	return gfx_vector_at(&pipeline->attachments, min);
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

	if(pipeline->first)
	{
		/* Preserve state, strip off clearing bits, put at end */
		gfx_list_splice_after((GFXList*)pipe, (GFXList*)pipeline->last);

		pipe->state = pipeline->last->state;
		pipe->state.state &= ~GFX_CLEAR_ALL;
	}

	/* First pipe */
	else pipeline->first = pipe;

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
	GFXVectorIterator it = _gfx_pipeline_find_attachment(internal, att.attachment);
	int new;

	if(it == internal->attachments.end) new = 1;
	else new = ((struct GFX_Attachment*)it)->attachment != att.attachment ? 1 : 0;

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
void gfx_pipeline_unlink_all(GFXPipeline* pipeline)
{
	struct GFX_Pipeline* internal = (struct GFX_Pipeline*)pipeline;

	/* Splice entire range into unlinked pipes */
	if(internal->first)
	{
		if(!internal->unlinked) internal->unlinked = internal->first;
		else gfx_list_splice_range_after((GFXList*)internal->unlinked, (GFXList*)internal->first, (GFXList*)internal->last);

		internal->first = NULL;
		internal->last = NULL;
		internal->current = NULL;
	}
}

/******************************************************/
void gfx_pipeline_unlink(GFXPipe* pipe)
{
	GFX_Pipe* internal = GFX_PTR_SUB_BYTES(pipe, offsetof(GFX_Pipe, ptr));
	struct GFX_Pipeline* pipeline = (struct GFX_Pipeline*)internal->pipeline;

	/* Fix current and unlink */
	if(pipeline->current == internal) pipeline->current = (GFX_Pipe*)internal->node.next;

	GFX_Pipe* new = (GFX_Pipe*)gfx_list_unsplice((GFXList*)internal, (GFXList*)internal);

	/* Replace if necessary */
	if(pipeline->first == internal)    pipeline->first    = new;
	if(pipeline->last == internal)     pipeline->last     = new;
	if(pipeline->unlinked == internal) pipeline->unlinked = new;

	/* Splice into unlinked pipes */
	if(!pipeline->unlinked) pipeline->unlinked = internal;
	else gfx_list_splice_after((GFXList*)internal, (GFXList*)pipeline->unlinked);
}

/******************************************************/
void gfx_pipeline_move(GFXPipe* pipe, GFXPipe* after)
{
	GFX_Pipe* int1 = GFX_PTR_SUB_BYTES(pipe, offsetof(GFX_Pipe, ptr));
	GFX_Pipe* int2 = after ? GFX_PTR_SUB_BYTES(after, offsetof(GFX_Pipe, ptr)) : NULL;

	if(int1 != int2 && ((GFX_Pipe*)int1->node.previous != int2 || !int2))
	{
		/* Check if it can be moved */
		if(int2) if(int1->pipeline != int2->pipeline) return;

		struct GFX_Pipeline* pl = (struct GFX_Pipeline*)int1->pipeline;

		/* Reconstruct pointers */
		if(pl->unlinked != int1)
		{
			if(pl->first == int1)   pl->first   = (GFX_Pipe*)int1->node.next;
			if(pl->last == int1)    pl->last    = (GFX_Pipe*)int1->node.previous;
			if(pl->current == int1) pl->current = (GFX_Pipe*)int1->node.next;
		}
		else pl->unlinked = (GFX_Pipe*)int1->node.next;

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

		if(pl->current == int1) pl->current = int2;
		else if(pl->current == int2) pl->current = int1;

		if(pl->unlinked == int1) pl->unlinked = int2;
		else if(pl->unlinked == int2) pl->unlinked = int1;

		/* Swap list elements */
		gfx_list_swap((GFXList*)int1, (GFXList*)int2);
	}
}

/******************************************************/
void gfx_pipeline_relink(size_t num, GFXPipe** pipes)
{
	if(num)
	{
		/* Get pipeline and check if they all have the same */
		GFXPipeline* pipeline = ((GFX_Pipe*)GFX_PTR_SUB_BYTES(pipes[0], offsetof(GFX_Pipe, ptr)))->pipeline;
		size_t ch = num;

		while(ch > 1)
		{
			GFXPipe* pipe = pipes[--ch];
			if(((GFX_Pipe*)GFX_PTR_SUB_BYTES(pipe, offsetof(GFX_Pipe, ptr)))->pipeline != pipeline)
				return;
		}

		/* Unlink and relink all pipes */
		gfx_pipeline_unlink_all(pipeline);
		while(num) gfx_pipeline_move(pipes[--num], NULL);
	}
}

/******************************************************/
void gfx_pipeline_remove(GFXPipe* pipe)
{
	GFX_Pipe* internal = GFX_PTR_SUB_BYTES(pipe, offsetof(GFX_Pipe, ptr));
	struct GFX_Pipeline* pipeline = (struct GFX_Pipeline*)internal->pipeline;

	/* Fix current and erase */
	if(pipeline->current == internal) pipeline->current = (GFX_Pipe*)internal->node.next;

	GFX_Pipe* new = _gfx_pipe_free(internal);

	/* Replace if necessary */
	if(pipeline->first == internal) pipeline->first = new;
	if(pipeline->last == internal) pipeline->last = new;
	if(pipeline->unlinked == internal) pipeline->unlinked = new;
}

/******************************************************/
void gfx_pipeline_execute(GFXPipeline* pipeline, unsigned int num)
{
	struct GFX_Pipeline* internal = (struct GFX_Pipeline*)pipeline;
	if(!internal->win) return;

	/* Bind as framebuffer */
	GFX_Extensions* ext = &internal->win->extensions;
	_gfx_pipeline_bind(internal->fbo, ext);

	/* Iterate over all pipes */
	unsigned int nolimit = num ? 0 : 1;
	GFX_Pipe* pipe = internal->current ? internal->current : internal->first;

	while(pipe && (nolimit | num--))
	{
		/* Set viewport */
		_gfx_states_set_viewport(internal->width, internal->height, ext);

		/* Process pipe */
		switch(pipe->type)
		{
			case GFX_PIPE_BUCKET :
				_gfx_bucket_process(pipe->ptr.bucket, &pipe->state, ext);
				break;

			case GFX_PIPE_PROCESS :
				_gfx_pipe_process_execute(pipe->ptr.process, &pipe->state, internal->win);
				break;
		}

		pipe = (GFX_Pipe*)pipe->node.next;
	}

	/* Update current */
	internal->current = pipe;
}
