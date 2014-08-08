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

#ifndef GFX_CONTAINERS_VECTOR_H
#define GFX_CONTAINERS_VECTOR_H

#include "groufix/utils.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Vector container
 *******************************************************/

/** Vector iterator */
typedef void* GFXVectorIterator;


/** Vector */
typedef struct GFXVector
{
	size_t elementSize;
	size_t capacity; /* in bytes */

	GFXVectorIterator begin;
	GFXVectorIterator end;

} GFXVector;


/**
 * Creates a new vector.
 *
 * @return NULL on failure.
 *
 */
GFXVector* gfx_vector_create(

		size_t elementSize);

/**
 * Creates a new vector with a preset content.
 *
 * @param numElements The number of elements stored in the buffer (not byte size!).
 * @param buff        Memory to copy content from, if NULL, nothing will be copied.
 * @return NULL on failure.
 *
 */
GFXVector* gfx_vector_create_from_buffer(

		size_t       elementSize,
		size_t       numElements,
		const void*  buff);

/**
 * Creates a copy of a vector.
 *
 * @return NULL on failure.
 *
 */
GFXVector* gfx_vector_create_copy(

		GFXVector* src);

/**
 * Makes sure the vector is freed properly.
 *
 */
void gfx_vector_free(

		GFXVector* vector);

/**
 * Initializes a vector.
 *
 */
void gfx_vector_init(

		GFXVector*  vector,
		size_t      elementSize);

/**
 * Initializes a vector with a preset content.
 *
 * @param numElements The number of elements stored in the buffer (not byte size!).
 * @param buff        Memory to copy content from, if NULL, nothing will be copied.
 *
 */
void gfx_vector_init_from_buffer(

		GFXVector*   vector,
		size_t       elementSize,
		size_t       numElements,
		const void*  buff);

/**
 * Initializes a copy of a vector.
 *
 */
void gfx_vector_init_copy(

		GFXVector*  vector,
		GFXVector*  src);

/**
 * Clears the content of a vector.
 *
 */
void gfx_vector_clear(

		GFXVector* vector);

/**
 * Requests a minimum capacity, which will hold as long as nothing is erased.
 *
 * @return If zero, out of memory.
 *
 */
int gfx_vector_reserve(

		GFXVector*  vector,
		size_t      numElements);

/**
 * Inserts an element at a given iterator.
 *
 * @param element Data to copy into the new element, can be NULL to copy nothing.
 * @return The iterator to the new element (vector->end on failure).
 *
 */
GFXVectorIterator gfx_vector_insert(

		GFXVector*         vector,
		const void*        element,
		GFXVectorIterator  pos);

/**
 * Inserts a range of elements at a given iterator (can be this vector).
 *
 * @param num   Number of elements to insert.
 * @param start The start of the range, can also be an arbitrary pointer, or NULL to copy nothing.
 * @return The iterator to the beginning of the inserted range (vector->end on failure).
 *
 */
GFXVectorIterator gfx_vector_insert_range(

		GFXVector*         vector,
		size_t             num,
		GFXVectorIterator  start,
		GFXVectorIterator  pos);

/**
 * Erases a range of elements at a given iterator.
 *
 * @param num   Number of elements to erase.
 * @param start The start of the range.
 * @return The iterator to the element taking its place.
 *
 */
GFXVectorIterator gfx_vector_erase_range(

		GFXVector*         vector,
		size_t             num,
		GFXVectorIterator  start);

/**
 * Returns the size of the vector in bytes.
 *
 */
inline size_t gfx_vector_get_byte_size(

		GFXVector* vector)
{
	return GFX_PTR_DIFF(vector->begin, vector->end);
}

/**
 * Returns the size of the vector in elements.
 *
 */
inline size_t gfx_vector_get_size(

		GFXVector* vector)
{
	return GFX_PTR_DIFF(vector->begin, vector->end) / vector->elementSize;
}

/**
 * Returns the index of an iterator.
 *
 */
inline size_t gfx_vector_get_index(

		GFXVector*         vector,
		GFXVectorIterator  it)
{
	return GFX_PTR_DIFF(vector->begin, it) / vector->elementSize;
}

/**
 * Returns an iterator of the element at a given index.
 *
 * This method does not check the bounds!
 *
 */
inline GFXVectorIterator gfx_vector_at(

		GFXVector*  vector,
		size_t      index)
{
	return GFX_PTR_ADD_BYTES(vector->begin, index * vector->elementSize);
}

/**
 * Increments an iterator to the next element.
 *
 */
inline GFXVectorIterator gfx_vector_next(

		GFXVector*         vector,
		GFXVectorIterator  it)
{
	return GFX_PTR_ADD_BYTES(it, vector->elementSize);
}

/**
 * Decrements an iterator to the previous element.
 *
 */
inline GFXVectorIterator gfx_vector_previous(

		GFXVector*         vector,
		GFXVectorIterator  it)
{
	return GFX_PTR_SUB_BYTES(it, vector->elementSize);
}

/**
 * Advances an iterator an arbitrary amount of elements (can be negative).
 *
 */
inline GFXVectorIterator gfx_vector_advance(

		GFXVector*         vector,
		GFXVectorIterator  it,
		int                num)
{
	return GFX_PTR_ADD_BYTES(it, vector->elementSize * num);
}

/**
 * Inserts an element after a given index.
 *
 * @param element Data to copy into the new element, can be NULL to copy nothing.
 * @return The iterator to the new element (vector->end on failure).
 *
 */
inline GFXVectorIterator gfx_vector_insert_at(

		GFXVector*   vector,
		const void*  element,
		size_t       index)
{
	return gfx_vector_insert(vector, element, gfx_vector_at(vector, index));
}

/**
 * Inserts a range of elements at a given index (can be this vector).
 *
 * @param num   Number of elements to insert.
 * @param start The start of the range, can also be an arbitrary pointer, or NULL to copy nothing.
 * @return The iterator to the beginning of the inserted range (vector->end on failure).
 *
 */
inline GFXVectorIterator gfx_vector_insert_range_at(

		GFXVector*         vector,
		size_t             num,
		GFXVectorIterator  start,
		size_t             index)
{
	return gfx_vector_insert_range(vector, num, start, gfx_vector_at(vector, index));
}

/**
 * Erases an element at a given iterator.
 *
 * @return The iterator to the element taking its place.
 *
 */
inline GFXVectorIterator gfx_vector_erase(

		GFXVector*         vector,
		GFXVectorIterator  pos)
{
	return gfx_vector_erase_range(vector, 1, pos);
}

/**
 * Erases an element at a given index.
 *
 * @return The iterator to the element taking its place.
 *
 */
inline GFXVectorIterator gfx_vector_erase_at(

		GFXVector*  vector,
		size_t      index)
{
	return gfx_vector_erase_range(vector, 1, gfx_vector_at(vector, index));
}

/**
 * Erases a range of elements at a given index.
 *
 * @param num Number of elements to erase.
 * @return The iterator to the element taking its place.
 *
 */
inline GFXVectorIterator gfx_vector_erase_range_at(

		GFXVector*  vector,
		size_t      num,
		size_t      index)
{
	return gfx_vector_erase_range(vector, num, gfx_vector_at(vector, index));
}


#ifdef __cplusplus
}
#endif

#endif // GFX_CONTAINERS_VECTOR_H
