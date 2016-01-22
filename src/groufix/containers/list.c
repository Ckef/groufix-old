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

#include "groufix/containers/list.h"
#include "groufix/core/errors.h"

#include <stdlib.h>

/******************************************************/
GFXList* gfx_list_create(

		size_t dataSize)
{
	/* Create a new list node */
	GFXList* list = malloc(dataSize);

	/* Out of memory error */
	if(!list) gfx_errors_output(
		"[GFX Out Of Memory]: List could not be allocated."
	);

	/* Initialize */
	list->next = NULL;
	list->previous = NULL;

	return list;
}

/******************************************************/
void gfx_list_free(

		GFXList* list)
{
	if(list)
	{
		/* If not root, fix it */
		if(list->previous)
			list->previous->next = NULL;

		while(list)
		{
			/* Get next, free, and continue */
			GFXList* next = list->next;
			free(list);
			list = next;
		}
	}
}

/******************************************************/
size_t gfx_list_get_size(

		const GFXList* list)
{
	size_t cnt = 1;
	while(list->next)
	{
		list = list->next;
		++cnt;
	}

	return cnt;
}

/******************************************************/
GFXList* gfx_list_at(

		GFXList*  node,
		long      num)
{
	/* Move forwards */
	if(num > 0) while(num--)
	{
		node = node->next;
		if(!node) break;
	}

	/* Move backwards */
	else while(num++)
	{
		node = node->previous;
		if(!node) break;
	}

	return node;
}

/******************************************************/
GFXList* gfx_list_insert_after(

		GFXList*  node,
		size_t    dataSize)
{
	/* Create the new node */
	GFXList* new = gfx_list_create(dataSize);
	if(!new) return NULL;

	new->previous = node;
	new->next = node->next;

	/* Set the previous node */
	if(node->next) node->next->previous = new;

	/* Set the next node */
	node->next = new;

	return new;
}

/******************************************************/
GFXList* gfx_list_insert_before(

		GFXList*  node,
		size_t    dataSize)
{
	/* Create the new node */
	GFXList* new = gfx_list_create(dataSize);
	if(!new) return NULL;

	new->previous = node->previous;
	new->next = node;

	/* Set the next node */
	if(node->previous) node->previous->next = new;

	/* Set the previous node */
	node->previous = new;

	return new;
}

/******************************************************/
GFXList* gfx_list_insert_at(

		GFXList*  list,
		size_t    dataSize,
		long      index)
{
	/* Just insert where we already are */
	if(!index)
		return gfx_list_insert_before(list, dataSize);

	/* Go to the node after the new node */
	if(index < 0)
	{
		list = gfx_list_at(list, index);
		if(!list) return NULL;

		return gfx_list_insert_before(list, dataSize);
	}

	/* Go to the node before the new node */
	list = gfx_list_at(list, index - 1);
	if(!list) return NULL;

	return gfx_list_insert_after(list, dataSize);
}

/******************************************************/
GFXList* gfx_list_erase(

		GFXList* node)
{
	GFXList* new = NULL;

	/* Set next node */
	if(node->previous)
	{
		node->previous->next = node->next;
		new = node->previous;
	}

	/* Set previous node */
	if(node->next)
	{
		node->next->previous = node->previous;
		new = node->next;
	}

	/* Destroy the node */
	free(node);

	return new;
}

/******************************************************/
GFXList* gfx_list_erase_at(

		GFXList*  list,
		long      index)
{
	list = gfx_list_at(list, index);
	if(!list) return NULL;

	return gfx_list_erase(list);
}

/******************************************************/
GFXList* gfx_list_erase_range(

		GFXList*  first,
		GFXList*  last)
{
	GFXList* new = gfx_list_unsplice_range(first, last);
	gfx_list_free(first);

	return new;
}

/******************************************************/
void gfx_list_splice_range_after(

		GFXList*  first,
		GFXList*  last,
		GFXList*  pos)
{
	/* Close the gap of the range */
	if(first->previous) first->previous->next = last->next;
	if(last->next) last->next->previous = first->previous;

	/* Link in the range */
	first->previous = pos;
	last->next = pos->next;

	/* Set the previous node */
	if(pos->next) pos->next->previous = last;

	/* Set the next node */
	pos->next = first;
}

/******************************************************/
void gfx_list_splice_range_before(

		GFXList*  first,
		GFXList*  last,
		GFXList*  pos)
{
	/* Close the gap of the range */
	if(first->previous) first->previous->next = last->next;
	if(last->next) last->next->previous = first->previous;

	/* Link in the range */
	first->previous = pos->previous;
	last->next = pos;

	/* Set the next node */
	if(pos->previous) pos->previous->next = first;

	/* Set the previous node */
	pos->previous = last;
}

/******************************************************/
void gfx_list_splice_range_at(

		GFXList*  first,
		GFXList*  last,
		GFXList*  list,
		long      index)
{
	/* Just insert where we already are */
	if(!index)
		gfx_list_splice_range_before(first, last, list);

	else if(index < 0)
	{
		/* Go to the node after the new range */
		list = gfx_list_at(list, index);
		if(list) gfx_list_splice_range_before(first, last, list);
	}
	else
	{
		/* Go to the node before the new range */
		list = gfx_list_at(list, index - 1);
		if(list) gfx_list_splice_range_after(first, last, list);
	}
}

/******************************************************/
GFXList* gfx_list_unsplice_range(

		GFXList*  first,
		GFXList*  last)
{
	GFXList* new = NULL;

	/* Close the gap of the range */
	if(first->previous)
	{
		first->previous->next = last->next;
		new = first->previous;
	}
	if(last->next)
	{
		last->next->previous = first->previous;
		new = last->next;
	}

	/* Unlink range */
	first->previous = NULL;
	last->next = NULL;

	return new;
}

/******************************************************/
void gfx_list_swap(

		GFXList*  node1,
		GFXList*  node2)
{
	/* Relink outer nodes and fix internal links */
	if(node1->next == node2)
	{
		if(node1->previous) node1->previous->next = node2;
		if(node2->next) node2->next->previous = node1;

		node1->next = node1;
		node2->previous = node2;
	}
	else if(node2->next == node1)
	{
		if(node1->next) node1->next->previous = node2;
		if(node2->previous) node2->previous->next = node1;

		node1->previous = node1;
		node2->next = node2;
	}
	else
	{
		/* Relink the two nodes */
		if(node1->previous) node1->previous->next = node2;
		if(node1->next) node1->next->previous = node2;

		if(node2->previous) node2->previous->next = node1;
		if(node2->next) node2->next->previous = node1;
	}

	/* Swap the two nodes */
	GFXList temp = *node1;
	*node1 = *node2;
	*node2 = temp;
}
