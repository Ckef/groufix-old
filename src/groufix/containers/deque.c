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

#include "groufix/containers/deque.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define GFX_DEQUE_PADDING  1
#define GFX_DEQUE_MSB (~(SIZE_MAX >> 1))

/******************************************************/
static inline size_t _gfx_deque_get_max_capacity(

		size_t size)
{
	if(size > GFX_DEQUE_MSB) return size;

	size_t cap = 1;
	while(cap < size) cap <<= 1;

	return cap;
}

/******************************************************/
static int _gfx_deque_realloc(

		GFXDeque*  deque,
		size_t     capacity)
{
	size_t begin = GFX_PTR_DIFF(deque->data, deque->begin);
	size_t end = GFX_PTR_DIFF(deque->data, deque->end);
	long int diff = capacity - deque->capacity;

	if(diff < 0)
	{
		/* Move end closer to begin */
		if(begin > end)
		{
			memmove(
				GFX_PTR_ADD_BYTES(deque->begin, diff),
				deque->begin,
				deque->capacity - begin
			);
			begin += diff;
		}

		/* Move everything to begin */
		else if(end > capacity)
		{
			end -= begin;
			begin = 0;

			deque->begin = memmove(deque->data, deque->begin, end);
			deque->end = GFX_PTR_ADD_BYTES(deque->data, end);
		}
	}

	/* Make sure to check if it worked */
	void* new = realloc(deque->data, capacity);
	if(!new)
	{
		/* Move memory back >.> */
		if(diff < 0 && begin > end) memmove(
			deque->begin,
			GFX_PTR_ADD_BYTES(deque->begin, diff),
			capacity - begin
		);
		return 0;
	}

	/* Set new properties */
	deque->data = new;
	deque->begin = GFX_PTR_ADD_BYTES(deque->data, begin);
	deque->end = GFX_PTR_ADD_BYTES(deque->data, end);

	/* Move end farther away from begin */
	if(diff > 0 && begin > end) deque->begin = memmove(
		GFX_PTR_ADD_BYTES(deque->begin, diff),
		deque->begin,
		deque->capacity - begin
	);

	deque->capacity = capacity;

	return 1;
}

/******************************************************/
GFXDeque* gfx_deque_create(

		size_t elementSize)
{
	/* Create a new deque */
	GFXDeque* deque = calloc(1, sizeof(GFXDeque));
	if(deque) deque->elementSize = elementSize;

	return deque;
}

/******************************************************/
GFXDeque* gfx_deque_create_from_buffer(

		size_t       elementSize,
		size_t       numElements,
		const void*  buff)
{
	/* Create a new deque */
	GFXDeque* deque = gfx_deque_create(elementSize);
	if(!deque) return NULL;

	/* Allocate and copy */
	size_t size = elementSize * numElements;
	if(!_gfx_deque_realloc(deque, _gfx_deque_get_max_capacity(size)))
	{
		gfx_deque_free(deque);
		return NULL;
	}
	if(buff) memcpy(deque->data, buff, size);

	/* Set end */
	deque->end = GFX_PTR_ADD_BYTES(deque->data, size);

	return deque;
}

/******************************************************/
GFXDeque* gfx_deque_create_copy(

		GFXDeque* src)
{
	/* Create a new deque */
	GFXDeque* deque = gfx_deque_create(src->elementSize);
	if(!deque) return NULL;

	/* Allocate and copy */
	if(!_gfx_deque_realloc(deque, src->capacity))
	{
		gfx_deque_free(deque);
		return NULL;
	}
	memcpy(deque->data, src->data, src->capacity);

	/* Set begin and end */
	deque->begin = GFX_PTR_ADD_BYTES(deque->data, GFX_PTR_DIFF(src->data, src->begin));
	deque->end = GFX_PTR_ADD_BYTES(deque->data, GFX_PTR_DIFF(src->data, src->end));

	return deque;
}

/******************************************************/
void gfx_deque_free(

		GFXDeque* deque)
{
	if(deque)
	{
		free(deque->data);
		free(deque);
	}
}

/******************************************************/
void gfx_deque_init(

		GFXDeque*  deque,
		size_t     elementSize)
{
	memset(deque, 0, sizeof(GFXDeque));
	deque->elementSize = elementSize;
}

