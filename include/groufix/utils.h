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

#ifndef GFX_UTILS_H
#define GFX_UTILS_H

#include <stdint.h>


/* SSE alignment */
#ifdef GFX_NO_SSE
	#define GFX_SSE_ALIGN struct
#elif defined(_MSC_VER)
	#define GFX_SSE_ALIGN __declspec(align(16)) struct
#elif defined (__GNUC__)
	#define GFX_SSE_ALIGN struct __attribute__ ((aligned(16)))
#else
	#define GFX_SSE_ALIGN struct
#endif

#define GFX_SSE_NO_ALIGN struct


/* Stringification */
#define GFX_STRIFY_BAD(x) #x
#define GFX_STRIFY(x) GFX_STRIFY_BAD(x)

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

/* Void comparison */
#define GFX_PTR_LESS(x,y) (GFX_VOID_TO_UINT(x) < GFX_VOID_TO_UINT(y))
#define GFX_PTR_LEQUAL(x,y) (GFX_VOID_TO_UINT(x) <= GFX_VOID_TO_UINT(y))
#define GFX_PTR_EQUAL(x,y) (GFX_VOID_TO_UINT(x) == GFX_VOID_TO_UINT(y))
#define GFX_PTR_NEQUAL(x,y) (GFX_VOID_TO_UINT(x) != GFX_VOID_TO_UINT(y))


#endif // GFX_UTILS_H
