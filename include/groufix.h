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

#ifndef GROUFIX_H
#define GROUFIX_H

/* Core includes */
#include "groufix/core/errors.h"
#include "groufix/core/pipeline.h"
#include "groufix/core/resources.h"


/********************************************************
 * Renderer extensions & limits
 *******************************************************/

/** Renderer Extensions */
/* TODO: tabbed out extensions to be ported to abstract renderer */
typedef enum GFXExtension
{
		GFX_EXT_ANISOTROPIC_FILTER,
	GFX_EXT_GEOMETRY_SHADER,
		GFX_EXT_INSTANCED_BASE_ATTRIBUTES,
		GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE,
		GFX_EXT_MULTISAMPLE_TEXTURE,
	GFX_EXT_POLYGON_STATE,
		GFX_EXT_PROGRAM_BINARY,
		GFX_EXT_PROGRAM_MAP,
	GFX_EXT_SEAMLESS_CUBEMAP,
	GFX_EXT_TESSELLATION_SHADER,
	GFX_EXT_TEXTURE_ARRAY_CUBEMAP,
	GFX_EXT_TEXTURE_ARRAY_MULTISAMPLE,
	GFX_EXT_TEXTURE_BUFFER,
	GFX_EXT_TEXTURE_MULTISAMPLE,
		GFX_EXT_VERTEX_BASE_INDICES,
	GFX_EXT_VERTEX_DOUBLE_PRECISION,
	GFX_EXT_VERTEX_INSTANCING,
	GFX_EXT_VERTEX_PACKED_FLOATS,

	GFX_EXT_COUNT

} GFXExtension;


/** Renderer Limits */
/* TODO: tabbed out limits to be ported to abstract renderer */
typedef enum GFXLimit
{
		GFX_LIM_MAX_ANISOTROPY,
		GFX_LIM_MAX_BUFFER_PROPERTIES,
		GFX_LIM_MAX_COLOR_ATTACHMENTS,
		GFX_LIM_MAX_COLOR_TARGETS,
		GFX_LIM_MAX_CUBEMAP_SIZE,
		GFX_LIM_MAX_FEEDBACK_BUFFERS,
	GFX_LIM_MAX_PATCH_VERTICES,
		GFX_LIM_MAX_SAMPLER_PROPERTIES,
	GFX_LIM_MAX_SAMPLES,
	GFX_LIM_MAX_TEXTURE_1D_SIZE,
	GFX_LIM_MAX_TEXTURE_2D_SIZE,
	GFX_LIM_MAX_TEXTURE_3D_SIZE,
	GFX_LIM_MAX_TEXTURE_BUFFER_SIZE,
	GFX_LIM_MAX_TEXTURE_CUBEMAP_SIZE,
	GFX_LIM_MAX_TEXTURE_ARRAY_SIZE,
	GFX_LIM_MAX_VERTEX_ATTRIBS,
	GFX_LIM_MAX_VERTEX_OFFSET,
	GFX_LIM_MAX_VERTEX_BUFFERS,
	GFX_LIM_MAX_VERTEX_STRIDE,

	GFX_LIM_COUNT

} GFXLimit;


/**
 * Returns whether a given extension is supported or not by the calling groufix thread.
 *
 * @return Non-zero if the extension is supported.
 *
 * Note: this will always return zero if groufix is not initialized.
 *
 */
GFX_API int gfx_is_extension_supported(

		GFXExtension extension);

/**
 * Returns a limit given by the hardware by the calling groufix thread.
 *
 * @return The limit, < 0 if not recognized.
 *
 * Note: this will always return < 0 if groufix is not initialized.
 *
 */
GFX_API long gfx_get_limit(

		GFXLimit limit);


/********************************************************
 * Groufix initialization, timing and polling
 *******************************************************/

/** Renderer context version */
typedef struct GFXContext
{
	int major;
	int minor;

} GFXContext;


/**
 * Initializes the Groufix engine.
 *
 * @param context Minimal context version to use (can be left as zeroes).
 * @param errors  Error mode to use for the engine.
 * @return non-zero if initialization was successful.
 *
 * Note: if groufix is compiled with DEBUG=YES, the error mode will be ignored
 * and assumed to be GFX_ERROR_MODE_DEBUG instead.
 *
 * Calling any function before this one is considered undefined behaviour.
 *
 */
GFX_API int gfx_init(

		GFXContext    context,
		GFXErrorMode  errors);

/**
 * Terminates the Groufix engine.
 *
 * Calling this function while the engine is not initialized is undefined behaviour.
 *
 */
GFX_API void gfx_terminate(void);

/**
 * Polls events of all windows.
 *
 * @return Zero if groufix was requested to terminate.
 *
 */
GFX_API int gfx_poll_events(void);

/**
 * Returns time in seconds since groufix was initialized.
 *
 * @return Time in seconds.
 *
 * Note: calling gfx_get_time across threads can be unreliable,
 * one should not use the same measurement for multiple threads.
 *
 */
GFX_API double gfx_get_time(void);

/**
 * Sets the time in seconds, changing the perceived time of initialization.
 *
 */
GFX_API void gfx_set_time(

		double time);


#endif // GROUFIX_H
