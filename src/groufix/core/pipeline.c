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
/** Internal Pipeline */
typedef struct GFX_Pipeline
{
	/* Super class */
	GFXPipeline pipeline;

	/* Framebuffer */
	GFX_RenderObjectID  id;
	GLuint              fbo;         /* OpenGL handle */
	GFXVector           attachments; /* Stores GFX_Attachment */
	unsigned int        numTargets;
	GLenum*             targets;     /* OpenGL draw buffers */

	GFX_Pipe*           first;
	GFX_Pipe*           last;
	GFX_Pipe*           current;
	GFX_Pipe*           unlinked;

} GFX_Pipeline;


/** Internal Attachment */
typedef struct GFX_Attachment
{
	GLenum         attachment; /* Key to sort on */
	GLuint         texture;
	GLenum         target;
	unsigned char  mipmap;
	unsigned int   layer;

} GFX_Attachment;


/******************************************************/
static GFXVectorIterator _gfx_pipeline_find_attachment(

		const GFX_Pipeline*  pipeline,
		GLenum               attach)
{
	/* Binary search for the attachment */
	size_t min = 0;
	size_t max = gfx_vector_get_size(&pipeline->attachments);

	while(max > min)
	{
		size_t mid = min + ((max - min) >> 1);

		GFXVectorIterator it =
			gfx_vector_at(&pipeline->attachments, mid);
		GLenum found =
			((GFX_Attachment*)it)->attachment;

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
static void _gfx_pipeline_init_attachment(

		GLuint                 fbo,
		const GFX_Attachment*  attach,
		GFX_CONT_ARG)
{
	/* Check texture handle */
	switch(attach->target)
	{
		case GL_TEXTURE_BUFFER :
			GFX_REND_GET.NamedFramebufferTexture(
				fbo,
				attach->attachment,
				attach->texture,
				0
			);
			break;

		case GL_TEXTURE_2D :
			GFX_REND_GET.NamedFramebufferTexture2D(
				fbo,
				attach->attachment,
				GL_TEXTURE_2D,
				attach->texture,
				attach->mipmap
			);
			break;

		case GL_TEXTURE_2D_MULTISAMPLE :
			GFX_REND_GET.NamedFramebufferTexture2D(
				fbo,
				attach->attachment,
				GL_TEXTURE_2D_MULTISAMPLE,
				attach->texture,
				0
			);
			break;

		case GL_TEXTURE_2D_MULTISAMPLE_ARRAY :
			GFX_REND_GET.NamedFramebufferTextureLayer(
				fbo,
				attach->attachment,
				attach->texture,
				0,
				attach->layer
			);
			break;

		case GL_TEXTURE_3D :
		case GL_TEXTURE_2D_ARRAY :
		case GL_TEXTURE_CUBE_MAP :
		case GL_TEXTURE_CUBE_MAP_ARRAY :
			GFX_REND_GET.NamedFramebufferTextureLayer(
				fbo,
				attach->attachment,
				attach->texture,
				attach->mipmap,
				attach->layer
			);
			break;
	}
}

/******************************************************/
static void _gfx_pipeline_push_pipe(

		GFX_Pipe* pipe)
{
	GFX_Pipeline* pipeline = (GFX_Pipeline*)pipe->pipeline;

	if(pipeline->first)
	{
		/* Preserve state, strip off clearing bits, put at end */
		gfx_list_splice_after(
			(GFXList*)pipe,
			(GFXList*)pipeline->last
		);

		pipe->state = pipeline->last->state;
		pipe->state.render.state &= ~GFX_CLEAR_ALL;
	}

	/* First pipe */
	else pipeline->first = pipe;

	pipeline->last = pipe;
}

/******************************************************/
static void _gfx_pipeline_obj_free(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_Pipeline* pipeline = (GFX_Pipeline*)object;

	pipeline->id = id;
	pipeline->fbo = 0;
}

/******************************************************/
static void _gfx_pipeline_obj_save(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_Pipeline* pipeline = (GFX_Pipeline*)object;

	/* Don't clear the attachments vector or target array */
	pipeline->id = id;
	GFX_REND_GET.DeleteFramebuffers(1, &pipeline->fbo);
	pipeline->fbo = 0;
}

/******************************************************/
static void _gfx_pipeline_obj_restore(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_Pipeline* pipeline = (GFX_Pipeline*)object;

	/* Create FBO */
	pipeline->id = id;
	GFX_REND_GET.CreateFramebuffers(1, &pipeline->fbo);

	/* Restore attachments */
	GFXVectorIterator it = pipeline->attachments.begin;
	while(it != pipeline->attachments.end)
	{
		_gfx_pipeline_init_attachment(
			pipeline->fbo,
			(GFX_Attachment*)it,
			GFX_CONT_AS_ARG
		);

		it = gfx_vector_next(&pipeline->attachments, it);
	}

	/* Restore targets */
	if(pipeline->numTargets)
		GFX_REND_GET.NamedFramebufferDrawBuffers(
			pipeline->fbo,
			pipeline->numTargets,
			pipeline->targets
		);
}

/******************************************************/
/* vtable for render object part of the pipeline */
static GFX_RenderObjectFuncs _gfx_pipeline_obj_funcs =
{
	_gfx_pipeline_obj_free,
	_gfx_pipeline_obj_save,
	_gfx_pipeline_obj_restore
};

/******************************************************/
GLuint _gfx_gl_pipeline_get_handle(

		const GFXPipeline* pipeline)
{
	return ((const GFX_Pipeline*)pipeline)->fbo;
}

/******************************************************/
GFXPipeline* gfx_pipeline_create(void)
{
	GFX_CONT_INIT(NULL);

	/* Create new pipeline */
	GFX_Pipeline* pl = calloc(1, sizeof(GFX_Pipeline));
	if(!pl)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Pipeline could not be allocated."
		);
		return NULL;
	}

	/* Register as object */
	pl->id = _gfx_render_object_register(
		&GFX_CONT_GET.objects,
		pl,
		&_gfx_pipeline_obj_funcs
	);

	if(!pl->id.id)
	{
		free(pl);
		return NULL;
	}

	/* Create OpenGL resources */
	GFX_REND_GET.CreateFramebuffers(1, &pl->fbo);

	gfx_vector_init(&pl->attachments, sizeof(GFX_Attachment));

	return (GFXPipeline*)pl;
}

