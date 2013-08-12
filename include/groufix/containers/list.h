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

#ifndef GFX_CONTAINERS_LIST_H
#define GFX_CONTAINERS_LIST_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * \brief List container
 *******************************************************/
typedef struct List
{
	struct List* next;
	struct List* previous;

} List;


/** 
 * \brief Returns the data attached to a node.
 *
 */
inline void* list_get_data(List* node)
{
	return (void*)(node + 1);
}

/**
 * \brief Creates a new list.
 *
 * \param dataSize Size of the attached data of the first node.
 * \return NULL on failure.
 *
 */
List* list_create(size_t dataSize);

/**
 * \brief Makes sure the list is freed properly.
 *
 */
void list_free(List* list);

/**
 * \brief Returns the size of the list in elements.
 *
 */
size_t list_get_size(List* list);

/**
 * \brief Returns a node at a given index, can be NULL.
 *
 */ 
List* list_at(List* list, size_t index);

/**
 * \brief Advances a node an arbitrary amount of indices (can be negative).
 *
 */
List* list_advance(List* node, int num);

/**
 * \brief Inserts an element after a given node.
 *
 * \param dataSize Size of the attached data of the new node.
 * \return The node of the new element (NULL on failure).
 *
 */
List* list_insert_after(List* node, size_t dataSize);

/**
 * \brief Inserts an element before a given node.
 *
 * \param dataSize Size of the attached data of the new node.
 * \return The node of the new element (NULL on failure).
 *
 */
List* list_insert_before(List* node, size_t dataSize);

/**
 * \brief Inserts an element after a given index.
 *
 * \param dataSize Size of the attached data of the new node.
 * \return The node of the new element (NULL on failure).
 *
 */
List* list_insert_at(List* list, size_t dataSize, size_t index);

/**
 * \brief Erases a node.
 *
 * \return The node of the element taking its place (can be NULL).
 *
 * If no node takes its place, it will try to return the previous node instead.
 *
 */
List* list_erase(List* node);

/**
 * \brief Erases an element at a given index.
 *
 * \return The node of the element taking its place (can be NULL).
 *
 * If no node takes its place, it will try to return the previous node instead.
 *
 */
List* list_erase_at(List* list, size_t index);


#ifdef __cplusplus
}
#endif

#endif // GFX_CONTAINERS_LIST_H
