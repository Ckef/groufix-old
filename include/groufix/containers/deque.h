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

#ifndef GFX_CONTAINERS_DEQUE_H
#define GFX_CONTAINERS_DEQUE_H

#include <stddef.h>

/* Padding between begin and end */
#define GFX_DEQUE_PADDING  1

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Double ended queue container
 *******************************************************/

/** \brief Deque iterator */
typedef void* GFXDequeIterator;


/** \brief Deque */
typedef struct GFXDeque
{
	size_t elementSize;
	size_t capacity; /* in bytes */

	void* data;

	GFXDequeIterator begin;
	GFXDequeIterator end;

} GFXDeque;


/**
 * \brief Creates a new deque.
 *
 * \return NULL on failure.
 *
 */
GFXDeque* gfx_deque_create(size_t elementSize);

/**
 * \brief Creates a new deque with a preset content.
 *
 * \param numElements The number of elements stored in the buffer (not byte size!).
 * \param buff        Memory to copy content from.
 * \return NULL on failure.
 *
 */
GFXDeque* gfx_deque_create_from_buffer(size_t elementSize, size_t numElements, const void* buff);

/**
 * \brief Creates a copy of a deque.
 *
 * \return NULL on failure.
 *
 */
GFXDeque* gfx_deque_create_copy(GFXDeque* src);

/**
 * \brief Makes sure the deque is freed properly.
 *
 */
void gfx_deque_free(GFXDeque* deque);

/**
 * \brief Initializes a deque.
 *
 */
void gfx_deque_init(GFXDeque* deque, size_t elementSize);

/**
 * \brief Initializes a deque with a preset content.
 *
 * \param numElements The number of elements stored in the buffer (not byte size!).
 * \param buff        Memory to copy content from.
 *
 */
void gfx_deque_init_from_buffer(GFXDeque* deque, size_t elementSize, size_t numElements, const void* buff);

/**
 * \brief Initializes a copy of a deque.
 *
 */
void gfx_deque_init_copy(GFXDeque* deque, GFXDeque* src);

/**
 * \brief Clears the content of a deque.
 *
 */
void gfx_deque_clear(GFXDeque* deque);

/**
 * \brief Returns the size of the deque in bytes.
 *
 */
size_t gfx_deque_get_byte_size(GFXDeque* deque);

/**
 * \brief Returns the size of the deque in elements.
 *
 */
size_t gfx_deque_get_size(GFXDeque* deque);

/**
 * \brief Requests a minimum capacity.
 *
 * \return If zero, out of memory.
 *
 */
int gfx_deque_reserve(GFXDeque* deque, size_t numElements);

/**
 * \brief Shrinks the deque to fit the elements it holds.
 *
 * \return If zero, out of memory.
 *
 */
int gfx_deque_shrink(GFXDeque* deque);

/**
 * \brief Returns an iterator of the element at a given index.
 *
 * This method does not check the bounds!
 *
 */ 
GFXDequeIterator gfx_deque_at(GFXDeque* deque, size_t index);

/**
 * \brief Increments an iterator to the next element.
 *
 */
GFXDequeIterator gfx_deque_next(GFXDeque* deque, GFXDequeIterator it);

/**
 * \brief Decrements an iterator to the previous element.
 *
 */
GFXDequeIterator gfx_deque_previous(GFXDeque* deque, GFXDequeIterator it);

/**
 * \brief Advances an iterator an arbitrary amount of elements (can be negative).
 *
 */
GFXDequeIterator gfx_deque_advance(GFXDeque* deque, GFXDequeIterator it, int num);

/**
 * \brief Adds an element to the front of the deque.
 *
 * \return An iterator to the new element (NULL or deque->end on failure).
 *
 */
GFXDequeIterator gfx_deque_push_front(GFXDeque* deque, const void* element);

/**
 * \brief Adds an element to the back of the deque.
 *
 * \return An iterator to the new element (NULL or deque->end on failure).
 *
 */
GFXDequeIterator gfx_deque_push_back(GFXDeque* deque, const void* element);

/**
 * \brief Removes an element from the front of the deque.
 *
 * \return An iterator to the element taking its place.
 *
 */
GFXDequeIterator gfx_deque_pop_front(GFXDeque* deque);

/**
 * \brief Removes an element from the back of the deque.
 *
 * \return An iterator to the element taking its place.
 *
 */
GFXDequeIterator gfx_deque_pop_back(GFXDeque* deque);


#ifdef __cplusplus
}
#endif

#endif // GFX_CONTAINERS_DEQUE_H
