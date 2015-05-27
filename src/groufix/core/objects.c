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

#include "groufix/core/renderer.h"

#include <string.h>

/******************************************************/
/* Actual object storage */
typedef struct GFX_Object
{
	void*                         handle;
	const GFX_RenderObjectFuncs*  funcs; /* NULL when empty */

} GFX_Object;


/******************************************************/
void _gfx_render_objects_init(

		GFX_RenderObjects* cont)
{
	gfx_vector_init(&cont->objects, sizeof(GFX_Object));
	gfx_deque_init(&cont->empties, sizeof(unsigned int));
	gfx_vector_init(&cont->saved, sizeof(GFX_Object));
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
GFX_RenderObjectID _gfx_render_object_register(

		GFX_RenderObjects*            cont,
		void*                         object,
		const GFX_RenderObjectFuncs*  funcs)
{
	GFX_RenderObjectID id =
	{
		.objects = NULL,
		.id = 0
	};

	if(!funcs) return id;

	/* Create internal object */
	GFX_Object obj =
	{
		.handle = object,
		.funcs = funcs
	};

	if(cont->empties.begin != cont->empties.end)
	{
		/* Replace an empty ID */
		id.id = *(unsigned int*)cont->empties.begin;
		gfx_deque_pop_begin(&cont->empties);

		*(GFX_Object*)gfx_vector_at(&cont->objects, id.id - 1) = obj;
	}
	else
	{
		/* Get index + 1 as ID, overflow? omg, many objects! */
		size_t size = gfx_vector_get_size(&cont->objects);
		unsigned int index = size + 1;

		if(index < size)
		{
			/* Overflow error */
			gfx_errors_push(
				GFX_ERROR_OVERFLOW,
				"Overflow occurred during render object registration."
			);
			return id;
		}

		/* Insert a new object at the end */
		GFXVectorIterator it = gfx_vector_insert(
			&cont->objects,
			&obj,
			cont->objects.end
		);

		if(it == cont->objects.end)
			return id;

		id.id = index;
	}

	id.objects = cont;
	return id;
}

/******************************************************/
void _gfx_render_object_unregister(

		GFX_RenderObjectID id)
{
	if(!id.objects) return;

	/* Good job */
	size_t size = gfx_vector_get_size(&id.objects->objects);
	if(!id.id || id.id > size) return;

	if(id.id < size)
	{
		/* Check if it is already empty */
		GFX_Object* obj = gfx_vector_at(
			&id.objects->objects,
			id.id - 1
		);

		if(!obj->funcs) return;
		memset(obj, 0, sizeof(GFX_Object));

		/* Save ID */
		gfx_deque_push_end(&id.objects->empties, &id.id);
	}
	else
	{
		/* Remove last element */
		gfx_vector_erase_at(&id.objects->objects, --size);
	}

	/* Clear both deque and vector */
	if(!size || size == gfx_deque_get_size(&id.objects->empties))
	{
		gfx_vector_clear(&id.objects->objects);
		gfx_deque_clear(&id.objects->empties);
	}
}

/******************************************************/
void _gfx_render_objects_free(

		GFX_RenderObjects* cont)
{
	GFX_RenderObjectID id =
	{
		.objects = NULL,
		.id = 0
	};

	/* Issue free request */
	GFX_Object* it;
	for(
		it = cont->objects.begin;
		it != cont->objects.end;
		it = gfx_vector_next(&cont->objects, it))
	{
		if(it->funcs)
			it->funcs->free(it->handle, id);
	}

	/* Unregister all */
	gfx_vector_clear(&cont->objects);
	gfx_deque_clear(&cont->empties);
}

/******************************************************/
void _gfx_render_objects_save(

		GFX_RenderObjects* cont)
{
	GFX_RenderObjectID id =
	{
		.objects = cont,
		.id = 0
	};

	/* Issue save request */
	GFX_Object* it;
	for(
		it = cont->objects.begin;
		it != cont->objects.end;
		it = gfx_vector_next(&cont->objects, it))
	{
		if(it->funcs)
			it->funcs->save(it->handle, id);
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
	GFX_Object* it;
	for(
		it = src->saved.begin;
		it != src->saved.end;
		it = gfx_vector_next(&src->saved, it))
	{
		if(it->funcs)
		{
			/* Register at new container */
			GFX_RenderObjectID id = _gfx_render_object_register(
				cont,
				it->handle,
				it->funcs
			);

			/* And restore */
			it->funcs->restore(it->handle, id);
		}
	}

	/* Remove all saved objects */
	gfx_vector_clear(&src->saved);
}
