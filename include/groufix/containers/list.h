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

#ifndef GFX_CONTAINERS_LIST_H
#define GFX_CONTAINERS_LIST_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * List container
 *******************************************************/
typedef struct GFXList
{
	struct GFXList* next;
	struct GFXList* previous;

} GFXList;


/** 
 * Returns the data attached to a node.
 *
 */
inline void* gfx_list_get_data(GFXList* node)
{
	return (void*)(node + 1);
}

/**
 * Creates a new list.
 *
 * @param dataSize Size of the node, >= sizeof(List).
 * @return NULL on failure.
 *
 */
GFXList* gfx_list_create(size_t dataSize);

/**
 * Makes sure the list is freed properly.
 *
 * Frees each node after the given node.
 *
 */
void gfx_list_free(GFXList* list);

/**
 * Returns the size of the list in elements.
 *
 */
size_t gfx_list_get_size(GFXList* list);

/**
 * Returns a node at a given index, can be NULL.
 *
 */ 
GFXList* gfx_list_at(GFXList* list, size_t index);

/**
 * Advances a node an arbitrary amount of indices (can be negative).
 *
 */
GFXList* gfx_list_advance(GFXList* node, int num);

/**
 * Inserts an element after a given node.
 *
 * @param dataSize Size of the node, >= sizeof(List).
 * @return The node of the new element (NULL on failure).
 *
 */
GFXList* gfx_list_insert_after(GFXList* node, size_t dataSize);

/**
 * Inserts an element before a given node.
 *
 * @param dataSize Size of the node, >= sizeof(List).
 * @return The node of the new element (NULL on failure).
 *
 */
GFXList* gfx_list_insert_before(GFXList* node, size_t dataSize);

/**
 * Inserts an element after a given index.
 *
 * @param dataSize Size of the node, >= sizeof(List).
 * @return The node of the new element (NULL on failure).
 *
 */
GFXList* gfx_list_insert_at(GFXList* list, size_t dataSize, size_t index);

/**
 * Erases a node.
 *
 * @return The node of the element taking its place (can be NULL).
 *
 * If no node takes its place, it will try to return the previous node instead.
 *
 */
GFXList* gfx_list_erase(GFXList* node);

/**
 * Erases an element at a given index.
 *
 * @return The node of the element taking its place (can be NULL).
 *
 * If no node takes its place, it will try to return the previous node instead.
 *
 */
GFXList* gfx_list_erase_at(GFXList* list, size_t index);

/**
 * Erases a range of nodes.
 *
 * @return The node of the element taking its place (can be NULL).
 *
 * If no node takes its place, it will try to return the previous node instead.
 *
 */
GFXList* gfx_list_erase_range(GFXList* first, GFXList* last);

/**
 * Moves an element after a given node.
 *
 * @param node Node to splice.
 * @param pos  Position to move after.
 *
 */
void gfx_list_splice_after(GFXList* node, GFXList* pos);

/**
 * Moves an element before a given node.
 *
 * @param node Node to splice.
 * @param pos  Position to move before.
 *
 */
void gfx_list_splice_before(GFXList* node, GFXList* pos);

/**
 * Inserts a range of elements after a given node.
 *
 * @param first The first node of the range.
 * @param last  The last node of the range.
 *
 * Note: if node is within the range, behaviour is undefined.
 *
 */
void gfx_list_splice_range_after(GFXList* node, GFXList* first, GFXList* last);

/**
 * Inserts a range of elements before a given node.
 *
 * @param first The first node of the range.
 * @param last  The last node of the range.
 *
 * Note: if node is within the range, behaviour is undefined.
 *
 */
void gfx_list_splice_range_before(GFXList* node, GFXList* first, GFXList* last);

/**
 * Unlinks a range of elements, effectively creating two seperate lists.
 *
 * @param first The first node of the range.
 * @param last  The last node of the range.
 *
 * The two nodes can be equal to each other.
 *
 */
void gfx_list_unsplice(GFXList* first, GFXList* last);


#ifdef __cplusplus
}
#endif

#endif // GFX_CONTAINERS_LIST_H
