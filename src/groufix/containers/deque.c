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

#include "groufix/containers/deque.h"
#include "groufix/utils.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
static size_t _deque_get_max_capacity(size_t size)
{
	size_t cap = 1;
	while(cap < size) cap <<= 1;

	return cap;
}

/******************************************************/
static int _deque_realloc(Deque* deque, size_t capacity)
{
	size_t begin = PTR_DIFF(deque->data, deque->begin);
	size_t end = PTR_DIFF(deque->data, deque->end);
	long int diff = capacity - deque->capacity;

	/* Move if necessary */
	if(diff < 0 && begin > end) deque->begin = memmove(
		PTR_ADD_BYTES(deque->begin, diff),
		deque->begin,
		deque->capacity - begin
	);

	/* Make sure to check if it worked */
	void* new = realloc(deque->data, capacity);
	if(!new)
	{
		deque_clear(deque);
		return 0;
	}

	/* Set new properties */
	deque->data = new;
	deque->begin = PTR_ADD_BYTES(deque->data, begin);
	deque->end = PTR_ADD_BYTES(deque->data, end);

	/* Move if necessary */
	if(diff > 0 && begin > end) deque->begin = memmove(
		PTR_ADD_BYTES(deque->begin, diff),
		deque->begin,
		deque->capacity - begin
	);

	/* New capacity */
	deque->capacity = capacity;

	return 1;
}

/******************************************************/
static DequeIterator _deque_advance(Deque* deque, DequeIterator it, long int bytes)
{
	/* Get sequential iterator */
	DequeIterator new = PTR_ADD_BYTES(it, bytes);
	long int diff = PTR_DIFF(deque->data, new);

	/* Get actual pointer */
	if(diff < 0) return PTR_ADD_BYTES(deque->data, deque->capacity + diff);
	if(diff >= deque->capacity && new != deque->end) return PTR_ADD_BYTES(deque->data, diff - deque->capacity);

	return new;
}

/******************************************************/
Deque* deque_create(size_t elementSize)
{
	/* Create a new deque */
	Deque* deque = (Deque*)calloc(1, sizeof(Deque));
	if(deque) deque->elementSize = elementSize;

	return deque;
}

/******************************************************/
Deque* deque_create_from_buffer(size_t elementSize, size_t numElements, const void* buff)
{
	/* Create a new deque */
	Deque* deque = deque_create(elementSize);
	if(!deque) return NULL;

	/* Allocate and copy */
	size_t size = elementSize * numElements;
	if(!_deque_realloc(deque, _deque_get_max_capacity(size)))
	{
		deque_free(deque);
		return NULL;
	}
	memcpy(deque->data, buff, size);

	/* Set end */
	deque->end = PTR_ADD_BYTES(deque->data, size);

	return deque;
}

/******************************************************/
Deque* deque_create_copy(Deque* src)
{
	/* Create a new deque */
	Deque* deque = deque_create(src->elementSize);
	if(!deque) return NULL;

	/* Allocate and copy */
	if(!_deque_realloc(deque, src->capacity))
	{
		deque_free(deque);
		return NULL;
	}
	memcpy(deque->data, src->data, src->capacity);

	/* Set begin and end */
	deque->begin = PTR_ADD_BYTES(deque->data, PTR_DIFF(src->data, src->begin));
	deque->end = PTR_ADD_BYTES(deque->data, PTR_DIFF(src->data, src->end));

	return deque;
}

/******************************************************/
void deque_free(Deque* deque)
{
	if(deque)
	{
		free(deque->data);
		free(deque);
	}
}

/******************************************************/
void deque_clear(Deque* deque)
{
	free(deque->data);
	deque->data = NULL;
	deque->begin = NULL;
	deque->end = NULL;

	deque->capacity = 0;
}

/******************************************************/
size_t deque_get_byte_size(Deque* deque)
{
	/* Get pointer difference */
	long int begToEnd = PTR_DIFF(deque->begin, deque->end);

	/* Get actual storage used */
	if(begToEnd < 0) return deque->capacity + begToEnd;
	return begToEnd;
}

/******************************************************/
size_t deque_get_size(Deque* deque)
{
	return deque_get_byte_size(deque) / deque->elementSize;
}

