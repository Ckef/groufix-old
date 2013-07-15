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

/********************************************************
 * Platform specifics
 *******************************************************/

/* Get build target */
#if defined(_WIN32) || defined(__WIN32__)
	#define GFX_WIN32
#elif defined(__APPLE__) || defined(__MACH__)
	#define GFX_OSX
#elif defined(__unix) || defined(__unix__) || defined(__linux__)
	#define GFX_UNIX

/* Maybe GLES? */
#elif !defined(GFX_GLES)
	#error "Platform not supported"
#endif

/* Windows */
#ifdef GFX_WIN32

	/* Unicode */
	#ifndef UNICODE
	#define UNICODE
	#endif

	#ifndef _UNICODE
	#define _UNICODE
	#endif

	/* Windows XP */
	#ifndef WINVER
	#define WINVER 0x0501
	#endif

	/* Nothing extra */
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef VC_EXTRALEAN
	#define VC_EXTRALEAN
	#endif
	
	/* To avoid redifinitions */
	#include <windows.h>

#endif


/********************************************************
 * Other unrelated platform stuff
 *******************************************************/

/** \brief Proc Address */
typedef void (*GFXProcAddress)(void);


/* Concatenation */
#define CAT_BAD(x,y) x ## y
#define CAT(x,y) CAT_BAD(x,y)

/* Template naming */
#define NAME_BAD(x,y) x ## _ ## y
#define NAME(x,y) NAME_BAD(x,y)

/* Pointer operations */
#define PTR_ADD_BYTES(x,y) ((void*)((intptr_t)(x) + (y)))
#define PTR_SUB_BYTES(x,y) ((void*)((intptr_t)(x) - (y)))
#define PTR_DIFF(x,y) ((intptr_t)(y) - (intptr_t)(x))

/* Void and uint conversion */
#define UINT_TO_VOID(x) ((void*)(uintptr_t)(x))
#define VOID_TO_UINT(x) ((uintptr_t)(x))


#endif // GFX_UTILS_H
