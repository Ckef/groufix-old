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

#include "groufix/core/errors.h"
#include "groufix/core/renderer.h"

#include <stdlib.h>

/******************************************************/
/** Internal temporary storage */
typedef struct GFX_RenderObjectStorage
{
	GFX_RenderObjectID*  id;
	void*                storage;

} GFX_RenderObjectStorage;


/******************************************************/
static int _gfx_render_object_id_ref(

		GFX_RenderObjectID*  id,
		GFX_RenderObjects*   cont)
{
	/* Check if it already exists */
	GFX_RenderObjectRef* ref;
	for(ref = &id->refs; ref; ref = ref->next)
		if(ref->objects == cont) return 0;

	/* Initialize */
	GFX_RenderObjectRef temp =
	{
		.objects = cont,
		.id      = 0,
		.next    = NULL
	};

	/* Allocate a new one if necessary */
	if(!id->refs.objects)
		ref = &temp;
	else
	{
		ref = malloc(sizeof(GFX_RenderObjectRef));
		if(!ref)
		{
			/* Out of memory error */
			gfx_errors_output(
				"[GFX Out Of Memory]: Render object ID ran out of memory during referencing."
			);
			return 0;
		}

		*ref = temp;
	}

	/* Insert the reference at the container */
	if(cont->empties.begin != cont->empties.end)
	{
		/* Replace an empty ID */
		ref->id = *(unsigned int*)cont->empties.begin;
		gfx_deque_pop_begin(&cont->empties);

		*(GFX_RenderObjectID**)gfx_vector_at(&cont->objects, ref->id - 1) = id;
	}
	else
	{
		/* Get index + 1 as ID, overflow? omg, many objects! */
		size_t size = gfx_vector_get_size(&cont->objects);
		unsigned int index = size + 1;

		if(index < size)
		{
			if(id->refs.objects) free(ref);
			return 0;
		}

		/* Insert new ID at the end */
		GFXVectorIterator it = gfx_vector_insert(
			&cont->objects,
			&id,
			cont->objects.end
		);

		if(it == cont->objects.end)
		{
			if(id->refs.objects) free(ref);
			return 0;
		}

		ref->id = index;
	}

	/* Insert it at the ID */
	if(!id->refs.objects)
		id->refs = *ref;
	else
	{
		ref->next = id->refs.next;
		id->refs.next = ref;
	}

	return 1;
}

/******************************************************/
static void _gfx_render_object_id_deref(

		GFX_RenderObjectID*  id,
		GFX_RenderObjects*   cont)
{
	/* Search for the reference */
	GFX_RenderObjectRef* ref;
	for(ref = &id->refs; ref; ref = ref->next)
		if(ref->objects == cont) break;

	if(!ref) return;

	/* Remove it from the container */
	size_t size = gfx_vector_get_size(&cont->objects);

	if(ref->id < size)
	{
		/* Empty it and save it in empties */
		*(GFX_RenderObjectID**)gfx_vector_at(&cont->objects, ref->id - 1) = NULL;
		gfx_deque_push_end(&cont->empties, &ref->id);
	}
	else
	{
		/* Remove last element */
		gfx_vector_erase_at(&cont->objects, --size);
	}

	/* Clear both deque and vector */
	if(!size || size == gfx_deque_get_size(&cont->empties))
	{
		gfx_vector_clear(&cont->objects);
		gfx_deque_clear(&cont->empties);
	}

	/* Remove the reference */
	if(id->refs.objects != cont)
	{
		/* Loop over all nodes and free the correct one */
		GFX_RenderObjectRef* prev = &id->refs;
		for(ref = prev->next; ref; ref = ref->next)
		{
			if(ref->objects == cont)
			{
				prev->next = ref->next;
				free(ref);

				break;
			}

			prev = prev->next;
		}
	}
	else
	{
		if(id->refs.next)
		{
			/* Free one node and move it to the structure itself */
			GFX_RenderObjectRef* del = id->refs.next;
			id->refs = *del;

			free(del);
		}
		else
		{
			id->refs.objects = NULL;
			id->refs.id = 0;

			/* This was the last, call the destruct callback */
			id->funcs->destruct((GFX_RenderObjectIDArg*)id);
		}
	}
}

/******************************************************/
int _gfx_render_objects_init(

		GFX_RenderObjects* cont)
{
	if(!_gfx_platform_mutex_init(&cont->mutex))
		return 0;

	gfx_vector_init(&cont->objects, sizeof(GFX_RenderObjectID*));
	gfx_deque_init(&cont->empties, sizeof(unsigned int));
	gfx_vector_init(&cont->temp, sizeof(GFX_RenderObjectStorage));

	return 1;
}

