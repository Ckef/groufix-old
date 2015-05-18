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