/******************************************************/
void gfx_pipeline_free(

		GFXPipeline* pipeline)
{
	if(pipeline)
	{
		GFX_CONT_INIT_UNSAFE;

		GFX_Pipeline* internal = (GFX_Pipeline*)pipeline;

		/* Unregister as object */
		_gfx_render_object_unregister(internal->id);

		if(!GFX_CONT_EQ(NULL))
		{
			/* Delete framebuffer */
			if(GFX_REND_GET.fbos[0] == internal->fbo)
				GFX_REND_GET.fbos[0] = 0;
			if(GFX_REND_GET.fbos[1] == internal->fbo)
				GFX_REND_GET.fbos[1] = 0;

			GFX_REND_GET.DeleteFramebuffers(1, &internal->fbo);
		}

		/* Free all pipes, attachments and targets */
		while(internal->first)
			gfx_pipeline_remove(&internal->first->ptr);
		while(internal->unlinked)
			gfx_pipeline_remove(&internal->unlinked->ptr);

		gfx_vector_clear(&internal->attachments);
		free(internal->targets);

		free(pipeline);
	}
}

/******************************************************/
unsigned int gfx_pipeline_target(

		GFXPipeline*        pipeline,
		unsigned int        num,
		const signed char*  indices)
{
	GFX_CONT_INIT(0);

	if(!num) return 0;
	GFX_Pipeline* internal = (GFX_Pipeline*)pipeline;

	/* Limit number of targets */
	num = (num > GFX_CONT_GET.lim[GFX_LIM_MAX_COLOR_TARGETS]) ?
		GFX_CONT_GET.lim[GFX_LIM_MAX_COLOR_TARGETS] : num;

	/* Construct attachment buffer */
	GLenum* targets = malloc(sizeof(GLenum) * num);
	if(!targets)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Pipeline ran out of memory during target allocation."
		);
		return 0;
	}

	free(internal->targets);
	internal->targets = targets;
	internal->numTargets = num;

	unsigned int i;
	int max = GFX_CONT_GET.lim[GFX_LIM_MAX_COLOR_ATTACHMENTS];

	for(i = 0; i < num; ++i)
	{
		internal->targets[i] = (indices[i] < 0 || indices[i] >= max)
			? GL_NONE : GFX_COLOR_ATTACHMENT + indices[i];
	}

	/* Pass to OGL */
	GFX_REND_GET.NamedFramebufferDrawBuffers(
		internal->fbo,
		num,
		internal->targets
	);

	return num;
}

