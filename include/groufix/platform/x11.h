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

#ifndef GFX_PLATFORM_X11_H
#define GFX_PLATFORM_X11_H

#include <stdint.h>

/* X11 window conversion */
#define GFX_X11_WND_TO_PTR(x) ((void*)(uintptr_t)x)
#define GFX_X11_PTR_TO_WND(x) ((uintptr_t)x)

/* Maximum key code lookup */
#define GFX_X11_MAX_KEYCODE 0xff
#define GFX_X11_NUM_KEYCODES 0x100

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * \brief X11 Connection
 *******************************************************/
typedef struct GFX_X11_Connection
{
	/* X Display */
	void*          display;        /* (of type Display*) */

	/* Windows */
	unsigned int   numWindows;
	uintptr_t*     windows;        /* (of type Window*) */
	unsigned long  wmDeleteWindow; /* WM_DELETE_WINDOW */

	/* Key table */
	GFXKey         keys[GFX_X11_NUM_KEYCODES];

} GFX_X11_Connection;

/**
 * \brief Server pointer
 */
extern GFX_X11_Connection* _gfx_x11;


#ifdef __cplusplus
}
#endif

#endif // GFX_PLATFORM_X11_H
