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

#ifndef GFX_CORE_INTERNAL_H
#define GFX_CORE_INTERNAL_H

#include "groufix/containers/deque.h"
#include "groufix/containers/vector.h"
#include "groufix/core/platform.h"
#include "groufix/core/pipeline.h"


/* Get renderer */
#if defined(GFX_GL) || defined(GFX_GLES)
	#define GFX_RENDERER_GL
	#include "groufix/core/renderer/gl.h"

#else
	#error "Renderer not supported"
#endif


/* Macros for safe current window/renderer fetching */
#define GFX_WIND_INIT_UNSAFE        GFX_Window* _gfx_w__ = _gfx_window_get_current();
#define GFX_WIND_INIT_BAD(r)        GFX_WIND_INIT_UNSAFE if(!_gfx_w__) return r;
#define GFX_WIND_INIT(r)            GFX_WIND_INIT_BAD(r)

#define GFX_WIND_EQ_BAD(x)          (_gfx_w__ == (x))
#define GFX_WIND_EQ(x)              GFX_WIND_EQ_BAD(x)

#define GFX_WIND_ARG                GFX_Window* _gfx_w__
#define GFX_WIND_AS_ARG             _gfx_w__
#define GFX_WIND_INT_AS_ARG_BAD(w)  (w)
#define GFX_WIND_INT_AS_ARG(w)      GFX_WIND_INT_AS_ARG_BAD(w)

#define GFX_WIND_GET                (*_gfx_w__)
#define GFX_REND_GET                (_gfx_w__->renderer)


#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Renderer interface definitions
 *******************************************************/

/** Renderer data */
typedef struct GFX_Renderer GFX_Renderer;


/**
 * Loads the renderer of the current window and sets flags and limits.
 *
 * Note: this method may assume the context version of the window is set and
 * the current renderer is initialized to all 0s.
 *
 */
void _gfx_renderer_load(void);

/**
 * Unloads and frees the renderer of the current window.
 *
 */
void _gfx_renderer_unload(void);


/********************************************************
 * Generic render object reconstruction
 *******************************************************/

/** Render object container */
typedef struct GFX_RenderObjects
{
	GFXVector  objects;
	GFXDeque   empties;
	GFXVector  saved;

} GFX_RenderObjects;


/** Generic render object operator */
typedef void (*GFX_RenderObjectFunc) (void*, unsigned int);


/** Operator vtable */
typedef struct GFX_RenderObjectFuncs
{
	GFX_RenderObjectFunc free;    /* Free all renderer data */
	GFX_RenderObjectFunc save;    /* Save off-renderer to restore in a new context later */
	GFX_RenderObjectFunc restore; /* Restore objects in a new context */

} GFX_RenderObjectFuncs;


/**
 * Initializes a render object container.
 *
 */
void _gfx_render_objects_init(

		GFX_RenderObjects* cont);

/**
 * Clears the content of a render object container.
 *
 */
void _gfx_render_objects_clear(

		GFX_RenderObjects* cont);

/**
 * Registers a render object at a container.
 *
 * @param cont   Container to register at.
 * @param object Aribtrary data to identify with the render object.
 * @param funcs  Functions to associate with the object.
 * @return Identifier of the object, 0 on failure.
 *
 */
unsigned int _gfx_render_object_register(

		GFX_RenderObjects*            cont,
		void*                         object,
		const GFX_RenderObjectFuncs*  funcs);

/**
 * Unregisters a render object at a container.
 *
 * @param id Identifier of the render object to unregister.
 *
 */
void _gfx_render_object_unregister(

		GFX_RenderObjects*  cont,
		unsigned int        id);

/**
 * Issue free method of all unsaved render objects.
 *
 * This will issue the free request and unregister all unsaved objects, sending 0 as new ID,
 * thus this callback is NOT allowed to unregister the object.
 * After this call, the current context will be destroyed.
 *
 */
void _gfx_render_objects_free(

		GFX_RenderObjects* cont);

/**
 * Issue save method of all unsaved render objects.
 *
 * This will issue a save request, sending 0 as new ID, as they should save on client side memory.
 * After this call, the current context may be destroyed.
 *
 */
void _gfx_render_objects_save(

		GFX_RenderObjects* cont);

/**
 * Issue restore method of all saved render objects.
 *
 * @param src Source container on which a save request was previously issued.
 *
 * During this operation, a new window and context must be current.
 * An new ID for this new context is given.
 *
 */
void _gfx_render_objects_restore(

		GFX_RenderObjects*  src,
		GFX_RenderObjects*  cont);


/********************************************************
 * Internal window data & methods
 *******************************************************/

/** Internal window */
typedef struct GFX_Window
{
	/* Super class */
	GFXWindow window;

	/* Renderer Extensions & Limits */
	unsigned char        ext[GFX_EXT_COUNT];
	int                  lim[GFX_LIM_COUNT];

	/* Hidden data */
	char                 offscreen;
	GFXContext           version;  /* Context version */
	GFXPipeState         state;
	GFX_PlatformWindow   handle;
	GFX_PlatformContext  context;

	GFX_Renderer         renderer; /* Renderer data */
	GFX_RenderObjects    objects;  /* Per window render objects */

} GFX_Window;


/**
 * Returns the top level window associated with a platform window.
 *
 */
GFX_Window* _gfx_window_get_from_handle(

		GFX_PlatformWindow handle);

/**
 * Creates a new off-screen window.
 *
 * @param w Width of the window.
 * @param h Height of the window.
 *
 * Note: events have no effect on an off-screen window.
 *
 * The returned window behaves as a regular window,
 * except that it is not destroyed at gfx_terminate.
 *
 */
GFX_Window* _gfx_window_create(

		GFXColorDepth  depth,
		unsigned int   w,
		unsigned int   h);

/**
 * Destroys the server side window.
 *
 * Creates a zombie window, the window struct still exists, but is not registered.
 * Thus, it must still be freed.
 *
 * Note: any saved objects are automatically restored in the main context,
 * if it still exists.
 *
 */
void _gfx_window_destroy(

		GFX_Window* window);

/**
 * Sets the window as the current context of the calling thread.
 *
 * @param window The window to make current, NULL to unmake any window current.
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


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_INTERNAL_H
