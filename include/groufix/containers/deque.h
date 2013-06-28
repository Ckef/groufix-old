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

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * \brief Double ended queue container
 *******************************************************/

/** \brief Deque iterator */
typedef void* DequeIterator;


/** \brief Deque */
typedef struct Deque
{
	size_t elementSize;
	size_t capacity; /* in bytes */

	void* data;

	DequeIterator begin;
	DequeIterator end;

} Deque;


/**
 * \brief Creates a new deque.
 *
 */
Deque* deque_create(size_t elementSize);

/**
 * \brief Creates a new deque with a preset content.
 *
 * \param numElements The number of elements stored in the buffer (not byte size!).
 * \param buff        Memory to copy content from.
 *
 */
Deque* deque_create_from_buffer(size_t elementSize, size_t numElements, const void* buff);

/**
 * \brief Creates a copy of a deque.
 *
 */
Deque* deque_create_copy(Deque* src);

/**
 * \brief Makes sure the deque is freed properly.
 *
 */
void deque_free(Deque* deque);

/**
 * \brief Clears the content of a vector.
 *
 */
void deque_clear(Deque* deque);

/**
 * \brief Returns the size of the deque in bytes.
 *
 */
size_t deque_get_byte_size(Deque* deque);

/**
 * \brief Returns the size of the deque in elements.
 *
 */
size_t deque_get_size(Deque* deque);

/**
 * \brief Requests a minimum capacity.
 *
 * \return If zero, out of memory.
 *
 */
int deque_reserve(Deque* deque, size_t numElements);

/**
 * \brief Shrinks the deque to fit the elements it holds.
 *
 * \return If zero, out of memory.
 *
 */
int deque_shrink(Deque* deque);

/**
 * \brief Returns an iterator of the element at a given index.
 *
 * This method does not check the bounds!
 *
 */ 
DequeIterator deque_at(Deque* deque, size_t index);

/**
 * \brief Increments an iterator to the next element.
 *
 */
DequeIterator deque_next(Deque* deque, DequeIterator it);

/**
 * \brief Decrements an iterator to the previous element.
 *
 */
DequeIterator deque_previous(Deque* deque, DequeIterator it);

/**
 * \brief Advances an iterator an arbitrary amount of elements (can be negative).
 *
 */
DequeIterator deque_advance(Deque* deque, DequeIterator it, int num);

/**
 * \brief Adds an element to the front of the deque.
 *
 * \return An iterator to the new element (NULL on failure).
 *
 */
DequeIterator deque_push_front(Deque* deque, const void* element);

/**
 * \brief Adds an element to the back of the deque.
 *
 * \return An iterator to the new element (NULL on failure).
 *
 */
DequeIterator deque_push_back(Deque* deque, const void* element);

/**
 * \brief Removes an element from the front of the deque.
 *
 * \return An iterator to the element taking its place.
 *
 */
DequeIterator deque_pop_front(Deque* deque);

/**
 * \brief Removes an element from the back of the deque.
 *
 * \return An iterator to the element taking its place.
 *
 */
DequeIterator deque_pop_back(Deque* deque);


#ifdef __cplusplus
}
#endif

#endif // GFX_CONTAINERS_DEQUE_H
