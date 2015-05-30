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

#include "groufix.h"
#include "groufix/core/internal.h"

/******************************************************/
/* Starting point of time */
static uint64_t _gfx_time_start = 0;


/******************************************************/
int gfx_is_extension_supported(

		GFXExtension extension)
{
	GFX_Context* context = _gfx_context_get_current();
	if(!context || extension >= GFX_EXT_COUNT) return 0;

	return context->ext[extension];
}

/******************************************************/
int gfx_get_limit(

		GFXLimit limit)
{
	GFX_Context* context = _gfx_context_get_current();
	if(!context || limit >= GFX_LIM_COUNT) return -1;

	return context->lim[limit];
}

/******************************************************/
int gfx_init(

		GFXContext    context,
		GFXErrorMode  errors)
{
	/* Always debug */
#ifndef NDEBUG
	errors = GFX_ERROR_MODE_DEBUG;
#endif

	/* Initialize errors, platform and context manager */
	if(_gfx_errors_init(errors))
	{
		if(_gfx_platform_init())
		{
			if(_gfx_context_manager_init(context))
			{
				/* Get starting point of time */
				_gfx_platform_init_timer();
				_gfx_time_start = _gfx_platform_get_time();

				return 1;
			}

			_gfx_platform_terminate();
		}

		_gfx_errors_terminate();
	}

	return 0;
}

/******************************************************/
int gfx_poll_events(void)
{
	return _gfx_platform_poll_events();
}

/******************************************************/
double gfx_get_time(void)
{
	return (double)(_gfx_platform_get_time() - _gfx_time_start) *
		_gfx_platform_get_time_resolution();
}

/******************************************************/
void gfx_set_time(

		double time)
{
	_gfx_time_start = _gfx_platform_get_time() -
		(uint64_t)(time / _gfx_platform_get_time_resolution());
}

/******************************************************/
void gfx_terminate(void)
{
	/* Cleanup shared buffers */
	gfx_shared_buffer_cleanup();

	/* Terminate */
	_gfx_errors_terminate();
	_gfx_context_manager_terminate();
	_gfx_platform_terminate();
}