/******************************************************/
int gfx_pipeline_attach(

		GFXPipeline*           pipeline,
		GFXTextureImage        image,
		GFXPipelineAttachment  attach,
		unsigned char          index)
{
	GFX_CONT_INIT(0);

	GFX_Pipeline* internal = (GFX_Pipeline*)pipeline;

	/* Check attachment limit */
	if(attach != GFX_COLOR_ATTACHMENT) index = 0;
	else if(index >= GFX_CONT_GET.lim[GFX_LIM_MAX_COLOR_ATTACHMENTS])
		return 0;

	/* Init attachment */
	GFX_Attachment att =
	{
		.attachment = attach + index,
		.texture    = _gfx_gl_texture_get_handle(image.texture),
		.target     = _gfx_gl_texture_get_internal_target(image.texture),
		.mipmap     = image.mipmap
	};

	/* Calculate appropriate layer */
	switch(att.target)
	{
		case GL_TEXTURE_CUBE_MAP :
			att.layer = image.face;
			break;

		case GL_TEXTURE_CUBE_MAP_ARRAY :
			att.layer = (image.layer * 6) + image.face;
			break;

		default :
			att.layer = image.layer;
			break;
	}

	/* Determine whether to insert a new one or replace the old one */
	GFXVectorIterator it = _gfx_pipeline_find_attachment(
		internal,
		att.attachment
	);

	int new;

	if(it == internal->attachments.end) new = 1;
	else new = ((GFX_Attachment*)it)->attachment != att.attachment ? 1 : 0;

	/* Now actually insert one or replace the old one */
	if(new) gfx_vector_insert(&internal->attachments, &att, it);
	else *((GFX_Attachment*)it) = att;

	/* Send attachment to OGL */
	_gfx_pipeline_init_attachment(
		internal->fbo,
		&att,
		GFX_CONT_AS_ARG
	);

	return 1;
}

/******************************************************/
GFXPipe* gfx_pipeline_push_bucket(

		GFXPipeline*    pipeline,
		unsigned char   bits)
{
	/* Create the pipe and push it */
	GFX_Pipe* pipe = _gfx_pipe_create_bucket(pipeline, bits);
	if(!pipe) return NULL;

	_gfx_pipeline_push_pipe(pipe);

	return &pipe->ptr;
}

/******************************************************/
GFXPipe* gfx_pipeline_push_process(

		GFXPipeline*  pipeline,
		GFXWindow*    target,
		int           swap)
{
	/* Create the pipe and push it */
	GFX_Pipe* pipe = _gfx_pipe_create_process(pipeline, target, swap);
	if(!pipe) return NULL;

	_gfx_pipeline_push_pipe(pipe);

	return &pipe->ptr;
}

/******************************************************/
void gfx_pipeline_unlink_all(

		GFXPipeline* pipeline)
{
	GFX_Pipeline* internal = (GFX_Pipeline*)pipeline;

	/* Splice entire range into unlinked pipes */
	if(internal->first)
	{
		if(!internal->unlinked)
			internal->unlinked = internal->first;

		else gfx_list_splice_range_after(
			(GFXList*)internal->first,
			(GFXList*)internal->last,
			(GFXList*)internal->unlinked
		);

		internal->first = NULL;
		internal->last = NULL;
		internal->current = NULL;
	}
}

/******************************************************/
void gfx_pipeline_unlink(

		GFXPipe* pipe)
{
	GFX_Pipe* internal =
		GFX_PTR_SUB_BYTES(pipe, offsetof(GFX_Pipe, ptr));
	GFX_Pipeline* pipeline =
		(GFX_Pipeline*)internal->pipeline;

	/* Fix current and unlink */
	if(pipeline->current == internal) pipeline->current =
		(GFX_Pipe*)internal->node.next;
	GFX_Pipe* new =
		(GFX_Pipe*)gfx_list_unsplice((GFXList*)internal);

	/* Replace if necessary */
	if(pipeline->first == internal)
		pipeline->first = new;
	if(pipeline->last == internal)
		pipeline->last = new;
	if(pipeline->unlinked == internal)
		pipeline->unlinked = new;

	/* Splice into unlinked pipes */
	if(!pipeline->unlinked)
		pipeline->unlinked = internal;

	else gfx_list_splice_after(
		(GFXList*)internal,
		(GFXList*)pipeline->unlinked
	);
}

/******************************************************/
void gfx_pipeline_move(

		GFXPipe*  pipe,
		GFXPipe*  after)
{
	GFX_Pipe* int1 = GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr));

	GFX_Pipe* int2 = after ? GFX_PTR_SUB_BYTES(
		after,
		offsetof(GFX_Pipe, ptr)) : NULL;

	if(int1 != int2 && ((GFX_Pipe*)int1->node.previous != int2 || !int2))
	{
		/* Check if it can be moved */
		if(int2) if(int1->pipeline != int2->pipeline) return;

		GFX_Pipeline* pl = (GFX_Pipeline*)int1->pipeline;

		/* Reconstruct pointers */
		if(pl->unlinked == int1)
			pl->unlinked = (GFX_Pipe*)int1->node.next;
		else
		{
			if(pl->first == int1)
				pl->first = (GFX_Pipe*)int1->node.next;
			if(pl->last == int1)
				pl->last = (GFX_Pipe*)int1->node.previous;
			if(pl->current == int1)
				pl->current = (GFX_Pipe*)int1->node.next;
		}

		/* Move it */
		if(int2)
		{
			gfx_list_splice_after(
				(GFXList*)int1,
				(GFXList*)int2
			);

			if(pl->last == int2) pl->last = int1;
		}

		else if(pl->first)
		{
			gfx_list_splice_before(
				(GFXList*)int1,
				(GFXList*)pl->first
			);

			pl->first = int1;
		}

		else
		{
			gfx_list_unsplice(
				(GFXList*)int1
			);

			pl->first = int1;
			pl->last = int1;
		}
	}
}

