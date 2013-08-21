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


#ifdef __cplusplus
}
#endif

#endif // GFX_CONTAINERS_LIST_H
