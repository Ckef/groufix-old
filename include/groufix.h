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

#ifndef GROUFIX_H
#define GROUFIX_H

/* Core includes */
#include "groufix/core/errors.h"
#include "groufix/core/pipeline.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Renderer extensions & limits
 *******************************************************/

/** Renderer Extensions */
typedef enum GFXExtension
{
	GFX_EXT_ANISOTROPIC_FILTER,
	GFX_EXT_BUFFER_TEXTURE,
	GFX_EXT_GEOMETRY_SHADER,
	GFX_EXT_INSTANCED_ATTRIBUTES,
	GFX_EXT_INSTANCED_BASE_ATTRIBUTES,
	GFX_EXT_LAYERED_CUBEMAP,
	GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE,
	GFX_EXT_MULTISAMPLE_TEXTURE,
	GFX_EXT_POLYGON_STATE,
	GFX_EXT_PROGRAM_BINARY,
	GFX_EXT_PROGRAM_MAP,
	GFX_EXT_SEAMLESS_CUBEMAP,
	GFX_EXT_TESSELLATION_SHADER,
	GFX_EXT_VERTEX_BASE_INDICES,

	GFX_EXT_COUNT

} GFXExtension;


/** Renderer Limits */
typedef enum GFXLimit
{
	GFX_LIM_MAX_ANISOTROPY,
	GFX_LIM_MAX_BUFFER_PROPERTIES,
	GFX_LIM_MAX_BUFFER_TEXTURE_SIZE,
	GFX_LIM_MAX_COLOR_ATTACHMENTS,
	GFX_LIM_MAX_COLOR_TARGETS,
	GFX_LIM_MAX_CUBEMAP_SIZE,
	GFX_LIM_MAX_FEEDBACK_BUFFERS,
	GFX_LIM_MAX_PATCH_VERTICES,
	GFX_LIM_MAX_SAMPLER_PROPERTIES,
	GFX_LIM_MAX_SAMPLES,
	GFX_LIM_MAX_TEXTURE_3D_SIZE,
	GFX_LIM_MAX_TEXTURE_LAYERS,
	GFX_LIM_MAX_TEXTURE_SIZE,
	GFX_LIM_MAX_VERTEX_ATTRIBS,
	GFX_LIM_MAX_VERTEX_ATTRIB_OFFSET,
	GFX_LIM_MAX_VERTEX_BUFFERS,
	GFX_LIM_MAX_VERTEX_STRIDE,

	GFX_LIM_COUNT

} GFXLimit;


/**
 * Returns whether a given extension is supported or not.
 *
 * @return Non-zero if the extension is supported.
 *
 * Note: this will always return zero if groufix is not initialized.
 *
 */
GFX_API int gfx_is_extension_supported(

		GFXExtension extension);

/**
 * Returns a limit given by the hardware.
 *
 * @return The limit, < 0 if not recognized.
 *
 * Note: this will always return < 0 if groufix is not initialized.
 *
 */
GFX_API int gfx_get_limit(

		GFXLimit limit);


/********************************************************
 * Groufix initialization, timing and polling
 *******************************************************/

/* Renderer Context */
typedef struct GFXContext
{
	int major;
	int minor;

} GFXContext;


/**
 * Initializes the Groufix engine.
 *
 * @param context Minimal context version to use (can be left as zeros).
 * @return non-zero if initialization was successful.
 *
 */
GFX_API int gfx_init(

		GFXContext context);

/**
 * Polls events of all windows.
 *
 * @return Zero if groufix was not yet initialized or already terminated.
 *
 * Note: this must be called on the same thread gfx_init was called on.
 *
 */
GFX_API int gfx_poll_events(void);

/**
 * Returns time in seconds since groufix was initialized.
 *
 * @return Time in seconds, this might be unreliable across threads depending on the platform.
 *
 * Note: when groufix is not initialized it is relative to some arbitrary point in time.
 *
 */
GFX_API double gfx_get_time(void);

/**
 * Sets the time in seconds, changing the perceived time of initialization.
 *
 * Note: this might be unreliable across threads depending on the platform.
 *
 */
GFX_API void gfx_set_time(

		double time);

/**
 * Terminates the Groufix engine.
 *
 * This has no effect when groufix is not initialized.
 * Note: this must be called on the same thread gfx_init was called on.
 *
 */
GFX_API void gfx_terminate(void);


#ifdef __cplusplus
}
#endif

#endif // GROUFIX_H
