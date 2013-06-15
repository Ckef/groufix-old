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

#ifndef GFX_EVENTS_H
#define GFX_EVENTS_H

#include "groufix/keys.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Internal Callbacks
 *******************************************************/

/**
 * \brief Called when a window is requested to close.
 *
 * Used to intercept a user requested window termination.
 *
 */
void _gfx_event_window_close(void* window);

/**
 * \brief Handles a key press event.
 *
 * \param key   What key is pressed.
 * \param state State of some special keys.
 *
 */
void _gfx_event_key_press(void* window, GFXKey key, GFXKeyState state);

/**
 * \brief Handles a key release event.
 *
 * \param key   What key is released.
 * \param state State of some special keys.
 *
 */
void _gfx_event_key_release(void* window, GFXKey key, GFXKeyState state);


#ifdef __cplusplus
}
#endif

#endif // GFX_EVENTS_H