/******************************************************/
void gfx_deque_init_from_buffer(

		GFXDeque*    deque,
		size_t       elementSize,
		size_t       numElements,
		const void*  buff)
{
	/* Init the deque */
	gfx_deque_init(deque, elementSize);

	/* Allocate and copy */
	size_t size = elementSize * numElements;
	if(_gfx_deque_realloc(deque, _gfx_deque_get_max_capacity(size)))
	{
		if(buff) memcpy(deque->data, buff, size);
		deque->end = GFX_PTR_ADD_BYTES(deque->data, size);
	}
	else gfx_deque_clear(deque);
}

/******************************************************/
void gfx_deque_init_copy(

		GFXDeque*  deque,
		GFXDeque*  src)
{
	/* Init the deque */
	gfx_deque_init(deque, src->elementSize);

	/* Allocate and copy */
	if(_gfx_deque_realloc(deque, src->capacity))
	{
		memcpy(deque->data, src->data, src->capacity);

		/* Set begin and end */
		deque->begin = GFX_PTR_ADD_BYTES(deque->data, GFX_PTR_DIFF(src->data, src->begin));
		deque->end = GFX_PTR_ADD_BYTES(deque->data, GFX_PTR_DIFF(src->data, src->end));
	}
	else gfx_deque_clear(deque);
}

/******************************************************/
void gfx_deque_clear(

		GFXDeque* deque)
{
	free(deque->data);
	deque->data = NULL;
	deque->begin = NULL;
	deque->end = NULL;

	deque->capacity = 0;
}

/******************************************************/
int gfx_deque_reserve(

		GFXDeque*  deque,
		size_t     numElements)
{
	size_t newSize = deque->elementSize * numElements;
	if(GFX_PTR_DIFF(deque->begin, deque->end) < 0) newSize += GFX_DEQUE_PADDING;

	if(newSize > deque->capacity)
		return _gfx_deque_realloc(deque, _gfx_deque_get_max_capacity(newSize));

	return 1;
}

/******************************************************/
GFXDequeIterator gfx_deque_push_front(

		GFXDeque*    deque,
		const void*  element)
{
	/* Reallocate if necessary */
	size_t oldSize = gfx_deque_get_byte_size(deque);
	size_t newSize = oldSize + deque->elementSize;
	if(GFX_PTR_DIFF(deque->begin, deque->end) < 0 || deque->begin == deque->data)
		newSize += GFX_DEQUE_PADDING;

	if(newSize > deque->capacity)
	{
		/* Get new capacity if empty */
		size_t cap = deque->capacity << 1;
		if(!cap) cap = _gfx_deque_get_max_capacity(newSize);

		if(!_gfx_deque_realloc(deque, cap)) return deque->end;
	}

	/* Get new iterators */
	if(oldSize)
	{
		if(deque->begin != deque->data)
		{
			GFXDequeIterator beg = GFX_PTR_SUB_BYTES(deque->begin, deque->elementSize);
			long int diff = GFX_PTR_DIFF(deque->data, beg);

			if(diff < 0)
			{
				/* Move everything to the beginning */
				memmove(GFX_PTR_SUB_BYTES(deque->begin, diff), deque->begin, oldSize);

				deque->begin = deque->data;
				deque->end = GFX_PTR_SUB_BYTES(deque->end, diff);
			}
			else deque->begin = beg;
		}

		/* Put new element at the end */
		else deque->begin = GFX_PTR_ADD_BYTES(deque->data, deque->capacity - deque->elementSize);
	}

	/* Just put the first element at the beginning */
	else deque->end = GFX_PTR_ADD_BYTES(deque->begin, deque->elementSize);

	/* Insert element */
	if(element) memcpy(deque->begin, element, deque->elementSize);

	return deque->begin;
}

