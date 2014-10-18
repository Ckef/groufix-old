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

#include "groufix/core/errors.h"
#include "groufix/core/renderer.h"

#include <string.h>

/******************************************************/
/* Actual object storage */
struct GFX_Object
{
	void*                         handle;
	const GFX_RenderObjectFuncs*  funcs; /* NULL when empty */
};

/******************************************************/
void _gfx_render_objects_init(

		GFX_RenderObjects* cont)
{
	gfx_vector_init(&cont->objects, sizeof(struct GFX_Object));
	gfx_deque_init(&cont->empties, sizeof(unsigned int));
	gfx_vector_init(&cont->saved, sizeof(struct GFX_Object));
}

/******************************************************/
void _gfx_render_objects_clear(

		GFX_RenderObjects* cont)
{
	gfx_vector_clear(&cont->objects);
	gfx_deque_clear(&cont->empties);
	gfx_vector_clear(&cont->saved);
}

/******************************************************/
unsigned int _gfx_render_object_register(

		GFX_RenderObjects*            cont,
		void*                         object,
		const GFX_RenderObjectFuncs*  funcs)
{
	if(!funcs) return 0;

	/* Create internal object */
	struct GFX_Object obj;
	obj.handle = object;
	obj.funcs = funcs;

	unsigned int id = 0;

	if(cont->empties.begin != cont->empties.end)
	{
		/* Replace an empty ID */
		id = *(unsigned int*)cont->empties.begin;
		gfx_deque_pop_front(&cont->empties);

		*(struct GFX_Object*)gfx_vector_at(&cont->objects, id - 1) = obj;
	}
	else
	{
		/* Get index + 1 as ID, overflow? omg, many objects! */
		size_t size = gfx_vector_get_size(&cont->objects);
		id = size + 1;

		if(id < size)
		{
			/* Overflow error */
			gfx_errors_push(
				GFX_ERROR_OVERFLOW,
				"Overflow occurred during render object registration."
			);
			return 0;
		}

		/* Insert a new object at the end */
		GFXVectorIterator it = gfx_vector_insert(
			&cont->objects,
			&obj,
			cont->objects.end
		);

		if(it == cont->objects.end)
			return 0;
	}

	return id;
}

/******************************************************/
void _gfx_render_object_unregister(

		GFX_RenderObjects*  cont,
		unsigned int        id)
{
	size_t size = gfx_vector_get_size(&cont->objects);

	/* Good job */
	if(!id || id > size) return;

	if(id < size)
	{
		/* Check if it is already empty */
		struct GFX_Object* obj = gfx_vector_at(
			&cont->objects,
			id - 1
		);

		if(!obj->funcs) return;
		memset(obj, 0, sizeof(struct GFX_Object));

		/* Save ID */
		gfx_deque_push_back(&cont->empties, &id);
	}

	/* Remove last element */
	else gfx_vector_erase_at(&cont->objects, size - 1);
}

/******************************************************/
void _gfx_render_objects_free(

		GFX_RenderObjects* cont)
{
	/* Issue free request */
	GFXVectorIterator it;
	for(
		it = cont->objects.begin;
		it != cont->objects.end;
		it = gfx_vector_next(&cont->objects, it))
	{
		struct GFX_Object* obj = (struct GFX_Object*)it;

		if(obj->funcs)
			obj->funcs->free(obj->handle, 0);
	}

	/* Unregister all */
	gfx_vector_clear(&cont->objects);
	gfx_deque_clear(&cont->empties);
}

/******************************************************/
void _gfx_render_objects_save(

		GFX_RenderObjects* cont)
{
	/* Issue save request */
	GFXVectorIterator it;
	for(
		it = cont->objects.begin;
		it != cont->objects.end;
		it = gfx_vector_next(&cont->objects, it))
	{
		struct GFX_Object* obj = (struct GFX_Object*)it;

		if(obj->funcs)
			obj->funcs->save(obj->handle, 0);
	}

	/* Move all objects to saved vector */
	gfx_vector_insert_range(
		&cont->saved,
		gfx_vector_get_size(&cont->objects),
		cont->objects.begin,
		cont->saved.end
	);

	/* And unregister all */
	gfx_vector_clear(&cont->objects);
	gfx_deque_clear(&cont->empties);
}

/******************************************************/
void _gfx_render_objects_restore(

		GFX_RenderObjects*  src,
		GFX_RenderObjects*  cont)
{
	/* Reserve the needed space */
	size_t size =
		gfx_vector_get_size(&cont->objects) +
		gfx_vector_get_size(&src->saved) -
		gfx_deque_get_size(&cont->empties);

	gfx_vector_reserve(&cont->objects, size);

	/* Iterate over all saved objects */
	GFXVectorIterator it;
	for(
		it = src->saved.begin;
		it != src->saved.end;
		it = gfx_vector_next(&src->saved, it))
	{
		struct GFX_Object* obj = (struct GFX_Object*)it;

		if(obj->funcs)
		{
			/* Register at new container */
			unsigned int id = _gfx_render_object_register(
				cont,
				obj->handle,
				obj->funcs
			);

			/* And restore */
			obj->funcs->restore(obj->handle, id);
		}
	}

	/* Remove all saved objects */
	gfx_vector_clear(&src->saved);
}
