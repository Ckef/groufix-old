/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
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
	GFXList* list = calloc(1, dataSize);

	/* Out of memory error */
	if(!list) gfx_errors_push(
		GFX_ERROR_OUT_OF_MEMORY,
		"List could not be allocated."
	);

	return list;
}

/******************************************************/
void gfx_list_free(

		GFXList* list)
{
	if(list)
	{
		/* If not root, fix it */
		if(list->previous) list->previous->next = NULL;

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

		GFXList* list)
{
	size_t cnt = 1;
	while(list->next)
	{
		/* Get next, count */
		list = list->next;
		++cnt;
	}
	return cnt;
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

	/* Set the previous element */
	if(node->next) node->next->previous = new;

	/* Set the next element */
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

	/* Set the next element */
	if(node->previous) node->previous->next = new;

	/* Set the previous element */
	node->previous = new;

	return new;
}

/******************************************************/
GFXList* gfx_list_insert_at(

		GFXList*  list,
		size_t    dataSize,
		size_t    index)
{
	/* Just insert where we already are */
	if(!index) return gfx_list_insert_before(list, dataSize);

	/* Get the node before the new node */
	list = gfx_list_at(list, index - 1);
	if(!list) return NULL;

	return gfx_list_insert_after(list, dataSize);
}

/******************************************************/
GFXList* gfx_list_erase(

		GFXList* node)
{
	GFXList* new = NULL;

	/* Set next element */
	if(node->previous)
	{
		node->previous->next = node->next;
		new = node->previous;
	}

	/* Set previous element */
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
		size_t    index)
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
	GFXList* new = NULL;

	/* Set next element */
	if(first->previous)
	{
		first->previous->next = last->next;
		new = first->previous;
	}

	/* Set previous element */
	if(last->next)
	{
		last->next->previous = first->previous;
		new = last->next;
	}

	/* Unlink range */
	first->previous = NULL;
	last->next = NULL;

	gfx_list_free(first);

	return new;
}

/******************************************************/
void gfx_list_splice_after(

		GFXList*  node,
		GFXList*  pos)
{
	if(node != pos)
	{
		/* Close the gap of its current position */
		if(node->previous) node->previous->next = node->next;
		if(node->next) node->next->previous = node->previous;

		node->previous = pos;
		node->next = pos->next;

		/* Set the previous element */
		if(pos->next) pos->next->previous = node;

		/* Set the next element */
		pos->next = node;
	}
}

/******************************************************/
void gfx_list_splice_before(

		GFXList*  node,
		GFXList*  pos)
{
	if(node != pos)
	{
		/* Close the gap of its current position */
		if(node->previous) node->previous->next = node->next;
		if(node->next) node->next->previous = node->previous;

		node->previous = pos->previous;
		node->next = pos;

		/* Set the next element */
		if(pos->previous) pos->previous->next = node;

		/* Set the previous element */
		pos->previous = node;
	}
}

/******************************************************/
void gfx_list_splice_range_after(

		GFXList*  node,
		GFXList*  first,
		GFXList*  last)
{
	/* Close the gap of the range */
	if(first->previous) first->previous->next = last->next;
	if(last->next) last->next->previous = first->previous;

	/* Link in the range */
	first->previous = node;
	last->next = node->next;

	/* Set the previous element */
	if(node->next) node->next->previous = last;

	/* Set the next element */
	node->next = first;
}

/******************************************************/
void gfx_list_splice_range_before(

		GFXList*  node,
		GFXList*  first,
		GFXList*  last)
{
	/* Close the gap of the range */
	if(first->previous) first->previous->next = last->next;
	if(last->next) last->next->previous = first->previous;

	/* Link in the range */
	first->previous = node->previous;
	last->next = node;

	/* Set the next element */
	if(node->previous) node->previous->next = first;

	/* Set the previous element */
	node->previous = last;
}

/******************************************************/
GFXList* gfx_list_unsplice(

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

/******************************************************/
GFXList* gfx_list_advance(

		GFXList*  node,
		int       num)
{
	if(num > 0) while(num--)
	{
		node = node->next;
		if(!node) break;
	}
	else while(num++)
	{
		node = node->previous;
		if(!node) break;
	}
	return node;
}
