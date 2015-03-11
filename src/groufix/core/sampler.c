/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/errors.h"
#include "groufix/core/internal.h"

#include <stdlib.h>

/******************************************************/
/* Internal sampler */
typedef struct GFX_Sampler
{
	/* Super class */
	GFXSampler sampler;

	/* Hidden data */
	GFX_RenderObjectID  id;
	unsigned int        references; /* Reference counter */
	GLuint              handle;     /* OpenGL handle */

} GFX_Sampler;


/******************************************************/
static void _gfx_sampler_obj_free(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_Sampler* sampler = (GFX_Sampler*)object;

	sampler->id = id;
	sampler->handle = 0;
}

/******************************************************/
static void _gfx_sampler_obj_save_restore(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_Sampler* sampler = (GFX_Sampler*)object;
	sampler->id = id;
}

/******************************************************/
/* vtable for render object part of the sampler */
static GFX_RenderObjectFuncs _gfx_sampler_obj_funcs =
{
	_gfx_sampler_obj_free,
	_gfx_sampler_obj_save_restore,
	_gfx_sampler_obj_save_restore
};

/******************************************************/
GLuint _gfx_sampler_get_handle(

		const GFXSampler* sampler)
{
	return ((const GFX_Sampler*)sampler)->handle;
}

/******************************************************/
GFXSampler* _gfx_sampler_create(

		const GFXSampler* values)
{
	GFX_WIND_INIT(NULL);

	/* Allocate new sampler */
	GFX_Sampler* samp = calloc(1, sizeof(GFX_Sampler));
	if(!samp)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Sampler could not be allocated."
		);
		return NULL;
	}

	if(GFX_WIND_GET.ext[GFX_EXT_SAMPLER_OBJECTS])
	{
		/* Register as object */
		samp->id = _gfx_render_object_register(
			&GFX_WIND_GET.objects,
			samp,
			&_gfx_sampler_obj_funcs
		);

		if(!samp->id.id)
		{
			free(samp);
			return NULL;
		}

		/* Allocate OGL resources */
		GFX_REND_GET.CreateSamplers(1, &samp->handle);
	}

	/* Initialize */
	samp->sampler = *values;
	samp->references = 1;

	return (GFXSampler*)samp;
}

/******************************************************/
int _gfx_sampler_reference(

		GFXSampler* sampler)
{
	GFX_Sampler* internal = (GFX_Sampler*)sampler;

	if(!(internal->references + 1))
	{
		/* Overflow error */
		gfx_errors_push(
			GFX_ERROR_OVERFLOW,
			"Overflow occurred during Sampler referencing."
		);
		return 0;
	}

	++internal->references;
	return 1;
}

/******************************************************/
void _gfx_sampler_free(

		GFXSampler* sampler)
{
	if(sampler)
	{
		GFX_Sampler* internal = (GFX_Sampler*)sampler;

		/* Check references */
		if(!(--internal->references))
		{
			GFX_WIND_INIT_UNSAFE;

			/* Unregister as object */
			_gfx_render_object_unregister(internal->id);

			if(!GFX_WIND_EQ(NULL))
			{
				/* Delete sampler object */
				_gfx_binder_unbind_sampler(
					internal->handle,
					GFX_WIND_AS_ARG
				);

				if(GFX_WIND_GET.ext[GFX_EXT_SAMPLER_OBJECTS])
					GFX_REND_GET.DeleteSamplers(
						1,
						&internal->handle
					);
			}

			free(sampler);
		}
	}
}
