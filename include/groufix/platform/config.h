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

#ifndef GFX_PLATFORM_CONFIG_H
#define GFX_PLATFORM_CONFIG_H

/* Get platform */
#if defined(_WIN32) || defined(__WIN32__)
	#define GFX_WIN32
#elif defined(__unix) || defined(__unix__) || defined(__linux__)
	#define GFX_X11
#else
	#error "Platform not supported"
#endif

/* Platform file */
#if defined(GFX_WIN32)
	#include "groufix/platform/win32.h"
#elif defined(GFX_X11)
	#include "groufix/platform/x11.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initializes the platform server.
 *
 * \return If succesfully initialized, a non-zero value is returned.
 *
 */
int _gfx_platform_init(void);

/**
 * \brief Returns a non-zero value if the platform is initialized correctly.
 *
 */
int _gfx_platform_is_initialized(void);

/**
 * \brief Terminates the platform server.
 *
 */
void _gfx_platform_terminate(void);


#ifdef __cplusplus
}
#endif

#endif // GFX_PLATFORM_CONFIG_H