/******************************************************/
void _gfx_render_objects_clear(

		GFX_RenderObjects* cont)
{
	/* Dereference all objects */
	size_t index;
	for(
		index = gfx_vector_get_size(&cont->objects);
		index > 0;
		--index)
	{
		GFX_RenderObjectID* id =
			*(GFX_RenderObjectID**)gfx_vector_at(&cont->objects, index - 1);

		if(id) _gfx_render_object_id_deref(id, cont);
	}

	gfx_vector_clear(&cont->objects);
	gfx_deque_clear(&cont->empties);
	gfx_vector_clear(&cont->temp);

	_gfx_platform_mutex_clear(&cont->mutex);
}

/******************************************************/
void _gfx_render_objects_prepare(

		GFX_RenderObjects*  src,
		int                 shared)
{
	_gfx_platform_mutex_lock(&src->mutex);

	/* Loop over all IDs */
	size_t index;
	for(
		index = gfx_vector_get_size(&src->objects);
		index > 0;
		--index)
	{
		GFX_RenderObjectID* id =
			*(GFX_RenderObjectID**)gfx_vector_at(&src->objects, index - 1);

		if(id)
		{
			/* Assign it some temporary storage */
			GFX_RenderObjectStorage storage =
			{
				.id      = id,
				.storage = NULL
			};

			GFX_RenderObjectStorage* temp = gfx_vector_insert(
				&src->temp,
				&storage,
				src->temp.end
			);

			if(temp == src->temp.end)
			{
				/* Out of memory error */
				gfx_errors_output(
					"[GFX Out Of Memory]: Render object container ran out of memory during preparation."
				);
				continue;
			}

			/* Dereference it and call the preparation callback */
			_gfx_render_object_id_deref(
				id,
				src);

			id->funcs->prepare(
				(GFX_RenderObjectIDArg*)id,
				&temp->storage,
				shared);
		}
	}

	_gfx_platform_mutex_unlock(&src->mutex);
}

/******************************************************/
void _gfx_render_objects_transfer(

		GFX_RenderObjects*  src,
		GFX_RenderObjects*  dest,
		int                 shared)
{
	_gfx_platform_mutex_lock(&dest->mutex);

	/* Loop over temporary storage */
	GFX_RenderObjectStorage* it;
	for(
		it = src->temp.begin;
		it != src->temp.end;
		it = gfx_vector_next(&src->temp, it))
	{
		/* Reference it at destination and call the transfer callback */
		_gfx_render_object_id_ref(
			it->id,
			dest);

		it->id->funcs->transfer(
			(GFX_RenderObjectIDArg*)it->id,
			&it->storage,
			shared);
	}

	_gfx_platform_mutex_unlock(&dest->mutex);

	gfx_vector_clear(&src->temp);
}

/******************************************************/
int _gfx_render_object_id_init(

		GFX_RenderObjectID*           id,
		GFX_RenderObjectFlags         flags,
		const GFX_RenderObjectFuncs*  funcs,
		GFX_RenderObjects*            cont)
{
	/* Need to have a container if it needs a reference */
	if((flags & GFX_OBJECT_NEEDS_REFERENCE) && !cont)
		return 0;

	int success = 1;

	id->flags        = flags;
	id->funcs        = funcs;
	id->refs.objects = NULL;
	id->refs.id      = 0;
	id->refs.next    = NULL;

	if(cont)
	{
		_gfx_platform_mutex_lock(&cont->mutex);

		success = _gfx_render_object_id_ref(id, cont);

		_gfx_platform_mutex_unlock(&cont->mutex);
	}

	return success;
}

/******************************************************/
void _gfx_render_object_id_clear(

		GFX_RenderObjectID* id)
{
	/* Dereference it at all referenced containers */
	while(id->refs.objects)
	{
		GFX_RenderObjects* cont = id->refs.objects;

		_gfx_platform_mutex_lock(&cont->mutex);

		_gfx_render_object_id_deref(id, cont);

		_gfx_platform_mutex_unlock(&cont->mutex);
	}
}

/******************************************************/
int _gfx_render_object_id_reference(

		GFX_RenderObjectID*  id,
		GFX_RenderObjects*   cont)
{
	/* Check share flag */
	if(id->refs.objects && !(id->flags & GFX_OBJECT_CAN_SHARE))
		return 0;

	int success;

	_gfx_platform_mutex_lock(&cont->mutex);

	success = _gfx_render_object_id_ref(id, cont);

	_gfx_platform_mutex_unlock(&cont->mutex);

	return success;
}

/******************************************************/
int _gfx_render_object_id_dereference(

		GFX_RenderObjectID*  id,
		GFX_RenderObjects*   cont)
{
	/* Check needs reference flag */
	if(!id->refs.next && (id->flags & GFX_OBJECT_NEEDS_REFERENCE))
		return 0;

	_gfx_platform_mutex_lock(&cont->mutex);

	_gfx_render_object_id_deref(id, cont);

	_gfx_platform_mutex_unlock(&cont->mutex);

	return 1;
}
