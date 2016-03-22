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
#include "groufix/core/threading.h"


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
 * Generic render object functions
 *******************************************************/

/** Just a typedef for the argument */
typedef void* GFX_RenderObjectIDArg;


/** Render object destruct operator */
typedef void (*GFX_RenderObjectDestructFunc) (GFX_RenderObjectIDArg);


/** Render object transfer operator */
typedef void (*GFX_RenderObjectTransferFunc) (GFX_RenderObjectIDArg, void**, int);


/** Operator vtable */
typedef struct GFX_RenderObjectFuncs
{
	GFX_RenderObjectDestructFunc  destruct; /* When the last container is being dereferenced (its context is current) */
	GFX_RenderObjectTransferFunc  prepare;  /* When the current set of shared contexts will be out of use (one is current) */
	GFX_RenderObjectTransferFunc  transfer; /* When a new set of shared contexts is referenced (a new one is current) */

} GFX_RenderObjectFuncs;


/********************************************************
 * Generic render object container
 *******************************************************/

/** Render object container */
typedef struct GFX_RenderObjects
{
	GFXVector          objects;
	GFXDeque           empties;
	GFXVector          temp;
	GFX_PlatformMutex  mutex;

} GFX_RenderObjects;


/**
 * Initializes a render object container.
 *
 * @return Zero on failure.
 *
 * Note: NEVER copy the initialized container, the same pointer must always be used!
 *
 */
int _gfx_render_objects_init(

		GFX_RenderObjects* cont);

/**
 * Clears the content of a render object container.
 *
 * Also dereferences all IDs.
 * Note: will call the destruct callback if appropriate.
 *
 */
void _gfx_render_objects_clear(

		GFX_RenderObjects* cont);

/**
 * Issues the prepare callback for all IDs.
 *
 * @param src    The container to prepare all IDs from.
 * @param shared Non-zero if the destination will be shared with src.
 *
 * The callback offers a pointer to allow for temporary storage and
 * whether the src and destination are shared.
 *
 * This function is thread safe.
 *
 * Note: This call MUST be made before calling _gfx_render_objects_transfer
 * with the same containers as src. The function can be called multiple times
 * before calling _gfx_render_objects_transfer, but it cannot overlap with it.
 *
 * Also, the IDs cannot be accessed or altered during this call and the transfer
 * call.
 *
 */
void _gfx_render_objects_prepare(

		GFX_RenderObjects*  src,
		int                 shared);

/**
 * Subsequent call on _gfx_render_objects_prepare, issues the transfer callback for all IDs.
 *
 * @param src    The container all IDs were prepared from.
 * @param dest   The container all prepared IDs from src are moved to.
 * @param shared Non-zero if the destination is shared with src.
 *
 * The callback gives the same pointer as _gfx_render_objects_prepare so to
 * restore the temporary memory and whether the src and destination are shared.
 *
 * This function is thread safe.
 *
 * Note: This call MUST be made after calling _gfx_render_objects_prepare
 * with the same containers as src, but it cannot overlap with it.
 *
 */
void _gfx_render_objects_transfer(

		GFX_RenderObjects*  src,
		GFX_RenderObjects*  dest,
		int                 shared);


/********************************************************
 * Generic render object identification
 *******************************************************/

/** Flags associated with an object ID */
typedef enum GFX_RenderObjectFlags
{
	GFX_OBJECT_NEEDS_REFERENCE  = 0x01,
	GFX_OBJECT_CAN_SHARE        = 0x02

} GFX_RenderObjectFlags;


/** Render object reference */
typedef struct GFX_RenderObjectRef
{
	GFX_RenderObjects*           objects;
	unsigned int                 id;
	struct GFX_RenderObjectRef*  next;

} GFX_RenderObjectRef;


/** Render object ID */
typedef struct GFX_RenderObjectID
{
	GFX_RenderObjectFlags         flags;
	const GFX_RenderObjectFuncs*  funcs;
	GFX_RenderObjectRef           refs;

} GFX_RenderObjectID;


/**
 * Initializes a render object ID.
 *
 * @param flags Flags that will always be associated with this ID.
 * @param funcs Function vtable to associate with this ID.
 * @param cont  Render object container to first reference at (can be NULL).
 * @return Zero on failure.
 *
 * This function is thread safe.
 *
 * cont cannot be NULL if flags contains GFX_OBJECT_NEEDS_REFERENCE.
 * Note: NEVER copy the initialized ID, the same pointer must always be used!
 *
 */
int _gfx_render_object_id_init(

		GFX_RenderObjectID*           id,
		GFX_RenderObjectFlags         flags,
		const GFX_RenderObjectFuncs*  funcs,
		GFX_RenderObjects*            cont);

/**
 * Clears a render object ID.
 *
 * This function is thread safe.
 *
 * Also dereferences it at all containers.
 * Note: will call the destruct callback if appropriate.
 *
 */
void _gfx_render_object_id_clear(

		GFX_RenderObjectID* id);

/**
 * References the ID at the container.
 *
 * @param cont Render object container to reference at.
 * @return Zero on failure.
 *
 * This function is thread safe.
 *
 */
int _gfx_render_object_id_reference(

		GFX_RenderObjectID*  id,
		GFX_RenderObjects*   cont);

/**
 * Dereferences the ID at the container.
 *
 * @return Zero on failure.
 *
 * This function is thread safe.
 *
 * Note: will call the destruct callback if appropriate.
 *
 */
int _gfx_render_object_id_dereference(

		GFX_RenderObjectID*  id,
		GFX_RenderObjects*   cont);


/********************************************************
 * Internal context data & methods
 *******************************************************/

/** Renderer data */
typedef struct GFX_Renderer GFX_Renderer;


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
 * Returns the context associated with a platform window.
 *
 * Note: does not return off-screen contexts of dummy windows returned by
 * _gfx_platform_context_create.
 *
 */
GFX_Context* _gfx_context_get_from_handle(

		GFX_PlatformWindow handle);

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
 * Thus, it must still be freed with gfx_window_free.
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


/********************************************************
 * Renderer interface
 *******************************************************/

/**
 * Loads the renderer of the current context and sets extension flags and limits.
 *
 * Note: this method may assume the context version is set and
 * the current renderer and relevant context fields are initialized to all 0s.
 *
 */
void _gfx_renderer_load(

		GFX_CONT_ARG);

/**
 * Unloads and frees the renderer of the current context.
 *
 */
void _gfx_renderer_unload(

		GFX_CONT_ARG);

/**
 * Allows the renderer to initialize errors for the current context.
 *
 */
void _gfx_renderer_init_errors(

		GFX_CONT_ARG);

/**
 * Allows the renderer to poll for errors of the current context.
 *
 */
void _gfx_renderer_poll_errors(

		GFX_CONT_ARG);

/**
 * Sets the state of the current context.
 *
 */
void _gfx_renderer_states_set(

		const GFXPipeState* state,
		GFX_CONT_ARG);

/**
 * Forces all state fields of the current context.
 *
 * This will reset all state fields, regardless of previous states.
 *
 */
void _gfx_renderer_states_force_set(

		const GFXPipeState* state,
		GFX_CONT_ARG);


#endif // GFX_CORE_RENDERER_H
