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
static size_t _vector_get_max_capacity(size_t size)
{
	size_t cap = 1;
	while(cap < size) cap <<= 1;

	return cap;
}

/******************************************************/
static int _vector_realloc(Vector* vector, size_t size, size_t capacity)
{
	/* Make sure to check if it worked */
	void* new = realloc(vector->begin, capacity);
	if(!new)
	{
		vector_clear(vector);
		return 0;
	}

	/* Set new properties */
	vector->begin = new;
	vector->end = PTR_ADD_BYTES(vector->begin, size);
	vector->capacity = capacity;

	return 1;
}

/******************************************************/
Vector* vector_create(size_t elementSize)
{
	/* Create a new vector */
	Vector* vector = (Vector*)calloc(1, sizeof(Vector));
	if(vector) vector->elementSize = elementSize;

	return vector;
}

/******************************************************/
Vector* vector_create_from_buffer(size_t elementSize, size_t numElements, const void* buff)
{
	/* Create a new vector */
	Vector* vector = vector_create(elementSize);
	if(!vector) return NULL;

	/* Allocate and copy */
	size_t size = elementSize * numElements;
	if(!_vector_realloc(vector, size, _vector_get_max_capacity(size)))
	{
		free(vector);
		return NULL;
	}
	memcpy(vector->begin, buff, size);

	return vector;
}

/******************************************************/
Vector* vector_create_copy(Vector* src)
{
	return vector_create_from_buffer(src->elementSize, vector_get_size(src), src->begin);
}

/******************************************************/
void vector_free(Vector* vector)
{
	if(vector)
	{
		free(vector->begin);
		free(vector);
	}
}

/******************************************************/
void vector_clear(Vector* vector)
{
	free(vector->begin);
	vector->begin = NULL;
	vector->end = NULL;

	vector->capacity = 0;
}

/******************************************************/
size_t vector_get_byte_size(Vector* vector)
{
	return PTR_DIFF(vector->begin, vector->end);
}

/******************************************************/
size_t vector_get_size(Vector* vector)
{
	return PTR_DIFF(vector->begin, vector->end) / vector->elementSize;
}

/******************************************************/
int vector_reserve(Vector* vector, size_t numElements)
{
	size_t newSize = vector->elementSize * numElements;
	if(newSize > vector->capacity)
		return _vector_realloc(vector, newSize, _vector_get_max_capacity(newSize));

	return 1;
}

/******************************************************/
VectorIterator vector_at(Vector* vector, size_t index)
{
	return PTR_ADD_BYTES(vector->begin, index * vector->elementSize);
}

/******************************************************/
VectorIterator vector_next(Vector* vector, VectorIterator it)
{
	return PTR_ADD_BYTES(it, vector->elementSize);
}

/******************************************************/
VectorIterator vector_previous(Vector* vector, VectorIterator it)
{
	return PTR_SUB_BYTES(it, vector->elementSize);
}

/******************************************************/
VectorIterator vector_advance(Vector* vector, VectorIterator it, int num)
{
	return PTR_ADD_BYTES(it, vector->elementSize * num);
}

/******************************************************/
VectorIterator vector_insert(Vector* vector, const void* element, VectorIterator pos)
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
		if(!cap) cap = _vector_get_max_capacity(newSize);

		if(!_vector_realloc(vector, newSize, cap)) return NULL;
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
VectorIterator vector_insert_at(Vector* vector, const void* element, size_t index)
{
	return vector_insert(vector, element, vector_at(vector, index));
}

/******************************************************/
VectorIterator vector_insert_range(Vector* vector, size_t num, VectorIterator start, VectorIterator pos)
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
		if(!_vector_realloc(vector, newSize, _vector_get_max_capacity(newSize))) return NULL;
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
VectorIterator vector_insert_range_at(Vector* vector, size_t num, VectorIterator start, size_t index)
{
	return vector_insert_range(vector, num, start, vector_at(vector, index));
}

/******************************************************/
VectorIterator vector_erase(Vector* vector, VectorIterator pos)
{
	/* Call erase range to make sure a minimum capacity is reallocated */
	return vector_erase_range(vector, 1, pos);
}

/******************************************************/
VectorIterator vector_erase_at(Vector* vector, size_t index)
{
	return vector_erase(vector, vector_at(vector, index));
}

/******************************************************/
VectorIterator vector_erase_range(Vector* vector, size_t num, VectorIterator start)
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
		vector_clear(vector);
		return NULL;
	}

	/* Move elements if necessary */
	size_t mov = toEnd - diff;
	if(mov) memmove(start, PTR_ADD_BYTES(start, diff), mov);

	/* Reallocate if necessary */
	if(newSize < (vector->capacity >> 1))
	{
		if(!_vector_realloc(vector, newSize, _vector_get_max_capacity(newSize))) return NULL;
		start = PTR_ADD_BYTES(vector->begin, oldSize - toEnd);
	}

	/* Set new end */
	else vector->end = PTR_ADD_BYTES(vector->begin, newSize);

	return start;
}

/******************************************************/
VectorIterator vector_erase_range_at(Vector* vector, size_t num, size_t index)
{
	return vector_erase_range(vector, num, vector_at(vector, index));
}

/******************************************************/
VectorIterator vector_find(Vector* vector, const void* equal)
{
	VectorIterator it;
	for(it = vector->begin; it != vector->end; it = vector_next(vector, it))
		if(!memcmp(it, equal, vector->elementSize)) break;

	return it;
}
