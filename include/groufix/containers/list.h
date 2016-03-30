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

#ifndef GFX_CONTAINERS_LIST_H
#define GFX_CONTAINERS_LIST_H

#include "groufix/utils.h"

#include <stddef.h>


/********************************************************
 * List container
 *******************************************************/

/** List node */
typedef struct GFXList
{
	struct GFXList*  next;
	struct GFXList*  previous;

} GFXList;


/**
 * Creates a new list.
 *
 * @param dataSize Size of the node, >= sizeof(GFXList).
 * @return NULL on failure.
 *
 */
GFX_API GFXList* gfx_list_create(

		size_t dataSize);

/**
 * Makes sure the list is freed properly.
 *
 * Frees each node after the given node.
 *
 */
GFX_API void gfx_list_free(

		GFXList* list);

/**
 * Returns the size of the list in nodes.
 *
 */
GFX_API size_t gfx_list_get_size(

		const GFXList* list);

/**
 * Returns a node at a given index, can be NULL.
 *
 * @param index Index relative to list, can be negative.
 *
 */
GFX_API GFXList* gfx_list_at(

		GFXList*  list,
		long      index);

/**
 * Inserts a node after a given node.
 *
 * @param dataSize Size of the node, >= sizeof(GFXList).
 * @return The new node (NULL on failure).
 *
 */
GFX_API GFXList* gfx_list_insert_after(

		GFXList*  node,
		size_t    dataSize);

/**
 * Inserts a node before a given node.
 *
 * @param dataSize Size of the node, >= sizeof(GFXList).
 * @return The new node (NULL on failure).
 *
 */
GFX_API GFXList* gfx_list_insert_before(

		GFXList*  node,
		size_t    dataSize);

/**
 * Inserts a node at a given index.
 *
 * @param dataSize Size of the node, >= sizeof(GFXList).
 * @return The new node (NULL on failure).
 *
 */
GFX_API GFXList* gfx_list_insert_at(

		GFXList*  list,
		size_t    dataSize,
		long      index);

/**
 * Erases a node.
 *
 * @return The node taking its place (can be NULL).
 *
 * If no node takes its place, it will try to return the previous node instead.
 *
 */
GFX_API GFXList* gfx_list_erase(

		GFXList* node);

/**
 * Erases a node at a given index.
 *
 * @return The node taking its place (can be NULL).
 *
 * If no node takes its place, it will try to return the previous node instead.
 *
 */
GFX_API GFXList* gfx_list_erase_at(

		GFXList*  list,
		long      index);

/**
 * Erases a range of nodes.
 *
 * @return The node taking their place (can be NULL).
 *
 * If no node takes their place, it will try to return the previous node instead.
 *
 */
GFX_API GFXList* gfx_list_erase_range(

		GFXList*  first,
		GFXList*  last);

/**
 * Inserts a range of nodes after a given node.
 *
 * @param first The first node of the range.
 * @param last  The last node of the range.
 *
 * Note: if pos is within the range, behaviour is undefined.
 *
 */
GFX_API void gfx_list_splice_range_after(

		GFXList*  first,
		GFXList*  last,
		GFXList*  pos);

/**
 * Inserts a range of nodes before a given node.
 *
 * @param first The first node of the range.
 * @param last  The last node of the range.
 *
 * Note: if pos is within the range, behaviour is undefined.
 *
 */
GFX_API void gfx_list_splice_range_before(

		GFXList*  first,
		GFXList*  last,
		GFXList*  pos);

/**
 * Inserts a range of nodes at a given index.
 *
 * Note: if list with an index offset is within the range, behaviour is undefined.
 *
 */
GFX_API void gfx_list_splice_range_at(

		GFXList*  first,
		GFXList*  last,
		GFXList*  list,
		long      index);

/**
 * Unlinks a range of nodes, effectively creating two seperate lists.
 *
 * @param first The first node of the range.
 * @param last  The last node of the range.
 * @return The node taking their place (can be NULL).
 *
 * If no node takes their place, it will try to return the previous node instead.
 *
 */
GFX_API GFXList* gfx_list_unsplice_range(

		GFXList*  first,
		GFXList*  last);

/**
 * Swaps two elements from position.
 *
 */
GFX_API void gfx_list_swap(

		GFXList*  node1,
		GFXList*  node2);

/**
 * Returns the data attached to a node.
 *
 */
static GFX_ALWAYS_INLINE void* gfx_list_get_data(

		GFXList* node)
{
	return (void*)(node + 1);
}

/**
 * Moves a node after a given node.
 *
 * @param node Node to splice.
 * @param pos  Position to move after.
 *
 */
static GFX_ALWAYS_INLINE void gfx_list_splice_after(

		GFXList*  node,
		GFXList*  pos)
{
	gfx_list_splice_range_after(node, node, pos);
}

/**
 * Moves a node before a given node.
 *
 * @param node Node to splice.
 * @param pos  Position to move before.
 *
 */
static GFX_ALWAYS_INLINE void gfx_list_splice_before(

		GFXList*  node,
		GFXList*  pos)
{
	gfx_list_splice_range_before(node, node, pos);
}

/**
 * Moves a node to a given index.
 *
 */
static GFX_ALWAYS_INLINE void gfx_list_splice_at(

		GFXList*  node,
		GFXList*  list,
		size_t    index)
{
	gfx_list_splice_range_at(node, node, list, index);
}

/**
 * Unlinks a node.
 *
 * @return The node taking its place (can be NULL).
 *
 * If no node takes its place, it will try to return the previous node instead.
 *
 */
static GFX_ALWAYS_INLINE GFXList* gfx_list_unsplice(

		GFXList* node)
{
	return gfx_list_unsplice_range(node, node);
}


#endif // GFX_CONTAINERS_LIST_H
