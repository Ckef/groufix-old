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

#ifndef GFX_SCREEN_H
#define GFX_SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * \brief Top level screen
 *******************************************************/
typedef void* GFXScreen;


/**
 * \brief Returns the number of visible screens.
 *
 */
unsigned int gfx_get_num_screens(void);

/**
 * \brief Returns a screen.
 *
 * \param num The number of the screens (num < num_screens).
 *
 */
GFXScreen gfx_get_screen(unsigned int num);

/**
 * \brief Returns the default screen.
 *
 */
GFXScreen gfx_get_default_screen(void);

/**
 * \brief Gets the resolution of a screen in pixels.
 *
 */
void gfx_screen_get_size(GFXScreen screen, unsigned int* width, unsigned int* height);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCREEN_H
