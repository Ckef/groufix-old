/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#ifndef GFX_UTILS_H
#define GFX_UTILS_H

#include <stdint.h>

/* Get compiler */
#if defined(__clang__)
	#define GFX_CLANG
#elif defined(__GNUC__)
	#define GFX_GCC
#elif defined(__MINGW32__)
	#define GFX_MINGW
#elif defined(_MSC_VER)
	#define GFX_VISUAL_C

#elif !defined(GFX_COMPILER_ANY)
	#error "Compiler not recognized, define GFX_COMPILER_ANY to use compiler agnostic functionality"
#endif


/********************************************************
 * Compiler specific attributes & helpful macros
 *******************************************************/

/* DLL import/export */
#if defined(GFX_MINGW) || defined(GFX_VISUAL_C)
	#ifdef GFX_BUILD_LIB
		#define GFX_API __declspec(dllexport)
	#else
		#define GFX_API __declspec(dllimport)
	#endif

#else
	#define GFX_API
#endif


/* Always inline */
#if defined(GFX_CLANG) || defined(GFX_GCC) || defined(GFX_MINGW)
	#define GFX_ALWAYS_INLINE inline __attribute__((always_inline))
#elif defined(GFX_VISUAL_C)
	#define GFX_ALWAYS_INLINE __forceinline

#else
	#define GFX_ALWAYS_INLINE inline
#endif


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


#endif // GFX_UTILS_H
