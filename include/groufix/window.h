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

#include "groufix/events.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Event Callbacks
 *******************************************************/

/* Forward declerate */
struct GFXWindow;

typedef void (*GFXWindowCloseFun)  (struct GFXWindow*);
typedef void (*GFXKeyPressFun)     (struct GFXWindow*, GFXKey, GFXKeyState);
typedef void (*GFXKeyReleaseFun)   (struct GFXWindow*, GFXKey, GFXKeyState);
typedef void (*GFXMouseMoveFun)    (struct GFXWindow*, int, int, GFXKeyState);
typedef void (*GFXMousePressFun)   (struct GFXWindow*, GFXMouseKey, int, int, GFXKeyState);
typedef void (*GFXMouseReleaseFun) (struct GFXWindow*, GFXMouseKey, int, int, GFXKeyState);
typedef void (*GFXMouseWheelFun)   (struct GFXWindow*, int, int, int, int, GFXKeyState);


/********************************************************
 * \brief A Window
 *******************************************************/
typedef struct GFXWindow
{
	/* Platform */
	GFX_Platform_Window handle;

	/* Callbacks */
	struct
	{
		GFXWindowCloseFun   windowClose;
		GFXKeyPressFun      keyPress;
		GFXKeyReleaseFun    keyRelease;
		GFXMouseMoveFun     mouseMove;
		GFXMousePressFun    mousePress;
		GFXMouseReleaseFun  mouseRelease;
		GFXMouseWheelFun    mouseWheel;

	} callbacks;

} GFXWindow;


#ifdef __cplusplus
}
#endif

#endif // GFX_WINDOW_H
