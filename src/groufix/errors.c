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

#include "groufix/errors.h"
#include "groufix/containers/deque.h"
#include "groufix/utils.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Maximum number of errors stored */
static size_t _gfx_errors_maximum = 3; /* feels about right */

/* Stored Errors */
static Deque* _gfx_errors = NULL;

/******************************************************/
static GFXError* _gfx_errors_last(void)
{
	if(!_gfx_errors) return NULL;

	if(_gfx_errors->begin == _gfx_errors->end) return NULL;
	return (GFXError*)_gfx_errors->begin;
}

/******************************************************/
static int _gfx_errors_compare(const DequeIterator it, const void* value)
{
	return ((GFXError*)it)->code == (GFXErrorCode)VOID_TO_UINT(value);
}

/******************************************************/
int gfx_errors_peek(GFXError* error)
{
	GFXError* err = _gfx_errors_last();
	if(!err) return 0;

	*error = *err;

	return 1;
}

/******************************************************/
int gfx_errors_find(GFXErrorCode code)
{
	if(!_gfx_errors) return 0;

	return deque_find(_gfx_errors, UINT_TO_VOID(code), _gfx_errors_compare) != _gfx_errors->end;
}

/******************************************************/
void gfx_errors_pop(void)
{
	GFXError* err = _gfx_errors_last();
	if(err)
	{
		/* Make sure to free it properly */
		free(err->description);
		deque_pop_front(_gfx_errors);
	}
}

/******************************************************/
void gfx_errors_push(GFXErrorCode code, const char* description)
{
	/* Allocate */
	if(!_gfx_errors)
	{
		_gfx_errors = deque_create(sizeof(GFXError));
		if(!_gfx_errors) return;

		/* Reserve right away */
		deque_reserve(_gfx_errors, _gfx_errors_maximum);
	}
	else if(deque_get_size(_gfx_errors) == _gfx_errors_maximum)
	{
		deque_pop_back(_gfx_errors);
	}

	/* Construct an error */
	GFXError error;
	error.code = code;
	error.description = NULL;

	/* Copy the description */
	if(description)
	{
		error.description = (char*)malloc(strlen(description) + 1);
		strcpy(error.description, description);
	}

	deque_push_front(_gfx_errors, &error);
}

/******************************************************/
void gfx_errors_empty(void)
{
	if(_gfx_errors)
	{
		/* Free all descriptions */
		DequeIterator it;
		for(it = _gfx_errors->begin; it != _gfx_errors->end; it = deque_next(_gfx_errors, it))
			free(((GFXError*)it)->description);

		deque_free(_gfx_errors);
		_gfx_errors = NULL;
	}
}

/******************************************************/
void gfx_errors_set_maximum(size_t max)
{
	_gfx_errors_maximum = max;

	/* Either deallocate or reserve */
	if(!max) gfx_errors_empty();
	else if(_gfx_errors)
	{
		/* Remove errors */
		while(deque_get_size(_gfx_errors) > max) deque_pop_back(_gfx_errors);

		/* Reallocate the memory used */
		deque_shrink(_gfx_errors);
		deque_reserve(_gfx_errors, max);
	}
}
