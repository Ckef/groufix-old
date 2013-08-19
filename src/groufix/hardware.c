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
/* Actual object storage */
struct GFX_Internal_Hardware_Object
{
	GFX_Hardware_Object        handle;
	const GFX_Hardware_Funcs*  funcs;
};

/* Created objects */
static GFXVector* _gfx_hw_objects = NULL;

/* Saved client side memory */
static GFXVector* _gfx_hw_saved_objects = NULL;

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
	/* Create vector if it doesn't exist yet */
	if(!_gfx_hw_objects)
	{
		_gfx_hw_objects = gfx_vector_create(sizeof(struct GFX_Internal_Hardware_Object));
		if(!_gfx_hw_objects) return 0;
	}

	/* Create internal object */
	struct GFX_Internal_Hardware_Object internal;
	internal.handle = object;
	internal.funcs = funcs;

	return gfx_vector_insert(_gfx_hw_objects, &internal, _gfx_hw_objects->end) != _gfx_hw_objects->end;
}

/******************************************************/
void _gfx_hardware_object_unregister(GFX_Hardware_Object object)
{
	if(_gfx_hw_objects)
	{
		/* Find and erase */
		GFXVectorIterator it;
		for(it = _gfx_hw_objects->begin; it != _gfx_hw_objects->end; it = gfx_vector_next(_gfx_hw_objects, it))
			if(((struct GFX_Internal_Hardware_Object*)it)->handle == object) break;

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
	}

	/* Unregister all */
	gfx_vector_free(_gfx_hw_objects);
	_gfx_hw_objects = NULL;

	gfx_vector_free(_gfx_hw_saved_objects);
	_gfx_hw_saved_objects = NULL;
}

/******************************************************/
void _gfx_hardware_objects_save(const GFX_Extensions* ext)
{
	if(_gfx_hw_objects)
	{
		/* Create memory pool */
		gfx_vector_free(_gfx_hw_saved_objects);
		_gfx_hw_saved_objects = gfx_vector_create(sizeof(void*));

		gfx_vector_reserve(_gfx_hw_saved_objects, gfx_vector_get_size(_gfx_hw_objects));

		/* Issue save method */
		GFXVectorIterator objs = _gfx_hw_objects->begin;
		GFXVectorIterator save = _gfx_hw_saved_objects->begin;
		while(objs != _gfx_hw_objects->end)
		{
			/* Store pointer to arbitrary storage */
			struct GFX_Internal_Hardware_Object* obj = (struct GFX_Internal_Hardware_Object*)objs;
			void* data = NULL;

			if(obj->funcs->save) data = obj->funcs->save(obj->handle, ext);
			gfx_vector_insert(_gfx_hw_saved_objects, &data, save);

			/* Next */
			objs = gfx_vector_next(_gfx_hw_objects, objs);
			save = gfx_vector_next(_gfx_hw_saved_objects, save);
		}
	}
}

/******************************************************/
void _gfx_hardware_objects_restore(const GFX_Extensions* ext)
{
	if(_gfx_hw_saved_objects)
	{
		/* Issue restore method */
		GFXVectorIterator objs = _gfx_hw_objects->begin;
		GFXVectorIterator save = _gfx_hw_saved_objects->begin;
		while(save != _gfx_hw_saved_objects->end)
		{
			/* Restore from pointer if given */
			struct GFX_Internal_Hardware_Object* obj = (struct GFX_Internal_Hardware_Object*)objs;
			if(obj->funcs->restore)
			{
				void* data = *(void**)save;
				if(data) obj->funcs->restore(obj->handle, data, ext);
			}

			/* Next */
			objs = gfx_vector_next(_gfx_hw_objects, objs);
			save = gfx_vector_next(_gfx_hw_saved_objects, save);
		}

		/* Free saved storage */
		gfx_vector_free(_gfx_hw_saved_objects);
		_gfx_hw_saved_objects = NULL;
	}
}