/******************************************************/
void gfx_pipeline_swap(

		GFXPipe*  pipe1,
		GFXPipe*  pipe2)
{
	GFX_Pipe* int1 = GFX_PTR_SUB_BYTES(
		pipe1,
		offsetof(GFX_Pipe, ptr));

	GFX_Pipe* int2 = GFX_PTR_SUB_BYTES(
		pipe2,
		offsetof(GFX_Pipe, ptr));

	/* Check if they can be swapped */
	if(int1->pipeline == int2->pipeline)
	{
		GFX_Pipeline* pl = (GFX_Pipeline*)int1->pipeline;

		/* Reconstruct pointers */
		if(pl->first == int1)
			pl->first = int2;
		else if(pl->first == int2)
			pl->first = int1;

		if(pl->last == int1)
			pl->last = int2;
		else if(pl->last == int2)
			pl->last = int1;

		if(pl->current == int1)
			pl->current = int2;
		else if(pl->current == int2)
			pl->current = int1;

		if(pl->unlinked == int1)
			pl->unlinked = int2;
		else if(pl->unlinked == int2)
			pl->unlinked = int1;

		/* Swap list elements */
		gfx_list_swap((GFXList*)int1, (GFXList*)int2);
	}
}

/******************************************************/
void gfx_pipeline_relink(

		size_t     num,
		GFXPipe**  pipes)
{
	if(num)
	{
		/* Get pipeline and check if they all have the same */
		GFXPipeline* pipeline = ((GFX_Pipe*)GFX_PTR_SUB_BYTES(
			pipes[0],
			offsetof(GFX_Pipe, ptr)))->pipeline;

		size_t ch = num;

		while(ch > 1)
		{
			GFXPipe* pipe = pipes[--ch];
			if(((GFX_Pipe*)GFX_PTR_SUB_BYTES(
				pipe,
				offsetof(GFX_Pipe, ptr)))->pipeline != pipeline)
			{
				return;
			}
		}

		/* Unlink and relink all pipes */
		gfx_pipeline_unlink_all(pipeline);
		while(num) gfx_pipeline_move(pipes[--num], NULL);
	}
}

/******************************************************/
void gfx_pipeline_remove(

		GFXPipe* pipe)
{
	GFX_Pipe* internal = GFX_PTR_SUB_BYTES(
		pipe,
		offsetof(GFX_Pipe, ptr)
	);

	GFX_Pipeline* pipeline =
		(GFX_Pipeline*)internal->pipeline;

	/* Fix current and erase */
	if(pipeline->current == internal)
		pipeline->current = (GFX_Pipe*)internal->node.next;

	GFX_Pipe* new = _gfx_pipe_free(internal);

	/* Replace if necessary */
	if(pipeline->first == internal)
		pipeline->first = new;
	if(pipeline->last == internal)
		pipeline->last = new;
	if(pipeline->unlinked == internal)
		pipeline->unlinked = new;
}

/******************************************************/
void gfx_pipeline_execute(

		GFXPipeline*  pipeline,
		size_t        num)
{
	GFX_CONT_INIT();

	GFX_Pipeline* internal = (GFX_Pipeline*)pipeline;

	/* Bind as framebuffer and set viewport */
	_gfx_gl_pipeline_bind(
		GL_DRAW_FRAMEBUFFER,
		internal->fbo,
		GFX_CONT_AS_ARG);

	_gfx_states_set_viewport(
		pipeline->viewport,
		GFX_CONT_AS_ARG);

	/* Iterate over all pipes */
	int nolimit = !num;
	GFX_Pipe* pipe = internal->current ?
		internal->current : internal->first;

	while(pipe && (nolimit | num--))
	{
		switch(pipe->type)
		{
			case GFX_PIPE_BUCKET :
				_gfx_bucket_process(
					pipe->ptr.bucket,
					&pipe->state,
					GFX_CONT_AS_ARG
				);
				break;

			case GFX_PIPE_PROCESS :
				_gfx_pipe_process_execute(
					pipe->ptr.process,
					&pipe->state,
					GFX_CONT_AS_ARG
				);
				break;
		}

		pipe = (GFX_Pipe*)pipe->node.next;
	}

	/* Update current */
	internal->current = pipe;
}
