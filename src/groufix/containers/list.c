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

#include "groufix/containers/list.h"

#include <stdlib.h>

/******************************************************/
GFXList* gfx_list_create(size_t dataSize)
{
	/* Create a new list node */
	GFXList* list = calloc(1, dataSize);
	if(!list) return NULL;

	return list;
}

/******************************************************/
void gfx_list_free(GFXList* list)
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
size_t gfx_list_get_size(GFXList* list)
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
GFXList* gfx_list_at(GFXList* list, size_t index)
{
	return gfx_list_advance(list, index);
}

/******************************************************/
GFXList* gfx_list_advance(GFXList* node, int num)
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

/******************************************************/
GFXList* gfx_list_insert_after(GFXList* node, size_t dataSize)
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
GFXList* gfx_list_insert_before(GFXList* node, size_t dataSize)
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
GFXList* gfx_list_insert_at(GFXList* list, size_t dataSize, size_t index)
{
	/* Just insert where we already are */
	if(!index) return gfx_list_insert_before(list, dataSize);

	/* Get the node before the new node */
	list = gfx_list_at(list, index - 1);
	if(!list) return NULL;

	return gfx_list_insert_after(list, dataSize);
}

/******************************************************/
GFXList* gfx_list_erase(GFXList* node)
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
GFXList* gfx_list_erase_at(GFXList* list, size_t index)
{
	list = gfx_list_at(list, index);
	if(!list) return NULL;

	return gfx_list_erase(list);
}

/******************************************************/
void gfx_list_splice_after(GFXList* node, GFXList* pos)
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

/******************************************************/
void gfx_list_splice_before(GFXList* node, GFXList* pos)
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

/******************************************************/
void gfx_list_swap(GFXList* node1, GFXList* node2)
{
	/* Keep temporary */
	GFXList temp = *node1;
	*node1 = *node2;
	*node2 = temp;
}