/******************************************************/
int deque_reserve(Deque* deque, size_t numElements)
{
	size_t newSize = deque->elementSize * numElements;
	if(newSize > deque->capacity)
		return _deque_realloc(deque, _deque_get_max_capacity(newSize));

	return 1;
}

/******************************************************/
int deque_shrink(Deque* deque)
{
	size_t newSize = deque_get_byte_size(deque);
	if(newSize < (deque->capacity >> 1))
		return _deque_realloc(deque, _deque_get_max_capacity(newSize));

	return 1;
}

/******************************************************/
DequeIterator deque_at(Deque* deque, size_t index)
{
	return _deque_advance(deque, deque->begin, index * deque->elementSize);
}

/******************************************************/
DequeIterator deque_next(Deque* deque, DequeIterator it)
{
	return _deque_advance(deque, it, deque->elementSize);
}

/******************************************************/
DequeIterator deque_previous(Deque* deque, DequeIterator it)
{
	return _deque_advance(deque, it, -deque->elementSize);
}

/******************************************************/
DequeIterator deque_advance(Deque* deque, DequeIterator it, int num)
{
	return _deque_advance(deque, it, deque->elementSize * num);
}

/******************************************************/
DequeIterator deque_push_front(Deque* deque, const void* element)
{
	/* Reallocate if necessary */
	size_t oldSize = deque_get_byte_size(deque);
	size_t newSize = oldSize + deque->elementSize;
	if(newSize > deque->capacity)
	{
		/* Get new capacity if empty */
		size_t cap = deque->capacity << 1;
		if(!cap) cap = _deque_get_max_capacity(newSize);

		if(!_deque_realloc(deque, cap)) return NULL;
	}

	/* Get new iterators */
	if(!oldSize) deque->end = PTR_ADD_BYTES(deque->begin, deque->elementSize);
	else deque->begin = _deque_advance(deque, deque->begin, -deque->elementSize);

	/* Insert element */
	memcpy(deque->begin, element, deque->elementSize);

	return deque->begin;
}

/******************************************************/
DequeIterator deque_push_back(Deque* deque, const void* element)
{
	/* Reallocate if necessary */
	size_t oldSize = deque_get_byte_size(deque);
	size_t newSize = oldSize + deque->elementSize;
	if(newSize > deque->capacity)
	{
		/* Get new capacity if empty */
		size_t cap = deque->capacity << 1;
		if(!cap) cap = _deque_get_max_capacity(newSize);

		if(!_deque_realloc(deque, cap)) return NULL;
	}

	/* Get iterator to insert at */
	DequeIterator insert = deque->end;
	if(PTR_DIFF(deque->data, deque->end) == deque->capacity) insert = deque->data;

	/* Insert element */
	memcpy(insert, element, deque->elementSize);
	deque->end = PTR_ADD_BYTES(insert, deque->elementSize);

	return insert;
}

/******************************************************/
DequeIterator deque_pop_front(Deque* deque)
{
	/* Nothing to pop */
	if(deque->begin != deque->end)
	{
		/* Just set a new begin iterator */
		deque->begin = _deque_advance(deque, deque->begin, deque->elementSize);
		if(deque->begin == deque->end)
		{
			/* Make sure to reset so begin cannot be at the end */
			deque->begin = deque->data;
			deque->end = deque->data;
		}
	}

	return deque->begin;
}

/******************************************************/
DequeIterator deque_pop_back(Deque* deque)
{
	/* Nothing to pop */
	if(deque->begin != deque->end)
	{
		/* Just set a new end iterator */
		deque->end = _deque_advance(deque, deque->end, -deque->elementSize);

		/* Loop end back to the actual end, if not empty */
		if(deque->end == deque->data && deque->begin != deque->end)
			deque->end = PTR_ADD_BYTES(deque->data, deque->capacity);
	}

	return deque->end == deque->begin ? deque->end : PTR_SUB_BYTES(deque->end, deque->elementSize);
}

/******************************************************/
DequeIterator deque_find(Deque* deque, const void* value, DequeComparison func)
{
	DequeIterator it;
	for(it = deque->begin; it != deque->end; it = deque_next(deque, it))
		if(func(it, value)) break;

	return it;
}
