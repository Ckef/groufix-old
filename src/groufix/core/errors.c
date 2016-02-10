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

#include "groufix/core/renderer.h"
#include "groufix/core/threading.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/******************************************************/
/** Internal error */
typedef struct GFX_Error
{
	GFXErrorCode  code;
	char*         description; /* Can be NULL */

} GFX_Error;


/** Maximum number of errors stored */
static size_t _gfx_errors_maximum = GFX_MAX_ERRORS_DEFAULT;


/** Error mode */
static GFXErrorMode _gfx_error_mode;


/** Stored Errors */
static GFXDeque _gfx_errors;


/** Synchronize any access */
static GFX_PlatformMutex  _gfx_error_mutex;


/******************************************************/
static inline void _gfx_errors_poll(void)
{
	if(_gfx_error_mode == GFX_ERROR_MODE_DEBUG)
		_gfx_renderer_poll_errors();
}

/******************************************************/
static GFX_Error* _gfx_errors_last(void)
{
	/* Lock but don't unlock as to not have to lock again */
	_gfx_platform_mutex_lock(&_gfx_error_mutex);

	_gfx_errors_poll();

	if(_gfx_errors.begin == _gfx_errors.end) return NULL;
	return (GFX_Error*)_gfx_errors.begin;
}

/******************************************************/
int _gfx_errors_init(

		GFXErrorMode mode)
{
	/* Initialize mutex */
	if(!_gfx_platform_mutex_init(&_gfx_error_mutex))
		return 0;

	_gfx_error_mode = mode;
	gfx_deque_init(&_gfx_errors, sizeof(GFX_Error));

	return 1;
}

/******************************************************/
GFXErrorMode _gfx_errors_get_mode(void)
{
	return _gfx_error_mode;
}

/******************************************************/
void _gfx_errors_terminate(void)
{
	gfx_deque_clear(&_gfx_errors);
	_gfx_platform_mutex_clear(&_gfx_error_mutex);
}

/******************************************************/
unsigned int gfx_get_num_errors(void)
{
	_gfx_platform_mutex_lock(&_gfx_error_mutex);

	_gfx_errors_poll();
	size_t size = gfx_deque_get_size(&_gfx_errors);

	_gfx_platform_mutex_unlock(&_gfx_error_mutex);

	return size;
}

/******************************************************/
int gfx_errors_peek(

		GFXError* error)
{
	GFX_Error* err = _gfx_errors_last();
	if(err)
	{
		error->code = err->code;
		error->description = err->description;
	}

	_gfx_platform_mutex_unlock(&_gfx_error_mutex);

	return err != NULL;
}

/******************************************************/
int gfx_errors_find(

		GFXErrorCode code)
{
	_gfx_platform_mutex_lock(&_gfx_error_mutex);

	_gfx_errors_poll();

	GFX_Error* it;
	for(
		it = _gfx_errors.begin;
		it != _gfx_errors.end;
		it = gfx_deque_next(&_gfx_errors, it))
	{
		if(it->code == code) break;
	}

	int ret = it != _gfx_errors.end;

	_gfx_platform_mutex_unlock(&_gfx_error_mutex);

	return ret;
}

/******************************************************/
void gfx_errors_pop(void)
{
	GFX_Error* err = _gfx_errors_last();
	if(err)
	{
		/* Make sure to free it properly */
		free(err->description);
		gfx_deque_pop_begin(&_gfx_errors);
	}

	_gfx_platform_mutex_unlock(&_gfx_error_mutex);
}

/******************************************************/
void gfx_errors_push(

		GFXErrorCode  code,
		const char*   description,
		...)
{
	_gfx_platform_mutex_lock(&_gfx_error_mutex);

	if(gfx_deque_get_size(&_gfx_errors) == _gfx_errors_maximum)
		gfx_deque_pop_end(&_gfx_errors);

	/* Construct an error */
	GFX_Error error =
	{
		.code = code,
		.description = NULL
	};

	if(description)
	{
		/* Note the 'vsnprintf' call is not in std c99 */
		/* However it is in the win32 and posix APIs */

		/* Format the description */
		va_list vl;
		va_start(vl, description);

		int size = vsnprintf(NULL, 0, description, vl);

		va_end(vl);

		/* And copy it */
		if(size++)
		{
			va_start(vl, description);

			error.description = malloc(size);

			if(error.description)
				vsnprintf(error.description, size, description, vl);

			va_end(vl);
		}
	}

	gfx_deque_push_begin(&_gfx_errors, &error);

	_gfx_platform_mutex_unlock(&_gfx_error_mutex);
}

/******************************************************/
void gfx_errors_output(

		const char* description,
		...)
{
	/* Format the description */
	va_list vl;
	va_start(vl, description);

	fprintf(stderr, description, vl);

	va_end(vl);

	/* Append newline and flush */
	fputc('\n', stderr);
	fflush(stderr);
}

/******************************************************/
void gfx_errors_empty(void)
{
	_gfx_platform_mutex_lock(&_gfx_error_mutex);

	/* Free all descriptions */
	GFX_Error* it;
	for(
		it = _gfx_errors.begin;
		it != _gfx_errors.end;
		it = gfx_deque_next(&_gfx_errors, it))
	{
		free(it->description);
	}

	gfx_deque_clear(&_gfx_errors);

	_gfx_platform_mutex_unlock(&_gfx_error_mutex);
}

/******************************************************/
void gfx_errors_set_maximum(

		size_t max)
{
	_gfx_platform_mutex_lock(&_gfx_error_mutex);

	_gfx_errors_maximum = max;

	/* Remove errors */
	while(gfx_deque_get_size(&_gfx_errors) > max)
	{
		GFX_Error* it = gfx_deque_previous(&_gfx_errors, _gfx_errors.end);
		free(it->description);

		gfx_deque_pop_end(&_gfx_errors);
	}

	/* Empty errors if necessary */
	if(!max) gfx_deque_clear(&_gfx_errors);

	_gfx_platform_mutex_unlock(&_gfx_error_mutex);
}
