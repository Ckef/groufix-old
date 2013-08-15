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
 * Vector container
 *******************************************************/

/** \brief Vector iterator */
typedef void* GFXVectorIterator;


/** \brief Vector */
typedef struct GFXVector
{
	size_t elementSize;
	size_t capacity; /* in bytes */

	GFXVectorIterator begin;
	GFXVectorIterator end;

} GFXVector;


/**
 * \brief Creates a new vector.
 *
 * \return NULL on failure.
 *
 */
GFXVector* gfx_vector_create(size_t elementSize);

/**
 * \brief Creates a new vector with a preset content.
 *
 * \param numElements The number of elements stored in the buffer (not byte size!).
 * \param buff        Memory to copy content from.
 * \return NULL on failure.
 *
 */
GFXVector* gfx_vector_create_from_buffer(size_t elementSize, size_t numElements, const void* buff);

/**
 * \brief Creates a copy of a vector.
 *
 * \return NULL on failure.
 *
 */
GFXVector* gfx_vector_create_copy(GFXVector* src);

/**
 * \brief Makes sure the vector is freed properly.
 *
 */
void gfx_vector_free(GFXVector* vector);

/**
 * \brief Initializes a vector.
 *
 */
void gfx_vector_init(GFXVector* vector, size_t elementSize);

/**
 * \brief Initializes a vector with a preset content.
 *
 * \param numElements The number of elements stored in the buffer (not byte size!).
 * \param buff        Memory to copy content from.
 *
 */
void gfx_vector_init_from_buffer(GFXVector* vector, size_t elementSize, size_t numElements, const void* buff);

/**
 * \brief Initializes a copy of a vector.
 *
 */
void gfx_vector_init_copy(GFXVector* vector, GFXVector* src);

/**
 * \brief Clears the content of a vector.
 *
 */
void gfx_vector_clear(GFXVector* vector);

/**
 * \brief Returns the size of the vector in bytes.
 *
 */
size_t gfx_vector_get_byte_size(GFXVector* vector);

/**
 * \brief Returns the size of the vector in elements.
 *
 */
size_t gfx_vector_get_size(GFXVector* vector);

/**
 * \brief Requests a minimum capacity, which will hold as long as nothing is erased.
 *
 * \return If zero, out of memory.
 *
 */
int gfx_vector_reserve(GFXVector* vector, size_t numElements);

/**
 * \brief Returns an iterator of the element at a given index.
 *
 * This method does not check the bounds!
 *
 */ 
GFXVectorIterator gfx_vector_at(GFXVector* vector, size_t index);

/**
 * \brief Increments an iterator to the next element.
 *
 */
GFXVectorIterator gfx_vector_next(GFXVector* vector, GFXVectorIterator it);

/**
 * \brief Decrements an iterator to the previous element.
 *
 */
GFXVectorIterator gfx_vector_previous(GFXVector* vector, GFXVectorIterator it);

/**
 * \brief Advances an iterator an arbitrary amount of elements (can be negative).
 *
 */
GFXVectorIterator gfx_vector_advance(GFXVector* vector, GFXVectorIterator it, int num);

/**
 * \brief Inserts an element at a given iterator.
 *
 * \return The iterator to the new element (NULL or vector->end on failure).
 *
 */
GFXVectorIterator gfx_vector_insert(GFXVector* vector, const void* element, GFXVectorIterator pos);

/**
 * \brief Inserts an element after a given index.
 *
 * \return The iterator to the new element (NULL or vector->end on failure).
 *
 */
GFXVectorIterator gfx_vector_insert_at(GFXVector* vector, const void* element, size_t index);

/**
 * \brief Inserts a range of elements at a given iterator (can be this vector).
 *
 * \param num   Number of elements to insert.
 * \param start The start of the range.
 * \return The iterator to the beginning of the inserted range (NULL or vector->end on failure).
 *
 */
GFXVectorIterator gfx_vector_insert_range(GFXVector* vector, size_t num, GFXVectorIterator start, GFXVectorIterator pos);

/**
 * \brief Inserts a range of elements at a given index (can be this vector).
 *
 * \param num   Number of elements to insert.
 * \param start The start of the range.
 * \return The iterator to the beginning of the inserted range (NULL or vector->end on failure).
 *
 */
GFXVectorIterator gfx_vector_insert_range_at(GFXVector* vector, size_t num, GFXVectorIterator start, size_t index);

/**
 * \brief Erases an element at a given iterator.
 *
 * \return The iterator to the element taking its place (NULL on failure).
 *
 */
GFXVectorIterator gfx_vector_erase(GFXVector* vector, GFXVectorIterator pos);

/**
 * \brief Erases an element at a given index.
 *
 * \return The iterator to the element taking its place (NULL on failure).
 *
 */
GFXVectorIterator gfx_vector_erase_at(GFXVector* vector, size_t index);

/**
 * \brief Erases a range of elements at a given iterator.
 *
 * \param num   Number of elements to erase.
 * \param start The start of the range.
 * \return The iterator to the element taking its place (NULL on failure).
 *
 */
GFXVectorIterator gfx_vector_erase_range(GFXVector* vector, size_t num, GFXVectorIterator start);

/**
 * \brief Erases a range of elements at a given index.
 *
 * \param num   Number of elements to erase.
 * \param start The start of the range.
 * \return The iterator to the element taking its place (NULL on failure).
 *
 */
GFXVectorIterator gfx_vector_erase_range_at(GFXVector* vector, size_t num, size_t index);


#ifdef __cplusplus
}
#endif

#endif // GFX_CONTAINERS_VECTOR_H
