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

#ifndef GROUFIX_H
#define GROUFIX_H

/* Core includes */
#include "groufix/core/errors.h"
#include "groufix/core/pipeline.h"
#include "groufix/utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the Groufix engine.
 *
 * @return non-zero if initialization was successful.
 *
 */
int gfx_init(void);

/**
 * Polls events of all windows.
 *
 * Note: this must be called on the same thread gfx_init was called on.
 *
 */
void gfx_poll_events(void);

/**
 * Returns time in seconds since groufix was initialized.
 *
 * @return Time in seconds, this might be unreliable across threads depending on the platform.
 *
 * Note: when groufix is not initialized it is relative to some arbitrary point in time.
 *
 */
double gfx_get_time(void);

/**
 * Sets the time in seconds, changing the perceived time of initialization.
 *
 * Note: this might be unreliable across threads depending on the platform.
 *
 */
void gfx_set_time(double time);

/**
 * Terminates the Groufix engine.
 *
 * This has no effect when groufix is not initialized.
 * Note: this must be called on the same thread gfx_init was called on.
 *
 */
void gfx_terminate(void);


#ifdef __cplusplus
}
#endif

#endif // GROUFIX_H
