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

#include "groufix/containers/vector.h"
#include "groufix/utils.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
static size_t _gfx_vector_get_max_capacity(size_t size)
{
	size_t cap = 1;
	while(cap < size) cap <<= 1;

	return cap;
}

/******************************************************/
static int _gfx_vector_realloc(GFXVector* vector, size_t size, size_t capacity)
{
	/* Make sure to check if it worked */
	void* new = realloc(vector->begin, capacity);
	if(!new)
	{
		gfx_vector_clear(vector);
		return 0;
	}

	/* Set new properties */
	vector->begin = new;
	vector->end = PTR_ADD_BYTES(vector->begin, size);
	vector->capacity = capacity;

	return 1;
}

/******************************************************/
GFXVector* gfx_vector_create(size_t elementSize)
{
	/* Create a new vector */
	GFXVector* vector = calloc(1, sizeof(GFXVector));
	if(vector) vector->elementSize = elementSize;

	return vector;
}

/******************************************************/
GFXVector* gfx_vector_create_from_buffer(size_t elementSize, size_t numElements, const void* buff)
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
	memcpy(vector->begin, buff, size);

	return vector;
}

/******************************************************/
GFXVector* gfx_vector_create_copy(GFXVector* src)
{
	return gfx_vector_create_from_buffer(src->elementSize, gfx_vector_get_size(src), src->begin);
}

/******************************************************/
void gfx_vector_free(GFXVector* vector)
{
	if(vector)
	{
		free(vector->begin);
		free(vector);
	}
}

/******************************************************/
void gfx_vector_clear(GFXVector* vector)
{
	free(vector->begin);
	vector->begin = NULL;
	vector->end = NULL;

	vector->capacity = 0;
}

/******************************************************/
size_t gfx_vector_get_byte_size(GFXVector* vector)
{
	return PTR_DIFF(vector->begin, vector->end);
}

/******************************************************/
size_t gfx_vector_get_size(GFXVector* vector)
{
	return PTR_DIFF(vector->begin, vector->end) / vector->elementSize;
}

/******************************************************/
int gfx_vector_reserve(GFXVector* vector, size_t numElements)
{
	size_t oldSize = PTR_DIFF(vector->begin, vector->end);
	size_t newSize = vector->elementSize * numElements;
	if(newSize > vector->capacity)
		return _gfx_vector_realloc(vector, oldSize, _gfx_vector_get_max_capacity(newSize));

	return 1;
}

/******************************************************/
GFXVectorIterator gfx_vector_at(GFXVector* vector, size_t index)
{
	return PTR_ADD_BYTES(vector->begin, index * vector->elementSize);
}

/******************************************************/
GFXVectorIterator gfx_vector_next(GFXVector* vector, GFXVectorIterator it)
{
	return PTR_ADD_BYTES(it, vector->elementSize);
}

/******************************************************/
GFXVectorIterator gfx_vector_previous(GFXVector* vector, GFXVectorIterator it)
{
	return PTR_SUB_BYTES(it, vector->elementSize);
}

/******************************************************/
GFXVectorIterator gfx_vector_advance(GFXVector* vector, GFXVectorIterator it, int num)
{
	return PTR_ADD_BYTES(it, vector->elementSize * num);
}

/******************************************************/
GFXVectorIterator gfx_vector_insert(GFXVector* vector, const void* element, GFXVectorIterator pos)
{
	/* Get properties */
	size_t oldSize = PTR_DIFF(vector->begin, vector->end);
	size_t newSize = oldSize + vector->elementSize;
	size_t mov = PTR_DIFF(pos, vector->end);

	/* Reallocate if necessary */
	if(newSize > vector->capacity)
	{
		/* Get new capacity if empty */
		size_t cap = vector->capacity << 1;
		if(!cap) cap = _gfx_vector_get_max_capacity(newSize);

		if(!_gfx_vector_realloc(vector, newSize, cap)) return NULL;
		pos = PTR_ADD_BYTES(vector->begin, oldSize - mov);
	}

	/* Set new end */
	else vector->end = PTR_ADD_BYTES(vector->begin, newSize);

	/* Move elements if inserting */
	if(mov) memmove(PTR_ADD_BYTES(pos, vector->elementSize), pos, mov);
	memcpy(pos, element, vector->elementSize);

	return pos;
}

