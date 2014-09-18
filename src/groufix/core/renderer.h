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

#ifndef GFX_CORE_INTERNAL_H
#define GFX_CORE_INTERNAL_H

#include "groufix/core/platform.h"
#include "groufix/core/pipeline.h"

/* Get renderer */
#if defined(GFX_GL) || defined(GFX_GLES)
	#define GFX_RENDERER_GL
	#include "groufix/core/renderer/gl.h"

#else
	#error "Renderer not supported"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Internal window data & methods
 *******************************************************/

/** Internal window */
typedef struct GFX_Window
{
	/* Super class */
	GFXWindow window;

	/* Hardware Extensions & Limits */
	unsigned char       flags[GFX_EXT_COUNT];
	int                 limits[GFX_LIM_COUNT];

	/* Hidden data */
	GFX_PlatformWindow  handle;
	GFXContext          context;  /* Context version */
	GFXPipeState        state;
	GFX_Renderer        renderer; /* Renderer data */

} GFX_Window;


/**
 * Returns the top level window associated with a platform window.
 *
 */
GFX_Window* _gfx_window_get_from_handle(

		GFX_PlatformWindow handle);

/**
 * Destroys the server side window.
 *
 * Creates a zombie window, the window struct still exists, but is not registered.
 * Thus, it must still be freed.
 *
 */
void _gfx_window_destroy(

		GFX_Window* window);

/**
 * Sets the window as the current context of the calling thread.
 *
 * Note: This SHOULD NOT be called unless you know damn well what you're doing.
 *
 */
void _gfx_window_make_current(

		GFX_Window* window);

/**
 * Returns the window as current context of the calling thread.
 *
 * @return The current window, can be NULL.
 *
 */
GFX_Window* _gfx_window_get_current(void);

/**
 * Swaps the internal buffers of the current window.
 *
 * Also polls errors of the context if error mode is debug.
 *
 */
void _gfx_window_swap_buffers(void);

/**
 * Loads the renderer of the current window and sets flags and limits.
 *
 * Note: this method assumes the context version of the window is set.
 *
 */
void _gfx_renderer_load(void);


/********************************************************
 * Generic hardware object reconstruction
 *******************************************************/

/** Generic hardware object operator */
typedef void (*GFX_HardwareObjectFunc) (void* object);


/** Hardware vtable, can all be NULL */
typedef struct GFX_HardwareFuncs
{
	GFX_HardwareObjectFunc  free;    /* GPU free request */
	GFX_HardwareObjectFunc  save;    /* Prepare for context destruction */
	GFX_HardwareObjectFunc  restore; /* Restore for new context */

} GFX_HardwareFuncs;


/**
 * Registers a new generic hardware object.
 *
 * @object Arbitrary data to identify with a number.
 * @funcs  Functions to associate with the object.
 * @return Identifier of the object, id > 1 (0 on failure).
 *
 * When an object is registered, it will be asked to free when all contexts are destroyed,
 * or reconstructed when the main context is destroyed.
 *
 */
unsigned int _gfx_hardware_object_register(

		void*                     object,
		const GFX_HardwareFuncs*  funcs);

/**
 * Unregisters a generic hardware object by identifier.
 *
 */
void _gfx_hardware_object_unregister(

		unsigned int id);

/**
 * Issue free request of all hardware objects, this happens when its parent context is destroyed.
 *
 * This will issue the free request and unregister ALL objects.
 * Thus this callback is NOT allowed to unregister the object.
 *
 */
void _gfx_hardware_objects_free(void);

/**
 * Issue save method of all hardware objects.
 *
 * During this operation, the current window and context are considered "deleted".
 * It is guaranteed another context is still active, this is only meant for objects which can't be shared.
 *
 */
void _gfx_hardware_objects_save(void);

/**
 * Issue restore method of all hardware objects.
 *
 * During this operation, a new window and context is current.
 *
 */
void _gfx_hardware_objects_restore(void);


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_INTERNAL_H
