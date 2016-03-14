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

#ifndef GFX_CONTAINERS_DEQUE_H
#define GFX_CONTAINERS_DEQUE_H

#include "groufix/utils.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Double ended queue container
 *******************************************************/

/** Deque iterator */
typedef void *GFXDequeIterator;


/** Deque */
typedef struct GFXDeque
{
	size_t            elementSize;
	size_t            capacity; /* in bytes */

	void             *data;
	GFXDequeIterator  begin;
	GFXDequeIterator  end;

} GFXDeque;


/**
 * Creates a new deque.
 *
 * @return NULL on failure.
 *
 */
GFX_API GFXDeque *gfx_deque_create(

		size_t elementSize);

/**
 * Creates a new deque with a preset content.
 *
 * @param numElements The number of elements stored in the buffer (not byte size!).
 * @param buff        Memory to copy content from, if NULL, nothing will be copied.
 * @return NULL on failure.
 *
 */
GFX_API GFXDeque *gfx_deque_create_from_buffer(

		size_t       elementSize,
		size_t       numElements,
		const void  *buff);

/**
 * Creates a copy of a deque.
 *
 * @return NULL on failure.
 *
 */
GFX_API GFXDeque *gfx_deque_create_copy(

		const GFXDeque *src);

/**
 * Makes sure the deque is freed properly.
 *
 */
GFX_API void gfx_deque_free(

		GFXDeque *deque);

/**
 * Initializes a deque.
 *
 */
GFX_API void gfx_deque_init(

		GFXDeque  *deque,
		size_t     elementSize);

/**
 * Initializes a deque with a preset content.
 *
 * @param numElements The number of elements stored in the buffer (not byte size!).
 * @param buff        Memory to copy content from, if NULL, nothing will be copied.
 *
 * Note: size will be 0 if the data could not be allocated.
 *
 */
GFX_API void gfx_deque_init_from_buffer(

		GFXDeque    *deque,
		size_t       elementSize,
		size_t       numElements,
		const void  *buff);

/**
 * Initializes a copy of a deque.
 *
 * Note: size will be 0 if the data could not be allocated.
 *
 */
GFX_API void gfx_deque_init_copy(

		GFXDeque        *deque,
		const GFXDeque  *src);

/**
 * Clears the content of a deque.
 *
 */
GFX_API void gfx_deque_clear(

		GFXDeque *deque);

/**
 * Requests a minimum capacity, which will hold as long as nothing is erased.
 *
 * @return If zero, out of memory.
 *
 */
GFX_API int gfx_deque_reserve(

		GFXDeque  *deque,
		size_t     numElements);

/**
 * Adds an element to the begin of the deque.
 *
 * @param element Data to copy into the new element, can be NULL to copy nothing.
 * @return An iterator to the new element (deque->end on failure).
 *
 */
GFX_API GFXDequeIterator gfx_deque_push_begin(

		GFXDeque    *deque,
		const void  *element);

/**
 * Adds an element to the end of the deque.
 *
 * @param element Data to copy into the new element, can be NULL to copy nothing.
 * @return An iterator to the new element (deque->end on failure).
 *
 */
GFX_API GFXDequeIterator gfx_deque_push_end(

		GFXDeque    *deque,
		const void  *element);

/**
 * Removes an element from the begin of the deque.
 *
 * @return An iterator to the element taking its place.
 *
 */
GFX_API GFXDequeIterator gfx_deque_pop_begin(

		GFXDeque *deque);

/**
 * Removes an element from the end of the deque.
 *
 * @return An iterator to the element taking its place.
 *
 */
GFX_API GFXDequeIterator gfx_deque_pop_end(

		GFXDeque *deque);

/**
 * Returns the size of the deque in bytes.
 *
 */
GFX_API size_t gfx_deque_get_byte_size(

		const GFXDeque *deque);

/**
 * Returns the index of an iterator.
 *
 */
GFX_API size_t gfx_deque_get_index(

		const GFXDeque         *deque,
		const GFXDequeIterator  it);

/**
 * Advances an iterator an arbitrary amount of elements (can be negative).
 *
 */
GFX_API GFXDequeIterator gfx_deque_advance(

		const GFXDeque         *deque,
		const GFXDequeIterator  it,
		int                     num);

/**
 * Returns the size of the deque in elements.
 *
 */
static GFX_ALWAYS_INLINE size_t gfx_deque_get_size(

		const GFXDeque *deque)
{
	return gfx_deque_get_byte_size(deque) / deque->elementSize;
}

/**
 * Returns an iterator of the element at a given index.
 *
 * This method does not check the bounds!
 *
 */
static GFX_ALWAYS_INLINE GFXDequeIterator gfx_deque_at(

		const GFXDeque  *deque,
		size_t           index)
{
	return gfx_deque_advance(deque, deque->begin, index);
}

/**
 * Increments an iterator to the next element.
 *
 */
static GFX_ALWAYS_INLINE GFXDequeIterator gfx_deque_next(

		const GFXDeque         *deque,
		const GFXDequeIterator  it)
{
	return gfx_deque_advance(deque, it, 1);
}

/**
 * Decrements an iterator to the previous element.
 *
 */
static GFX_ALWAYS_INLINE GFXDequeIterator gfx_deque_previous(

		const GFXDeque         *deque,
		const GFXDequeIterator  it)
{
	return gfx_deque_advance(deque, it, -1);
}


#ifdef __cplusplus
}
#endif

#endif // GFX_CONTAINERS_DEQUE_H