/******************************************************/
GFXVectorIterator gfx_vector_insert_at(GFXVector* vector, const void* element, size_t index)
{
	return gfx_vector_insert(vector, element, gfx_vector_at(vector, index));
}

/******************************************************/
GFXVectorIterator gfx_vector_insert_range(GFXVector* vector, size_t num, GFXVectorIterator start, GFXVectorIterator pos)
{
	/* Get properties */
	size_t diff = num * vector->elementSize;
	size_t oldSize = PTR_DIFF(vector->begin, vector->end);
	size_t newSize = oldSize + diff;
	size_t mov = PTR_DIFF(pos, vector->end);

	/* Copy to a temporary buffer */
	int8_t buff[diff];
	memcpy(buff, start, diff);

	/* Reallocate if necessary */
	if(newSize > vector->capacity)
	{
		if(!_gfx_vector_realloc(vector, newSize, _gfx_vector_get_max_capacity(newSize))) return NULL;
		pos = PTR_ADD_BYTES(vector->begin, oldSize - mov);
	}

	/* Set new end */
	else vector->end = PTR_ADD_BYTES(vector->begin, newSize);

	/* Move elements if inserting */
	if(mov) memmove(PTR_ADD_BYTES(pos, diff), pos, mov);
	memcpy(pos, buff, diff);

	return pos;
}

/******************************************************/
GFXVectorIterator gfx_vector_insert_range_at(GFXVector* vector, size_t num, GFXVectorIterator start, size_t index)
{
	return gfx_vector_insert_range(vector, num, start, gfx_vector_at(vector, index));
}

/******************************************************/
GFXVectorIterator gfx_vector_erase(GFXVector* vector, GFXVectorIterator pos)
{
	/* Call erase range to make sure a minimum capacity is reallocated */
	return gfx_vector_erase_range(vector, 1, pos);
}

/******************************************************/
GFXVectorIterator gfx_vector_erase_at(GFXVector* vector, size_t index)
{
	return gfx_vector_erase(vector, gfx_vector_at(vector, index));
}

/******************************************************/
GFXVectorIterator gfx_vector_erase_range(GFXVector* vector, size_t num, GFXVectorIterator start)
{
	/* Nothing to erase */
	if(start == vector->end || !num) return NULL;

	/* Get new properties */
	size_t diff = num * vector->elementSize;
	size_t oldSize = PTR_DIFF(vector->begin, vector->end);
	size_t toEnd = PTR_DIFF(start, vector->end);

	/* Boundaries! */
	if(diff > toEnd) diff = toEnd;
	size_t newSize = oldSize - diff;

	/* Deallocate if necessary */
	if(!newSize)
	{
		gfx_vector_clear(vector);
		return NULL;
	}

	/* Move elements if necessary */
	size_t mov = toEnd - diff;
	if(mov) memmove(start, PTR_ADD_BYTES(start, diff), mov);

	/* Reallocate if necessary */
	if(newSize < (vector->capacity >> 1))
	{
		if(!_gfx_vector_realloc(vector, newSize, _gfx_vector_get_max_capacity(newSize))) return NULL;
		start = PTR_ADD_BYTES(vector->begin, oldSize - toEnd);
	}

	/* Set new end */
	else vector->end = PTR_ADD_BYTES(vector->begin, newSize);

	return start;
}

/******************************************************/
GFXVectorIterator gfx_vector_erase_range_at(GFXVector* vector, size_t num, size_t index)
{
	return gfx_vector_erase_range(vector, num, gfx_vector_at(vector, index));
}
