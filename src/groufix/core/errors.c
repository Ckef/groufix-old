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
#include "groufix/core/renderer.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Internal error */
typedef struct GFX_Error
{
	GFXErrorCode  code;
	char*         description; /* Can be NULL */

} GFX_Error;


/* Maximum number of errors stored */
static size_t _gfx_errors_maximum = GFX_MAX_ERRORS_DEFAULT;


/* Stored Errors */
static GFXDeque* _gfx_errors = NULL;


/******************************************************/
static void _gfx_errors_poll(void)
{
	GFX_WIND_INIT();

	/* Ignore error mode if debugging */
#ifdef NDEBUG
	if(_gfx_window_manager_get_error_mode() == GFX_ERROR_MODE_DEBUG)
	{
#endif

		/* Loop over all errors */
		GLenum err = GFX_REND_GET.GetError();
		while(err != GL_NO_ERROR)
		{
			gfx_errors_push(err, "[DEBUG] An OpenGL error occurred.");
			err = GFX_REND_GET.GetError();
		}

#ifdef NDEBUG
	}
#endif
}

/******************************************************/
static GFX_Error* _gfx_errors_last(void)
{
	_gfx_errors_poll();

	if(!_gfx_errors) return NULL;

	if(_gfx_errors->begin == _gfx_errors->end) return NULL;
	return (GFX_Error*)_gfx_errors->begin;
}

/******************************************************/
unsigned int gfx_get_num_errors(void)
{
	_gfx_errors_poll();

	if(!_gfx_errors) return 0;
	return gfx_deque_get_size(_gfx_errors);
}

/******************************************************/
int gfx_errors_peek(

		GFXError* error)
{
	GFX_Error* err = _gfx_errors_last();
	if(!err) return 0;

	error->code = err->code;
	error->description = err->description;

	return 1;
}

/******************************************************/
int gfx_errors_find(

		GFXErrorCode code)
{
	_gfx_errors_poll();

	if(!_gfx_errors) return 0;

	GFXDequeIterator it;
	for(
		it = _gfx_errors->begin;
		it != _gfx_errors->end;
		it = gfx_deque_next(_gfx_errors, it))
	{
		if(((GFX_Error*)it)->code == code) break;
	}

	return it != _gfx_errors->end;
}

/******************************************************/
void gfx_errors_pop(void)
{
	GFX_Error* err = _gfx_errors_last();
	if(err)
	{
		/* Make sure to free it properly */
		free(err->description);
		gfx_deque_pop_begin(_gfx_errors);
	}
}

/******************************************************/
void gfx_errors_push(

		GFXErrorCode  code,
		const char*   description)
{
	/* Allocate */
	if(!_gfx_errors)
	{
		_gfx_errors = gfx_deque_create(sizeof(GFX_Error));
		if(!_gfx_errors) return;

		/* Reserve right away */
		gfx_deque_reserve(_gfx_errors, _gfx_errors_maximum);
	}
	else if(gfx_deque_get_size(_gfx_errors) == _gfx_errors_maximum)
	{
		gfx_deque_pop_end(_gfx_errors);
	}

	/* Construct an error */
	GFX_Error error;
	error.code = code;
	error.description = NULL;

	/* Copy the description */
	if(description)
	{
		char* des = malloc(strlen(description) + 1);
		if(des) strcpy(des, description);

		error.description = des;
	}

	gfx_deque_push_begin(_gfx_errors, &error);
}

/******************************************************/
void gfx_errors_empty(void)
{
	if(_gfx_errors)
	{
		/* Free all descriptions */
		GFXDequeIterator it;
		for(
			it = _gfx_errors->begin;
			it != _gfx_errors->end;
			it = gfx_deque_next(_gfx_errors, it))
		{
			free(((GFX_Error*)it)->description);
		}

		gfx_deque_free(_gfx_errors);
		_gfx_errors = NULL;
	}
}

/******************************************************/
void gfx_errors_set_maximum(

		size_t max)
{
	_gfx_errors_maximum = max;

	/* Either deallocate or reserve */
	if(!max) gfx_errors_empty();
	else if(_gfx_errors)
	{
		/* Remove errors */
		while(gfx_deque_get_size(_gfx_errors) > max)
		{
			GFXDequeIterator it = gfx_deque_previous(
				_gfx_errors,
				_gfx_errors->end
			);

			free(((GFX_Error*)it)->description);
			gfx_deque_pop_end(_gfx_errors);
		}

		/* Reserve the memory */
		gfx_deque_reserve(_gfx_errors, max);
	}
}
