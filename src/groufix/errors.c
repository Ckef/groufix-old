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

/******************************************************/
/* Maximum number of errors stored */
static size_t _gfx_errors_maximum = 3; /* feels about right */

/* Stored Errors */
static Deque* _gfx_errors = NULL;

/* All human readable error messages */
static const char* _gfx_error_messages[] = {

	"", /* No error has been recorded. */

	"An unknown error has been recorded.",
	"An unacceptable value is specified for an enumerated argument.",
	"A numeric argument is out of range.",
	"The specified operation is not allowed in the current state.",
	"The framebuffer object is not complete.",
	"There is not enough memory left to execute the command.",
	"An attempt has been made to perform an operation that would cause an internal stack to underflow.",
	"An attempt has been made to perform an operation that would cause an internal stack to overflow.",
	"An attempt to access inaccessible memory has been made."
};

/******************************************************/
int gfx_errors_peek(GFXError* error)
{
	if(!_gfx_errors) return 0;

	if(_gfx_errors->begin == _gfx_errors->end) return 0;
	*error = *(GFXError*)_gfx_errors->begin;

	return 1;
}

/******************************************************/
int gfx_errors_pop(GFXError* error)
{
	if(!gfx_errors_peek(error)) return 0;
	deque_pop_front(_gfx_errors);

	return 1;
}

/******************************************************/
void gfx_errors_push(GFXErrorCode error)
{
	if(_gfx_errors_maximum && error > GFX_NO_ERROR && error < GFX_NUM_ERRORS)
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
		GFXError err;
		err.code = error;
		err.message = _gfx_error_messages[error];

		deque_push_front(_gfx_errors, &err);
	}
}

/******************************************************/
void gfx_errors_empty(void)
{
	deque_free(_gfx_errors);
	_gfx_errors = NULL;
}

/******************************************************/
void gfx_errors_set_maximum(size_t max)
{
	_gfx_errors_maximum = max;

	/* Either deallocate or reserve */
	if(!max) gfx_errors_empty();
	else if(_gfx_errors)
	{
		deque_shrink(_gfx_errors);
		deque_reserve(_gfx_errors, max);
	}
}
