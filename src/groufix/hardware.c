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
#include "groufix/containers/vector.h"
#include "groufix/errors.h"

/******************************************************/
/* Created objects */
static GFXVector* _gfx_hw_objects = NULL;

/* Actual object storage */
struct GFX_Internal_Hardware_Object
{
	GFX_Hardware_Object        handle;
	const GFX_Hardware_Funcs*  funcs;
};

/******************************************************/
/* Binary search for an object */
static GFXVectorIterator _gfx_hardware_obj_find(size_t start, size_t end, GFX_Hardware_Object object)
{
	while(start < end)
	{
		size_t mid = start + ((end - start) >> 1);
		struct GFX_Internal_Hardware_Object* internal = gfx_vector_at(_gfx_hw_objects, mid);

		/* Move boundaries */
		if(object < internal->handle) end = mid;
		else if(object > internal->handle) start = mid + 1;

		else return (GFXVectorIterator)internal;
	}

	/* Return position to insert at */
	return gfx_vector_at(_gfx_hw_objects, start);
}

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
int _gfx_hardware_object_register(GFX_Hardware_Object object, const GFX_Hardware_Funcs* funcs)
{
	if(!_gfx_hw_objects)
	{
		/* Create vector if it doesn't exist yet */
		_gfx_hw_objects = gfx_vector_create(sizeof(struct GFX_Internal_Hardware_Object));
		if(!_gfx_hw_objects) return 0;
	}

	/* Try to find it and update functions */
	GFXVectorIterator it = _gfx_hardware_obj_find(0, gfx_vector_get_size(_gfx_hw_objects), object);
	if(((struct GFX_Internal_Hardware_Object*)it)->handle == object)
	{
		((struct GFX_Internal_Hardware_Object*)it)->funcs = funcs;
		return 1;
	}

	/* Create internal object and insert if not found */
	struct GFX_Internal_Hardware_Object internal;
	internal.handle = object;
	internal.funcs = funcs;

	return gfx_vector_insert(_gfx_hw_objects, &internal, it) != _gfx_hw_objects->end;
}

/******************************************************/
void _gfx_hardware_object_unregister(GFX_Hardware_Object object)
{
	if(_gfx_hw_objects)
	{
		/* Find and erase */
		GFXVectorIterator it = _gfx_hardware_obj_find(0, gfx_vector_get_size(_gfx_hw_objects), object);
		if(((struct GFX_Internal_Hardware_Object*)it)->handle == object)
			gfx_vector_erase(_gfx_hw_objects, it);

		/* Get rid of memory */
		if(_gfx_hw_objects->begin == _gfx_hw_objects->end)
		{
			gfx_vector_free(_gfx_hw_objects);
			_gfx_hw_objects = NULL;
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
			if(obj->funcs->free) obj->funcs->free(obj->handle, ext);
		}

		/* Unregister all */
		gfx_vector_free(_gfx_hw_objects);
		_gfx_hw_objects = NULL;
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
		if(obj->funcs->save) obj->funcs->save(obj->handle, ext);
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
		if(obj->funcs->restore) obj->funcs->restore(obj->handle, ext);
	}
}
