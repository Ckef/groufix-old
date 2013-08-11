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
List* list_create(size_t dataSize)
{
	/* Create a new list node */
	List* list = (List*)calloc(1, sizeof(list));
	if(!list) return NULL;

	list->data = malloc(dataSize);

	return list;
}

/******************************************************/
void list_free(List* list)
{
	if(list)
	{
		/* If not root, fix it */
		if(list->previous) list->previous->next = NULL;

		while(list)
		{
			/* Get next, free, and continue */
			List* next = list->next;
			free(list->data);
			free(list);
			list = next;
		}
	}
}

/******************************************************/
size_t list_get_size(List* list)
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
List* list_at(List* list, size_t index)
{
	return list_advance(list, index);
}

/******************************************************/
List* list_advance(List* node, int num)
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
List* list_insert_after(List* node, size_t dataSize)
{
	/* Create the new node */
	List* new = list_create(dataSize);
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
List* list_insert_before(List* node, size_t dataSize)
{
	/* Create the new node */
	List* new = list_create(dataSize);
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
List* list_insert_at(List* list, size_t dataSize, size_t index)
{
	/* Just insert where we already are */
	if(!index) return list_insert_before(list, dataSize);

	/* Get the node before the new node */
	list = list_at(list, index - 1);
	if(!list) return NULL;

	return list_insert_after(list, dataSize);
}

/******************************************************/
List* list_erase(List* node)
{
	List* new = NULL;

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
	free(node->data);
	free(node);

	return new;
}

/******************************************************/
List* list_erase_at(List* list, size_t index)
{
	list = list_at(list, index);
	if(!list) return NULL;

	return list_erase(list);
}
