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

#include "groufix/core/renderer.h"
#include "groufix.h"

/******************************************************/
/* Starting point of time */
static uint64_t _gfx_time_start = 0;


/******************************************************/
int gfx_is_extension_supported(

		GFXExtension extension)
{
	GFX_Window* window = _gfx_window_get_current();
	if(!window || extension >= GFX_EXT_COUNT) return 0;

	return window->ext[extension];
}

/******************************************************/
int gfx_get_limit(

		GFXLimit limit)
{
	GFX_Window* window = _gfx_window_get_current();
	if(!window || limit >= GFX_LIM_COUNT) return -1;

	return window->lim[limit];
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

	/* Initialize platform */
	if(!_gfx_platform_init()) return 0;

	/* Initialize window manager */
	if(!_gfx_window_manager_init(context, errors))
	{
		_gfx_platform_terminate();
		return 0;
	}

	/* Get starting point of time */
	_gfx_platform_init_timer();
	_gfx_time_start = _gfx_platform_get_time();

	return 1;
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
	_gfx_window_manager_terminate();
	_gfx_platform_terminate();

	/* Empty error queue */
	gfx_errors_empty();
}
