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

#ifndef GFX_WINDOW_H
#define GFX_WINDOW_H

#include "groufix/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Window Definition */
typedef struct GFXWindow
{
	/* Properties */
	unsigned int width;
	unsigned int height;

	/* Platform specific */
	void *handle;

} GFXWindow;

/**
 * \brief Creates a new window, allocating the memory.
 *
 * \param width  Desired width, must be greater than 0.
 * \param height Desired height, must be greater than 0.
 *
 */
GFXWindow *gfx_create_window(unsigned int width, unsigned int height);

/**
 * \brief Destroys a window, freeing the memory.
 *
 */
void gfx_destroy_window(GFXWindow *window);

#ifdef __cplusplus
}
#endif

#endif // GFX_WINDOW_H
