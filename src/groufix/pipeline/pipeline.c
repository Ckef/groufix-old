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

#include "groufix/internal.h"

#include <stdlib.h>

/******************************************************/
/** Internal Pipeline */
struct GFX_Internal_Pipeline
{
	/* Super class */
	GFXPipeline pipeline;

	/* OpenGL handle */
	GLuint fbo;

	/* Not a shared resource */
	const GFX_Extensions* ext;
};

/******************************************************/
static void _gfx_pipeline_obj_free(void* object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Pipeline* pipeline = (struct GFX_Internal_Pipeline*)object;

	/* Destroy framebuffer */
	ext->DeleteFramebuffers(1, &pipeline->fbo);

	pipeline->ext = NULL;
	pipeline->fbo = 0;
	pipeline->pipeline.id = 0;
}

/******************************************************/
static void _gfx_pipeline_obj_save(void* object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Pipeline* pipeline = (struct GFX_Internal_Pipeline*)object;

	/* Destroy framebuffer */
	ext->DeleteFramebuffers(1, &pipeline->fbo);

	pipeline->ext = NULL;
	pipeline->fbo = 0;
}

/******************************************************/
static void _gfx_pipeline_obj_restore(void* object, const GFX_Extensions* ext)
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

		/* Delete FBO */
		if(internal->ext) internal->ext->DeleteFramebuffers(1, &internal->fbo);

		free(pipeline);
	}
}
