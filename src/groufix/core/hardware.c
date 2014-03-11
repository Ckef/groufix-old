/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Groufix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Groufix.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "groufix/core/errors.h"
#include "groufix/core/internal.h"
#include "groufix/containers/deque.h"
#include "groufix/containers/vector.h"

#include <string.h>

/******************************************************/
/* Created objects, index = ID */
static GFXVector* _gfx_hw_objects = NULL;

/* Free IDs, not at the end */
static GFXDeque* _gfx_hw_ids = NULL;

/* Actual object storage */
struct GFX_HardwareObject
{
	void* handle;
	const GFX_HardwareFuncs* funcs;
};

/******************************************************/
int gfx_hardware_is_extension_supported(GFXExtension extension)
{
	GFX_Window* wind = _gfx_window_get_current();
	if(!wind) return 0;

	return wind->extensions.flags[extension];
}

/******************************************************/
int gfx_hardware_get_limit(GFXLimit limit)
{
	GFX_Window* wind = _gfx_window_get_current();
	if(!wind) return -1;

	return wind->extensions.limits[limit];
}

/******************************************************/
size_t _gfx_hardware_object_register(void* object, const GFX_HardwareFuncs* funcs)
{
	if(!funcs) return 0;

	/* Create internal object */
	struct GFX_HardwareObject internal;
	internal.handle = object;
	internal.funcs = funcs;

	size_t id = 0;

	if(_gfx_hw_ids)
	{
		/* Replace an empty ID */
		id = *(size_t*)_gfx_hw_ids->begin;
		gfx_deque_pop_front(_gfx_hw_ids);

		*(struct GFX_HardwareObject*)gfx_vector_at(_gfx_hw_objects, id - 1) = internal;

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
			_gfx_hw_objects = gfx_vector_create(sizeof(struct GFX_HardwareObject));
			if(!_gfx_hw_objects) return 0;
		}

		/* Get index + 1 as ID, overflow? omg, many objects! */
		id = gfx_vector_get_size(_gfx_hw_objects) + 1;
		if(!id) return 0;

		/* Insert a new object at the end */
		if(gfx_vector_insert(_gfx_hw_objects, &internal, _gfx_hw_objects->end) == _gfx_hw_objects->end)
		{
			if(_gfx_hw_objects->begin == _gfx_hw_objects->end)
			{
				gfx_vector_free(_gfx_hw_objects);
				_gfx_hw_objects = NULL;
			}
			return 0;
		}
	}
	return id;
}

/******************************************************/
void _gfx_hardware_object_unregister(size_t id)
{
	if(_gfx_hw_objects && id)
	{
		size_t size = gfx_vector_get_size(_gfx_hw_objects);

		/* Good job */
		if(id > size) return;

		else if(id < size)
		{
			/* Check if it is already empty */
			struct GFX_HardwareObject* obj = gfx_vector_at(_gfx_hw_objects, id - 1);
			if(!obj->funcs) return;

			memset(obj, 0, sizeof(struct GFX_HardwareObject));

			/* Save ID */
			if(!_gfx_hw_ids)
			{
				/* Create deque if it doesn't exist yet */
				_gfx_hw_ids = gfx_deque_create(sizeof(size_t));
				if(!_gfx_hw_ids) return;
			}
			gfx_deque_push_back(_gfx_hw_ids, &id);
		}
		else
		{
			/* Remove last element */
			gfx_vector_erase_at(_gfx_hw_objects, id - 1);
			--size;
		}

		/* Erase both deque and vector */
		if(!size || size == gfx_deque_get_size(_gfx_hw_ids))
		{
			gfx_vector_free(_gfx_hw_objects);
			gfx_deque_free(_gfx_hw_ids);

			_gfx_hw_objects = NULL;
			_gfx_hw_objects = NULL;
		}
	}
}

/******************************************************/
void _gfx_hardware_objects_free(GFX_Extensions* ext)
{
	if(_gfx_hw_objects)
	{
		/* Issue free request */
		GFXVectorIterator it;
		for(it = _gfx_hw_objects->begin; it != _gfx_hw_objects->end; it = gfx_vector_next(_gfx_hw_objects, it))
		{
			struct GFX_HardwareObject* obj = (struct GFX_HardwareObject*)it;
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
void _gfx_hardware_objects_save(GFX_Extensions* ext)
{
	/* Issue save method */
	GFXVectorIterator it;
	if(_gfx_hw_objects) for(it = _gfx_hw_objects->begin; it != _gfx_hw_objects->end; it = gfx_vector_next(_gfx_hw_objects, it))
	{
		/* Store pointer to arbitrary storage */
		struct GFX_HardwareObject* obj = (struct GFX_HardwareObject*)it;
		if(obj->funcs) if(obj->funcs->save) obj->funcs->save(obj->handle, ext);
	}
}

/******************************************************/
void _gfx_hardware_objects_restore(GFX_Extensions* ext)
{
	/* Issue restore method */
	GFXVectorIterator it;
	if(_gfx_hw_objects) for(it = _gfx_hw_objects->begin; it != _gfx_hw_objects->end; it = gfx_vector_next(_gfx_hw_objects, it))
	{
		/* Restore from pointer if given */
		struct GFX_HardwareObject* obj = (struct GFX_HardwareObject*)it;
		if(obj->funcs) if(obj->funcs->restore) obj->funcs->restore(obj->handle, ext);
	}
}