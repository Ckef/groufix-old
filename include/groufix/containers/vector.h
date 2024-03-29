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

#ifndef GFX_CONTAINERS_VECTOR_H
#define GFX_CONTAINERS_VECTOR_H

#include "groufix/utils.h"

#include <stddef.h>


/********************************************************
 * Vector container
 *******************************************************/

/** Vector iterator */
typedef void* GFXVectorIterator;


/** Vector */
typedef struct GFXVector
{
	size_t             elementSize;
	size_t             capacity; /* in bytes */

	GFXVectorIterator  begin;
	GFXVectorIterator  end;

} GFXVector;


/**
 * Creates a new vector.
 *
 * @return NULL on failure.
 *
 */
GFX_API GFXVector* gfx_vector_create(

		size_t elementSize);

/**
 * Creates a new vector with a preset content.
 *
 * @param numElements The number of elements stored in the buffer (not byte size!).
 * @param buff        Memory to copy content from, if NULL, nothing will be copied.
 * @return NULL on failure.
 *
 */
GFX_API GFXVector* gfx_vector_create_from_buffer(

		size_t       elementSize,
		size_t       numElements,
		const void*  buff);

/**
 * Creates a copy of a vector.
 *
 * @return NULL on failure.
 *
 */
GFX_API GFXVector* gfx_vector_create_copy(

		const GFXVector* src);

/**
 * Makes sure the vector is freed properly.
 *
 */
GFX_API void gfx_vector_free(

		GFXVector* vector);

/**
 * Initializes a vector.
 *
 */
GFX_API void gfx_vector_init(

		GFXVector*  vector,
		size_t      elementSize);

/**
 * Initializes a vector with a preset content.
 *
 * @param numElements The number of elements stored in the buffer (not byte size!).
 * @param buff        Memory to copy content from, if NULL, nothing will be copied.
 *
 */
GFX_API void gfx_vector_init_from_buffer(

		GFXVector*   vector,
		size_t       elementSize,
		size_t       numElements,
		const void*  buff);

/**
 * Initializes a copy of a vector.
 *
 */
GFX_API void gfx_vector_init_copy(

		GFXVector*        vector,
		const GFXVector*  src);

/**
 * Clears the content of a vector.
 *
 */
GFX_API void gfx_vector_clear(

		GFXVector* vector);

/**
 * Requests a minimum capacity, which will hold as long as nothing is erased.
 *
 * @return If zero, out of memory.
 *
 */
GFX_API int gfx_vector_reserve(

		GFXVector*  vector,
		size_t      numElements);

/**
 * Inserts an element at a given iterator.
 *
 * @param element Data to copy into the new element, can be NULL to copy nothing.
 * @return The iterator to the new element (vector->end on failure).
 *
 */
GFX_API GFXVectorIterator gfx_vector_insert(

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
GFX_API GFXVectorIterator gfx_vector_insert_range(

		GFXVector*               vector,
		size_t                   num,
		const GFXVectorIterator  start,
		GFXVectorIterator        pos);

/**
 * Erases a range of elements at a given iterator.
 *
 * @param num   Number of elements to erase.
 * @param start The start of the range.
 * @return The iterator to the element taking its place.
 *
 */
GFX_API GFXVectorIterator gfx_vector_erase_range(

		GFXVector*         vector,
		size_t             num,
		GFXVectorIterator  start);

/**
 * Returns the size of the vector in bytes.
 *
 */
static GFX_ALWAYS_INLINE size_t gfx_vector_get_byte_size(

		const GFXVector* vector)
{
	return GFX_PTR_DIFF(vector->begin, vector->end);
}

/**
 * Returns the size of the vector in elements.
 *
 */
static GFX_ALWAYS_INLINE size_t gfx_vector_get_size(

		const GFXVector* vector)
{
	return GFX_PTR_DIFF(vector->begin, vector->end) / vector->elementSize;
}

/**
 * Returns the index of an iterator.
 *
 */
static GFX_ALWAYS_INLINE size_t gfx_vector_get_index(

		const GFXVector*         vector,
		const GFXVectorIterator  it)
{
	return GFX_PTR_DIFF(vector->begin, it) / vector->elementSize;
}

/**
 * Returns an iterator of the element at a given index.
 *
 * This method does not check the bounds!
 *
 */
static GFX_ALWAYS_INLINE GFXVectorIterator gfx_vector_at(

		const GFXVector*  vector,
		size_t            index)
{
	return GFX_PTR_ADD_BYTES(vector->begin, index * vector->elementSize);
}

/**
 * Increments an iterator to the next element.
 *
 */
static GFX_ALWAYS_INLINE GFXVectorIterator gfx_vector_next(

		const GFXVector*         vector,
		const GFXVectorIterator  it)
{
	return GFX_PTR_ADD_BYTES(it, vector->elementSize);
}

/**
 * Decrements an iterator to the previous element.
 *
 */
static GFX_ALWAYS_INLINE GFXVectorIterator gfx_vector_previous(

		const GFXVector*         vector,
		const GFXVectorIterator  it)
{
	return GFX_PTR_SUB_BYTES(it, vector->elementSize);
}

/**
 * Advances an iterator an arbitrary amount of elements (can be negative).
 *
 */
static GFX_ALWAYS_INLINE GFXVectorIterator gfx_vector_advance(

		const GFXVector*         vector,
		const GFXVectorIterator  it,
		int                      num)
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
static GFX_ALWAYS_INLINE GFXVectorIterator gfx_vector_insert_at(

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
static GFX_ALWAYS_INLINE GFXVectorIterator gfx_vector_insert_range_at(

		GFXVector*               vector,
		size_t                   num,
		const GFXVectorIterator  start,
		size_t                   index)
{
	return gfx_vector_insert_range(vector, num, start, gfx_vector_at(vector, index));
}

/**
 * Erases an element at a given iterator.
 *
 * @return The iterator to the element taking its place.
 *
 */
static GFX_ALWAYS_INLINE GFXVectorIterator gfx_vector_erase(

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
static GFX_ALWAYS_INLINE GFXVectorIterator gfx_vector_erase_at(

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
static GFX_ALWAYS_INLINE GFXVectorIterator gfx_vector_erase_range_at(

		GFXVector*  vector,
		size_t      num,
		size_t      index)
{
	return gfx_vector_erase_range(vector, num, gfx_vector_at(vector, index));
}


#endif // GFX_CONTAINERS_VECTOR_H
