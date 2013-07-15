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

#ifndef GFX_CONTAINERS_VECTOR_H
#define GFX_CONTAINERS_VECTOR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * \brief Vector container
 *******************************************************/

/** \brief Vector iterator */
typedef void* VectorIterator;


/** \brief Comparison function */
typedef int (*VectorComparison)(const VectorIterator, const void* value);


/** \brief Vector */
typedef struct Vector
{
	size_t elementSize;
	size_t capacity; /* in bytes */

	VectorIterator begin;
	VectorIterator end;

} Vector;


/**
 * \brief Creates a new vector.
 *
 * \return NULL on failure.
 *
 */
Vector* vector_create(size_t elementSize);

/**
 * \brief Creates a new vector with a preset content.
 *
 * \param numElements The number of elements stored in the buffer (not byte size!).
 * \param buff        Memory to copy content from.
 * \return NULL on failure.
 *
 */
Vector* vector_create_from_buffer(size_t elementSize, size_t numElements, const void* buff);

/**
 * \brief Creates a copy of a vector.
 *
 * \return NULL on failure.
 *
 */
Vector* vector_create_copy(Vector* src);

/**
 * \brief Makes sure the vector is freed properly.
 *
 */
void vector_free(Vector* vector);

/**
 * \brief Clears the content of a vector.
 *
 */
void vector_clear(Vector* vector);

/**
 * \brief Returns the size of the vector in bytes.
 *
 */
size_t vector_get_byte_size(Vector* vector);

/**
 * \brief Returns the size of the vector in elements.
 *
 */
size_t vector_get_size(Vector* vector);

/**
 * \brief Requests a minimum capacity, which will hold as long as nothing is erased.
 *
 * \return If zero, out of memory.
 *
 */
int vector_reserve(Vector* vector, size_t numElements);

/**
 * \brief Returns an iterator of the element at a given index.
 *
 * This method does not check the bounds!
 *
 */ 
VectorIterator vector_at(Vector* vector, size_t index);

/**
 * \brief Increments an iterator to the next element.
 *
 */
VectorIterator vector_next(Vector* vector, VectorIterator it);

/**
 * \brief Decrements an iterator to the previous element.
 *
 */
VectorIterator vector_previous(Vector* vector, VectorIterator it);

/**
 * \brief Advances an iterator an arbitrary amount of elements (can be negative).
 *
 */
VectorIterator vector_advance(Vector* vector, VectorIterator it, int num);

/**
 * \brief Inserts an element at a given iterator.
 *
 * \return The iterator to the new element (NULL on failure).
 *
 */
VectorIterator vector_insert(Vector* vector, const void* element, VectorIterator pos);

/**
 * \brief Inserts an element after a given index.
 *
 * \return The iterator to the new element (NULL on failure).
 *
 */
VectorIterator vector_insert_at(Vector* vector, const void* element, size_t index);

/**
 * \brief Inserts a range of elements at a given iterator (can be this vector).
 *
 * \param num   Number of elements to insert.
 * \param start The start of the range.
 * \return The iterator to the beginning of the inserted range (NULL on failure).
 *
 */
VectorIterator vector_insert_range(Vector* vector, size_t num, VectorIterator start, VectorIterator pos);

/**
 * \brief Inserts a range of elements at a given index (can be this vector).
 *
 * \param num   Number of elements to insert.
 * \param start The start of the range.
 * \return The iterator to the beginning of the inserted range (NULL on failure).
 *
 */
VectorIterator vector_insert_range_at(Vector* vector, size_t num, VectorIterator start, size_t index);

/**
 * \brief Erases an element at a given iterator.
 *
 * \return The iterator to the element taking its place.
 *
 */
VectorIterator vector_erase(Vector* vector, VectorIterator pos);

/**
 * \brief Erases an element at a given index.
 *
 * \return The iterator to the element taking its place.
 *
 */
VectorIterator vector_erase_at(Vector* vector, size_t index);

/**
 * \brief Erases a range of elements at a given iterator.
 *
 * \param num   Number of elements to erase.
 * \param start The start of the range.
 * \return The iterator to the element taking its place.
 *
 */
VectorIterator vector_erase_range(Vector* vector, size_t num, VectorIterator start);

/**
 * \brief Erases a range of elements at a given index.
 *
 * \param num   Number of elements to erase.
 * \param start The start of the range.
 * \return The iterator to the element taking its place.
 *
 */
VectorIterator vector_erase_range_at(Vector* vector, size_t num, size_t index);

/**
 * \brief Finds an element equal to the given element and returns the iterator to it.
 *
 * \param fun The comparison function to use, it should return a non-zero value if the values are considered equal.
 * \return An iterator to the first found element, equals vector->end if none were found.
 *
 */
VectorIterator vector_find(Vector* vector, const void* value, VectorComparison fun);


#ifdef __cplusplus
}
#endif

#endif // GFX_CONTAINERS_VECTOR_H
