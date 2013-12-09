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

#include "groufix/errors.h"
#include "groufix/containers/deque.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Maximum number of errors stored */
static unsigned int _gfx_errors_maximum = GFX_MAX_ERRORS_DEFAULT;

/* Stored Errors */
static GFXDeque* _gfx_errors = NULL;

/******************************************************/
static GFXError* _gfx_errors_last(void)
{
	if(!_gfx_errors) return NULL;

	if(_gfx_errors->begin == _gfx_errors->end) return NULL;
	return (GFXError*)_gfx_errors->begin;
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

	GFXDequeIterator it;
	for(it = _gfx_errors->begin; it != _gfx_errors->end; it = gfx_deque_next(_gfx_errors, it))
		if(((GFXError*)it)->code == code) break;

	return it != _gfx_errors->end;
}

/******************************************************/
void gfx_errors_pop(void)
{
	GFXError* err = _gfx_errors_last();
	if(err)
	{
		/* Make sure to free it properly */
		free((char*)err->description);
		gfx_deque_pop_front(_gfx_errors);
	}
}

/******************************************************/
void gfx_errors_push(GFXErrorCode code, const char* description)
{
	/* Allocate */
	if(!_gfx_errors)
	{
		_gfx_errors = gfx_deque_create(sizeof(GFXError));
		if(!_gfx_errors) return;

		/* Reserve right away */
		gfx_deque_reserve(_gfx_errors, _gfx_errors_maximum);
	}
	else if(gfx_deque_get_size(_gfx_errors) == _gfx_errors_maximum)
	{
		gfx_deque_pop_back(_gfx_errors);
	}

	/* Construct an error */
	GFXError error;
	error.code = code;
	error.description = NULL;

	/* Copy the description */
	if(description)
	{
		char* des = malloc(sizeof(char) * (strlen(description) + 1));
		strcpy(des, description);

		error.description = des;
	}

	gfx_deque_push_front(_gfx_errors, &error);
}

/******************************************************/
void gfx_errors_empty(void)
{
	if(_gfx_errors)
	{
		/* Free all descriptions */
		GFXDequeIterator it;
		for(it = _gfx_errors->begin; it != _gfx_errors->end; it = gfx_deque_next(_gfx_errors, it))
			free((char*)((GFXError*)it)->description);

		gfx_deque_free(_gfx_errors);
		_gfx_errors = NULL;
	}
}

/******************************************************/
void gfx_errors_set_maximum(unsigned int max)
{
	_gfx_errors_maximum = max;

	/* Either deallocate or reserve */
	if(!max) gfx_errors_empty();
	else if(_gfx_errors)
	{
		/* Remove errors */
		while(gfx_deque_get_size(_gfx_errors) > max)
		{
			GFXDequeIterator it = gfx_deque_previous(_gfx_errors, _gfx_errors->end);
			free((char*)((GFXError*)it)->description);

			gfx_deque_pop_back(_gfx_errors);
		}

		/* Reserve the memory */
		gfx_deque_reserve(_gfx_errors, max);
	}
}
