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

#include "groufix/internal.h"
#include "groufix/containers/deque.h"
#include "groufix/containers/vector.h"
#include "groufix/errors.h"

#include <string.h>

/******************************************************/
/* Created objects, index = ID */
static GFXVector* _gfx_hw_objects = NULL;

/* Free IDs, not at the end */
static GFXDeque* _gfx_hw_ids = NULL;

/* Actual object storage */
struct GFX_Internal_Hardware_Object
{
	void* handle;
	const GFX_Hardware_Funcs* funcs;
};

/******************************************************/
int gfx_hardware_is_extension_supported(GFXExtension extension)
{
	GFX_Internal_Window* wind = _gfx_window_get_current();
	if(!wind) return 0;

	return wind->extensions.flags[extension];
}

/******************************************************/
unsigned int gfx_hardware_poll_errors(const char* description)
{
	unsigned int count = 0;

	/* Check if there is a context */
	if(_gfx_window_get_current())
	{
		/* Loop over all errors */
		GLenum err = glGetError();
		while(err != GL_NO_ERROR)
		{
			gfx_errors_push(err, description);
			err = glGetError();

			++count;
		}
	}
	return count;
}

/******************************************************/
size_t _gfx_hardware_object_register(void* object, const GFX_Hardware_Funcs* funcs)
{
	/* Create internal object */
	struct GFX_Internal_Hardware_Object internal;
	internal.handle = object;
	internal.funcs = funcs;

	size_t id = 0;

	if(_gfx_hw_ids)
	{
		/* Replace an empty ID */
		id = *(size_t*)_gfx_hw_ids->begin;
		gfx_deque_pop_front(_gfx_hw_ids);

		*(struct GFX_Internal_Hardware_Object*)gfx_vector_at(_gfx_hw_objects, id - 1) = internal;

		/* Erase deque */
		if(_gfx_hw_ids->begin == _gfx_hw_ids->end)
		{
			gfx_deque_free(_gfx_hw_ids);
			_gfx_hw_ids = NULL;
		}
	}
	else
	{
		if(!_gfx_hw_objects)
		{
			/* Create vector if it doesn't exist yet */
			_gfx_hw_objects = gfx_vector_create(sizeof(struct GFX_Internal_Hardware_Object));
			if(!_gfx_hw_objects) return 0;
		}

		/* Insert a new object at the end (issue size after insertion to get index + 1) */
		if(gfx_vector_insert(_gfx_hw_objects, &internal, _gfx_hw_objects->end) != _gfx_hw_objects->end)
			id = gfx_vector_get_size(_gfx_hw_objects);
	}
	return id;
}

/******************************************************/
void* _gfx_hardware_object_get(size_t id)
{
	if(!id) return NULL;

	return ((struct GFX_Internal_Hardware_Object*)gfx_vector_at(_gfx_hw_objects, id - 1))->handle;
}

/******************************************************/
void _gfx_hardware_object_unregister(size_t id)
{
	if(_gfx_hw_objects && id)
	{
		size_t size = gfx_vector_get_size(_gfx_hw_objects);
		if(id < size)
		{
			if(!_gfx_hw_ids)
			{
				/* Create deque if it doesn't exist yet */
				_gfx_hw_ids = gfx_deque_create(sizeof(size_t));
				if(!_gfx_hw_ids) return;
			}

			/* Add ID as empty ID and empty the actual data */
			gfx_deque_push_back(_gfx_hw_ids, &id);
			memset(gfx_vector_at(_gfx_hw_objects, id - 1), 0, sizeof(struct GFX_Internal_Hardware_Object));
		}
		else if(id == size)
		{
			/* Remove last element */
			gfx_vector_erase_at(_gfx_hw_objects, id - 1);

			/* Erase vector */
			if(_gfx_hw_objects->begin == _gfx_hw_objects->end)
			{
				gfx_vector_free(_gfx_hw_objects);
				_gfx_hw_objects = NULL;
			}
		}
	}
}

/******************************************************/
void _gfx_hardware_objects_free(const GFX_Extensions* ext)
{
	if(_gfx_hw_objects)
	{
		/* Issue free request */
		GFXVectorIterator it;
		for(it = _gfx_hw_objects->begin; it != _gfx_hw_objects->end; it = gfx_vector_next(_gfx_hw_objects, it))
		{
			struct GFX_Internal_Hardware_Object* obj = (struct GFX_Internal_Hardware_Object*)it;
			if(obj->funcs) if(obj->funcs->free) obj->funcs->free(obj->handle, ext);
		}

		/* Unregister all */
		gfx_vector_free(_gfx_hw_objects);
		gfx_deque_free(_gfx_hw_ids);

		_gfx_hw_objects = NULL;
		_gfx_hw_ids = NULL;
	}
}

/******************************************************/
void _gfx_hardware_objects_save(const GFX_Extensions* ext)
{
	/* Issue save method */
	GFXVectorIterator it;
	if(_gfx_hw_objects) for(it = _gfx_hw_objects->begin; it != _gfx_hw_objects->end; it = gfx_vector_next(_gfx_hw_objects, it))
	{
		/* Store pointer to arbitrary storage */
		struct GFX_Internal_Hardware_Object* obj = (struct GFX_Internal_Hardware_Object*)it;
		if(obj->funcs) if(obj->funcs->save) obj->funcs->save(obj->handle, ext);
	}
}

/******************************************************/
void _gfx_hardware_objects_restore(const GFX_Extensions* ext)
{
	/* Issue restore method */
	GFXVectorIterator it;
	if(_gfx_hw_objects) for(it = _gfx_hw_objects->begin; it != _gfx_hw_objects->end; it = gfx_vector_next(_gfx_hw_objects, it))
	{
		/* Restore from pointer if given */
		struct GFX_Internal_Hardware_Object* obj = (struct GFX_Internal_Hardware_Object*)it;
		if(obj->funcs) if(obj->funcs->restore) obj->funcs->restore(obj->handle, ext);
	}
}
