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

#include "groufix.h"
#include "groufix/core/internal.h"

/******************************************************/
/** Starting point of time */
static uint64_t _gfx_time_start = 0;


/******************************************************/
int gfx_is_extension_supported(

		GFXExtension extension)
{
	if(extension >= GFX_EXT_COUNT) return 0;

	GFX_CONT_INIT(0);

	return GFX_CONT_GET.ext[extension];
}

/******************************************************/
int gfx_get_limit(

		GFXLimit limit)
{
	if(limit >= GFX_LIM_COUNT) return -1;

	GFX_CONT_INIT(-1);

	return GFX_CONT_GET.lim[limit];
}

/******************************************************/
int gfx_init(

		GFXContext        context,
		GFXErrorMode      errors,
		GFXTerminateFunc  term)
{
	/* Always debug */
#ifndef NDEBUG
	errors = GFX_ERROR_MODE_DEBUG;
#endif

	/* Set termination callback */
	_gfx_terminate_func = term;

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
	_gfx_context_manager_terminate();
	_gfx_platform_terminate();
	_gfx_errors_terminate();
}