/******************************************************/
GFXDequeIterator gfx_deque_push_back(

		GFXDeque*    deque,
		const void*  element)
{
	/* Reallocate if necessary */
	size_t oldSize = gfx_deque_get_byte_size(deque);
	size_t newSize = oldSize + deque->elementSize;
	if(GFX_PTR_DIFF(deque->begin, deque->end) < 0) newSize += GFX_DEQUE_PADDING;

	if(newSize > deque->capacity)
	{
		/* Get new capacity if empty */
		size_t cap = deque->capacity << 1;
		if(!cap) cap = _gfx_deque_get_max_capacity(newSize);

		if(!_gfx_deque_realloc(deque, cap)) return deque->end;
	}

	/* Get iterator to insert at */
	GFXDequeIterator insert = deque->end;
	GFXDequeIterator end = GFX_PTR_ADD_BYTES(insert, deque->elementSize);
	long int diff = GFX_PTR_DIFF(deque->data, end) - deque->capacity;

	if(diff > 0)
	{
		/* Move everything to the beginning */
		deque->begin = memmove(deque->data, deque->begin, oldSize);

		insert = GFX_PTR_ADD_BYTES(deque->data, oldSize);
		deque->end = GFX_PTR_ADD_BYTES(insert, deque->elementSize);
	}

	/* Just append new element */
	else deque->end = end;

	/* Insert element */
	if(element) memcpy(insert, element, deque->elementSize);

	return insert;
}

/******************************************************/
GFXDequeIterator gfx_deque_pop_front(

		GFXDeque* deque)
{
	/* Nothing to pop */
	if(deque->begin != deque->end)
	{
		/* Just set a new begin iterator */
		deque->begin = gfx_deque_advance(deque, deque->begin, 1);

		/* Reallocate if necessary */
		/* Use upperbound/4 instead to avoid constant realloc */
		if(deque->begin != deque->end)
		{
			size_t size = gfx_deque_get_byte_size(deque);

			if(GFX_PTR_DIFF(deque->begin, deque->end) < 0)
				size += GFX_DEQUE_PADDING;

			if(size < (deque->capacity >> 2))
				_gfx_deque_realloc(deque, _gfx_deque_get_max_capacity(size));
		}
		else gfx_deque_clear(deque);
	}

	return deque->begin;
}

/******************************************************/
GFXDequeIterator gfx_deque_pop_back(

		GFXDeque* deque)
{
	/* Nothing to pop */
	if(deque->begin != deque->end)
	{
		/* Just set a new end iterator */
		deque->end = gfx_deque_advance(deque, deque->end, -1);

		if(deque->begin != deque->end)
		{
			if(deque->end == deque->data)
				deque->end = GFX_PTR_ADD_BYTES(deque->data, deque->capacity);

			/* Reallocate if necessary */
			/* Use upperbound/4 instead to avoid constant realloc */
			size_t size = gfx_deque_get_byte_size(deque);

			if(GFX_PTR_DIFF(deque->begin, deque->end) < 0)
				size += GFX_DEQUE_PADDING;

			if(size < (deque->capacity >> 2))
				_gfx_deque_realloc(deque, _gfx_deque_get_max_capacity(size));

			/* Return correct replacement */
			return GFX_PTR_SUB_BYTES(deque->end, deque->elementSize);
		}
		else gfx_deque_clear(deque);
	}

	return deque->end;
}

/******************************************************/
size_t gfx_deque_get_byte_size(

		GFXDeque* deque)
{
	/* Get pointer difference */
	long int begToEnd = GFX_PTR_DIFF(deque->begin, deque->end);

	/* Get actual storage used */
	if(begToEnd < 0) return deque->capacity + begToEnd;
	return begToEnd;
}

/******************************************************/
size_t gfx_deque_get_index(

		GFXDeque*         deque,
		GFXDequeIterator  it)
{
	/* Get pointer difference */
	long int diff = GFX_PTR_DIFF(deque->begin, it);

	/* Get actual index */
	if(diff < 0) return (deque->capacity + diff) / deque->elementSize;
	else return diff / deque->elementSize;
}

/******************************************************/
GFXDequeIterator gfx_deque_advance(

		GFXDeque*         deque,
		GFXDequeIterator  it,
		int               num)
{
	/* Get sequential iterator */
	GFXDequeIterator new = GFX_PTR_ADD_BYTES(it, deque->elementSize * num);
	long int diff = GFX_PTR_DIFF(deque->data, new);

	/* Get actual pointer */
	if(diff < 0)
		return GFX_PTR_ADD_BYTES(deque->data, deque->capacity + diff);

	if(diff >= deque->capacity && new != deque->end)
		return GFX_PTR_ADD_BYTES(deque->data, diff - deque->capacity);

	return new;
}
