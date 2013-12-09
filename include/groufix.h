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
#include "groufix/errors.h"
#include "groufix/math.h"
#include "groufix/memory.h"
#include "groufix/pipeline.h"
#include "groufix/shading.h"
#include "groufix/utils.h"
#include "groufix/window.h"

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
 * @return Whether or not the engine is still active.
 *
 */
int gfx_poll_events(void);

/**
 * Terminates the Groufix engine.
 *
 */
void gfx_terminate(void);


#ifdef __cplusplus
}
#endif

#endif // GROUFIX_H
