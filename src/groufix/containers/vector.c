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

#include "groufix/containers/vector.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define GFX_VECTOR_MSB (~(SIZE_MAX >> 1))

/******************************************************/
static inline size_t _gfx_vector_get_max_capacity(

		size_t size)
{
	if(size > GFX_VECTOR_MSB) return size;

	size_t cap = 1;
	while(cap < size) cap <<= 1;

	return cap;
}

/******************************************************/
static int _gfx_vector_realloc(

		GFXVector*  vector,
		size_t      size,
		size_t      capacity)
{
	/* Make sure to check if it worked */
	void* new = realloc(vector->begin, capacity);
	if(!new) return 0;

	/* Set new properties */
	vector->begin = new;
	vector->end = GFX_PTR_ADD_BYTES(vector->begin, size);
	vector->capacity = capacity;

	return 1;
}

/******************************************************/
GFXVector* gfx_vector_create(

		size_t elementSize)
{
	/* Create a new vector */
	GFXVector* vector = calloc(1, sizeof(GFXVector));
	if(vector) vector->elementSize = elementSize;

	return vector;
}

/******************************************************/
GFXVector* gfx_vector_create_from_buffer(

		size_t       elementSize,
		size_t       numElements,
		const void*  buff)
{
	/* Create a new vector */
	GFXVector* vector = gfx_vector_create(elementSize);
	if(!vector) return NULL;

	/* Allocate and copy */
	size_t size = elementSize * numElements;
	if(!_gfx_vector_realloc(vector, size, _gfx_vector_get_max_capacity(size)))
	{
		free(vector);
		return NULL;
	}
	else if(buff) memcpy(vector->begin, buff, size);

	return vector;
}

/******************************************************/
GFXVector* gfx_vector_create_copy(

		GFXVector* src)
{
	return gfx_vector_create_from_buffer(
		src->elementSize,
		gfx_vector_get_size(src),
		src->begin);
}

/******************************************************/
void gfx_vector_free(

		GFXVector* vector)
{
	if(vector)
	{
		free(vector->begin);
		free(vector);
	}
}

/******************************************************/
void gfx_vector_init(

		GFXVector*  vector,
		size_t      elementSize)
{
	memset(vector, 0, sizeof(GFXVector));
	vector->elementSize = elementSize;
}

/******************************************************/
void gfx_vector_init_from_buffer(

		GFXVector*   vector,
		size_t       elementSize,
		size_t       numElements,
		const void*  buff)
{
	/* Init the vector */
	gfx_vector_init(vector, elementSize);

	/* Allocate and copy */
	size_t size = elementSize * numElements;
	if(!_gfx_vector_realloc(vector, size, _gfx_vector_get_max_capacity(size)))
	{
		gfx_vector_clear(vector);
	}
	else if(buff) memcpy(vector->begin, buff, size);
}

/******************************************************/
void gfx_vector_init_copy(

		GFXVector*  vector,
		GFXVector*  src)
{
	gfx_vector_init_from_buffer(
		vector, src->elementSize,
		gfx_vector_get_size(src),
		src->begin);
}

/******************************************************/
void gfx_vector_clear(

		GFXVector* vector)
{
	free(vector->begin);
	vector->begin = NULL;
	vector->end = NULL;

	vector->capacity = 0;
}

/******************************************************/
int gfx_vector_reserve(

		GFXVector*  vector,
		size_t      numElements)
{
	size_t oldSize = GFX_PTR_DIFF(vector->begin, vector->end);
	size_t newSize = vector->elementSize * numElements;

	if(newSize > vector->capacity) return _gfx_vector_realloc(
		vector,
		oldSize,
		_gfx_vector_get_max_capacity(newSize));

	return 1;
}

/******************************************************/
GFXVectorIterator gfx_vector_insert(

		GFXVector*         vector,
		const void*        element,
		GFXVectorIterator  pos)
{
	/* Get properties */
	size_t oldSize = GFX_PTR_DIFF(vector->begin, vector->end);
	size_t newSize = oldSize + vector->elementSize;
	size_t mov = GFX_PTR_DIFF(pos, vector->end);

	/* Reallocate if necessary */
	if(newSize > vector->capacity)
	{
		/* Get new capacity if empty */
		size_t cap = vector->capacity << 1;
		if(!cap) cap = _gfx_vector_get_max_capacity(newSize);

		if(!_gfx_vector_realloc(vector, newSize, cap)) return vector->end;
		pos = GFX_PTR_ADD_BYTES(vector->begin, oldSize - mov);
	}

	/* Set new end */
	else vector->end = GFX_PTR_ADD_BYTES(vector->begin, newSize);

	/* Move elements if inserting */
	if(mov) memmove(GFX_PTR_ADD_BYTES(pos, vector->elementSize), pos, mov);
	if(element) memcpy(pos, element, vector->elementSize);

	return pos;
}

/******************************************************/
GFXVectorIterator gfx_vector_insert_range(

		GFXVector*         vector,
		size_t             num,
		GFXVectorIterator  start,
		GFXVectorIterator  pos)
{
	/* Get properties */
	size_t diff = num * vector->elementSize;
	size_t oldSize = GFX_PTR_DIFF(vector->begin, vector->end);
	size_t newSize = oldSize + diff;
	size_t mov = GFX_PTR_DIFF(pos, vector->end);

	/* Copy to a temporary buffer */
	int8_t buff[diff];
	if(start) memcpy(buff, start, diff);

	/* Reallocate if necessary */
	if(newSize > vector->capacity)
	{
		if(!_gfx_vector_realloc(vector, newSize, _gfx_vector_get_max_capacity(newSize)))
			return vector->end;

		pos = GFX_PTR_ADD_BYTES(vector->begin, oldSize - mov);
	}

	/* Set new end */
	else vector->end = GFX_PTR_ADD_BYTES(vector->begin, newSize);

	/* Move elements if inserting */
	if(mov) memmove(GFX_PTR_ADD_BYTES(pos, diff), pos, mov);
	memcpy(pos, buff, diff);

	return pos;
}

/******************************************************/
GFXVectorIterator gfx_vector_erase_range(

		GFXVector*         vector,
		size_t             num,
		GFXVectorIterator  start)
{
	/* Nothing to erase */
	if(start == vector->end || !num) return vector->end;

	/* Get new properties */
	size_t diff = num * vector->elementSize;
	size_t oldSize = GFX_PTR_DIFF(vector->begin, vector->end);
	size_t toEnd = GFX_PTR_DIFF(start, vector->end);

	/* Boundaries! */
	if(diff > toEnd) diff = toEnd;
	size_t newSize = oldSize - diff;

	/* Deallocate if necessary */
	if(!newSize)
	{
		gfx_vector_clear(vector);
		return vector->end;
	}

	/* Move elements if necessary */
	size_t mov = toEnd - diff;
	if(mov) memmove(start, GFX_PTR_ADD_BYTES(start, diff), mov);

	/* Reallocate if necessary */
	/* Use upperbound/4 instead to avoid constant realloc */
	if(newSize < (vector->capacity >> 2))
	{
		_gfx_vector_realloc(vector, newSize, _gfx_vector_get_max_capacity(newSize));
		start = GFX_PTR_ADD_BYTES(vector->begin, oldSize - toEnd);
	}

	/* Set new end */
	else vector->end = GFX_PTR_ADD_BYTES(vector->begin, newSize);

	return start;
}
