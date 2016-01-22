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

#ifndef GFX_CORE_RENDERER_H
#define GFX_CORE_RENDERER_H

#include "groufix.h"
#include "groufix/containers/deque.h"
#include "groufix/containers/vector.h"
#include "groufix/core/platform.h"


/* Get renderer */
#if defined(GFX_GL) || defined(GFX_GLES)
	#define GFX_RENDERER_GL
	#include "groufix/core/renderer/gl_def.h"
#elif defined(GFX_VULKAN)
	#define GFX_RENDERER_VK
	#error "Renderer not yet supported"

#else
	#error "Renderer not supported"
#endif


/* Whether a context can be current in multiple threads */
#if defined(GFX_RENDERER_GL)
	#define GFX_CONTEXT_MULTI_THREAD 0
#elif defined(GFX_RENDERER_VK)
	#define GFX_CONTEXT_MULTI_THREAD 1
#endif


/* Macros for safe current context fetching */
#define GFX_CONT_INIT_UNSAFE        GFX_Context* _gfx_c__ = _gfx_context_get_current();
#define GFX_CONT_INIT_BAD(r)        GFX_CONT_INIT_UNSAFE if(!_gfx_c__) return r;
#define GFX_CONT_INIT(r)            GFX_CONT_INIT_BAD(r)

#define GFX_CONT_EQ_BAD(x)          (_gfx_c__ == (x))
#define GFX_CONT_EQ(x)              GFX_CONT_EQ_BAD(x)

#define GFX_CONT_ARG                GFX_Context* _gfx_c__
#define GFX_CONT_AS_ARG             _gfx_c__
#define GFX_CONT_INT_AS_ARG_BAD(c)  (c)
#define GFX_CONT_INT_AS_ARG(c)      GFX_CONT_INT_AS_ARG_BAD(c)

#define GFX_CONT_GET                (*_gfx_c__)
#define GFX_REND_GET                (_gfx_c__->renderer)


/********************************************************
 * Renderer interface definitions
 *******************************************************/

/** Renderer data */
typedef struct GFX_Renderer GFX_Renderer;


/**
 * Loads the renderer of the current context and sets extension flags and limits.
 *
 * Note: this method may assume the context version is set and
 * the current renderer and relevant context fields are initialized to all 0s.
 *
 */
void _gfx_renderer_load(void);

/**
 * Allows the renderer to initialize errors for the current context.
 *
 */
void _gfx_renderer_init_errors(void);

/**
 * Allows the renderer to poll for errors of the current context.
 *
 */
void _gfx_renderer_poll_errors(void);

/**
 * Unloads and frees the renderer of the current context.
 *
 */
void _gfx_renderer_unload(void);


/********************************************************
 * Generic render object definitions
 *******************************************************/

/** Render object container */
typedef struct GFX_RenderObjects
{
	GFXVector  objects;
	GFXDeque   empties;
	GFXVector  saved;

} GFX_RenderObjects;


/** Render object ID */
typedef struct GFX_RenderObjectID
{
	GFX_RenderObjects*  objects;
	unsigned int        id;

} GFX_RenderObjectID;


/********************************************************
 * Generic render object reconstruction
 *******************************************************/

/** Generic render object operator */
typedef void (*GFX_RenderObjectFunc) (void*, GFX_RenderObjectID);


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
 * @param object Arbitrary data to identify with the render object.
 * @param funcs  Functions to associate with the object.
 * @return Identifier of the object, all 0s on failure.
 *
 */
GFX_RenderObjectID _gfx_render_object_register(

		GFX_RenderObjects*            cont,
		void*                         object,
		const GFX_RenderObjectFuncs*  funcs);

/**
 * Unregisters a render object at its container.
 *
 * @param id Identifier of the render object to unregister.
 *
 */
void _gfx_render_object_unregister(

		GFX_RenderObjectID id);

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
 * During this operation, a new context must be current.
 * An new ID for this new context is given.
 *
 */
void _gfx_render_objects_restore(

		GFX_RenderObjects*  src,
		GFX_RenderObjects*  cont);


/********************************************************
 * Internal context data & methods
 *******************************************************/

/** Internal generic context */
typedef struct GFX_Context
{
	/* Super class */
	GFXWindow window;

	/* Renderer Extensions & Limits */
	unsigned char        ext[GFX_EXT_COUNT];
	int                  lim[GFX_LIM_COUNT];

	/* Hidden data */
	GFXContext           version;  /* Context version */
	GFXPipeState         state;
	char                 offscreen;
	char                 swap;     /* Non-zero if it makes sense to swap buffers */

	GFX_PlatformWindow   handle;
	GFX_PlatformContext  context;
	GFX_Renderer         renderer; /* Renderer data */
	GFX_RenderObjects    objects;  /* Per context render objects */

} GFX_Context;


/**
 * Initializes the context manager.
 *
 * @param version Minimum context version to use.
 * @return Zero on failure.
 *
 */
int _gfx_context_manager_init(

		GFXContext version);

/**
 * Terminates the context manager.
 *
 * If it was not yet initialized before, this method has no effect.
 *
 */
void _gfx_context_manager_terminate(void);

/**
 * Creates a new off-screen context.
 *
 * Note: events have no effect on an off-screen context.
 *
 * The returned context does not behave as a regular context,
 * the only valid operations, besides destroying and freeing,
 * are making it current and using it as active renderer context.
 *
 */
GFX_Context* _gfx_context_create(void);

/**
 * Destroys the server side context.
 *
 * Creates a zombie context, the context struct still exists, but is not registered.
 * Thus, it must still be freed.
 *
 * Note: all objects are automatically saved and restored to the main context.
 *
 */
void _gfx_context_destroy(

		GFX_Context* context);

/**
 * Sets the context as the current context of the calling thread.
 *
 * @param context The context to make current, NULL to unmake any context current.
 *
 * This function is thread safe.
 *
 */
void _gfx_context_make_current(

		GFX_Context* context);

/**
 * Returns the current context of the calling thread.
 *
 * @return The current context, can be NULL.
 *
 * This function is thread safe.
 *
 */
GFX_Context* _gfx_context_get_current(void);

/**
 * Swaps the internal buffers of the current context.
 *
 */
void _gfx_context_swap_buffers(void);

/**
 * Returns the context associated with a platform window.
 *
 * Note: does not return off-screen contexts of dummy windows returned by
 * _gfx_platform_context_create.
 *
 */
GFX_Context* _gfx_context_get_from_handle(

		GFX_PlatformWindow handle);


#endif // GFX_CORE_RENDERER_H
