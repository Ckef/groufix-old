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

#ifndef GFX_UTILS_H
#define GFX_UTILS_H

#include <stdint.h>

/* Unicode */
#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

/* Concatenation */
#define GFX_CAT_BAD(x,y) x ## y
#define GFX_CAT(x,y) GFX_CAT_BAD(x,y)

/* Template naming */
#define GFX_NAME_BAD(x,y) x ## _ ## y
#define GFX_NAME(x,y) GFX_NAME_BAD(x,y)

/* Pointer operations */
#define GFX_PTR_ADD_BYTES(x,y) ((void*)((intptr_t)(x) + (y)))
#define GFX_PTR_SUB_BYTES(x,y) ((void*)((intptr_t)(x) - (y)))
#define GFX_PTR_DIFF(x,y) ((intptr_t)(y) - (intptr_t)(x))

/* Void and uint conversion */
#define GFX_UINT_TO_VOID(x) ((void*)(uintptr_t)(x))
#define GFX_VOID_TO_UINT(x) ((uintptr_t)(x))

#endif // GFX_UTILS_H
