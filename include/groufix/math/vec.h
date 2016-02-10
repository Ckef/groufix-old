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

#ifndef GFX_MATH_VEC_H
#define GFX_MATH_VEC_H

#include "groufix/utils.h"

#include <math.h>
#include <string.h>

#define GFX_VEC_CREATE_NAME(size,type) GFX_NAME(gfx, GFX_CAT(GFX_CAT(type, vec), size))
#define GFX_VEC_CREATE_FUNC(size,type,postfix) GFX_NAME(GFX_VEC_CREATE_NAME(size, type), postfix)

#endif // GFX_MATH_VEC_H


/* Invalid defines */
#if !defined(GFX_VEC_TYPE) && defined(GFX_VEC_DATA)
	#error "Missing define for GFX_VEC_TYPE"

#elif !defined(GFX_VEC_DATA) && defined(GFX_VEC_TYPE)
	#error "Missing define for GFX_VEC_DATA"


/* Load all default sizes */
#elif !defined(GFX_VEC_SIZE)

	#define GFX_VEC_SIZE 2
	#include "groufix/math/vec.h"
	#undef GFX_VEC_SIZE

	#define GFX_VEC_SIZE 3
	#include "groufix/math/vec.h"
	#undef GFX_VEC_SIZE

	#define GFX_VEC_SIZE 4
	#include "groufix/math/vec.h"
	#undef GFX_VEC_SIZE


/* Load all default datatypes */
#elif !defined(GFX_VEC_TYPE)

	#define GFX_VEC_TYPE c
	#define GFX_VEC_DATA int8_t
	#include "groufix/math/vec.h"
	#undef GFX_VEC_DATA
	#undef GFX_VEC_TYPE

	#define GFX_VEC_TYPE uc
	#define GFX_VEC_DATA uint8_t
	#include "groufix/math/vec.h"
	#undef GFX_VEC_DATA
	#undef GFX_VEC_TYPE

	#define GFX_VEC_TYPE s
	#define GFX_VEC_DATA int16_t
	#include "groufix/math/vec.h"
	#undef GFX_VEC_DATA
	#undef GFX_VEC_TYPE

	#define GFX_VEC_TYPE us
	#define GFX_VEC_DATA uint16_t
	#include "groufix/math/vec.h"
	#undef GFX_VEC_DATA
	#undef GFX_VEC_TYPE

	#define GFX_VEC_TYPE i
	#define GFX_VEC_DATA int32_t
	#include "groufix/math/vec.h"
	#undef GFX_VEC_DATA
	#undef GFX_VEC_TYPE

	#define GFX_VEC_TYPE u
	#define GFX_VEC_DATA uint32_t
	#include "groufix/math/vec.h"
	#undef GFX_VEC_DATA
	#undef GFX_VEC_TYPE

	#define GFX_VEC_TYPE
	#define GFX_VEC_DATA float
	#include "groufix/math/vec.h"
	#undef GFX_VEC_DATA
	#undef GFX_VEC_TYPE

	#define GFX_VEC_TYPE d
	#define GFX_VEC_DATA double
	#include "groufix/math/vec.h"
	#undef GFX_VEC_DATA
	#undef GFX_VEC_TYPE

#else


/********************************************************
 * Template definition
 *******************************************************/

/* Name & Function */
#define GFX_VEC_NAME GFX_VEC_CREATE_NAME(GFX_VEC_SIZE, GFX_VEC_TYPE)
#define GFX_VEC_FUNC(postfix) GFX_VEC_CREATE_FUNC(GFX_VEC_SIZE, GFX_VEC_TYPE, postfix)

/* Alignment */
#if GFX_VEC_SIZE == 4 && GFX_VEC_DATA == int32_t
	#define GFX_VEC_ALIGN GFX_SSE_ALIGN
#elif GFX_VEC_SIZE == 4 && GFX_VEC_DATA == uint32_t
	#define GFX_VEC_ALIGN GFX_SSE_ALIGN
#elif GFX_VEC_SIZE == 4 && GFX_VEC_DATA == float
	#define GFX_VEC_ALIGN GFX_SSE_ALIGN
#elif GFX_VEC_SIZE == 2 && GFX_VEC_DATA == double
	#define GFX_VEC_ALIGN GFX_SSE_ALIGN

#else
	#define GFX_VEC_ALIGN GFX_SSE_NO_ALIGN
#endif


#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Vector Template
 *******************************************************/

/** N-dimensional vector */
typedef GFX_VEC_ALIGN
{
	/* Components */
	GFX_VEC_DATA data[GFX_VEC_SIZE];

} GFX_VEC_NAME;


/**
 * Returns a value of the vector.
 *
 */
static GFX_ALWAYS_INLINE GFX_VEC_DATA* GFX_VEC_FUNC(get)(

		GFX_VEC_NAME*  a,
		size_t         component)
{
	return a->data + component;
}

/**
 * Sets the value of all a vector's components to 0.
 *
 * @return The given vector itself.
 *
 */
static GFX_ALWAYS_INLINE GFX_VEC_NAME* GFX_VEC_FUNC(set_zero)(

		GFX_VEC_NAME* a)
{
	return (GFX_VEC_NAME*)memset(a, 0, sizeof(GFX_VEC_NAME));
}

/**
 * Add two vectors.
 *
 * @param dest Destination vector.
 *
 */
static GFX_ALWAYS_INLINE GFX_VEC_NAME* GFX_VEC_FUNC(add)(

		GFX_VEC_NAME*        dest,
		const GFX_VEC_NAME*  a,
		const GFX_VEC_NAME*  b)
{
	size_t i;
	for(i = 0; i < GFX_VEC_SIZE; ++i)
		dest->data[i] = a->data[i] + b->data[i];

	return dest;
}

/**
 * Subtract two vectors.
 *
 * @param dest Destination vector.
 *
 */
static GFX_ALWAYS_INLINE GFX_VEC_NAME* GFX_VEC_FUNC(sub)(

		GFX_VEC_NAME*        dest,
		const GFX_VEC_NAME*  a,
		const GFX_VEC_NAME*  b)
{
	size_t i;
	for(i = 0; i < GFX_VEC_SIZE; ++i)
		dest->data[i] = a->data[i] - b->data[i];

	return dest;
}

/**
 * Multiplies two vectors component wise.
 *
 * @param dest Destination vector.
 *
 */
static GFX_ALWAYS_INLINE GFX_VEC_NAME* GFX_VEC_FUNC(mult)(

		GFX_VEC_NAME*        dest,
		const GFX_VEC_NAME*  a,
		const GFX_VEC_NAME*  b)
{
	size_t i;
	for(i = 0; i < GFX_VEC_SIZE; ++i)
		dest->data[i] = a->data[i] * b->data[i];

	return dest;
}

/**
 * Scales a vector by a scalar.
 *
 * @param dest Destination vector.
 *
 */
static GFX_ALWAYS_INLINE GFX_VEC_NAME* GFX_VEC_FUNC(scale)(

		GFX_VEC_NAME*        dest,
		const GFX_VEC_NAME*  a,
		const GFX_VEC_DATA   scalar)
{
	size_t i;
	for(i = 0; i < GFX_VEC_SIZE; ++i)
		dest->data[i] = a->data[i] * scalar;

	return dest;
}

/**
 * Take the dot product of two vectors.
 *
 */
static GFX_ALWAYS_INLINE GFX_VEC_DATA GFX_VEC_FUNC(dot)(

		const GFX_VEC_NAME*  a,
		const GFX_VEC_NAME*  b)
{
	GFX_VEC_DATA dot = 0;

	size_t i;
	for(i = 0; i < GFX_VEC_SIZE; ++i)
		dot += a->data[i] * b->data[i];

	return dot;
}

#if GFX_VEC_SIZE == 3

/**
 * Take the cross product of two 3D vectors.
 *
 * @param dest Destination vector.
 *
 */
static GFX_ALWAYS_INLINE GFX_VEC_NAME* GFX_VEC_FUNC(cross)(

		GFX_VEC_NAME*        dest,
		const GFX_VEC_NAME*  a,
		const GFX_VEC_NAME*  b)
{
	GFX_VEC_NAME res;
	res.data[0] = a->data[1] * b->data[2] - a->data[2] * b->data[1];
	res.data[1] = a->data[2] * b->data[0] - a->data[0] * b->data[2];
	res.data[2] = a->data[0] * b->data[1] - a->data[1] * b->data[0];

	*dest = res;

	return dest;
}

#endif

/**
 * Take the squared magnitude of a vector.
 *
 */
static GFX_ALWAYS_INLINE GFX_VEC_DATA GFX_VEC_FUNC(magnitude_squared)(

		const GFX_VEC_NAME* a)
{
	return GFX_VEC_FUNC(dot)(a, a);
}

/**
 * Take the magnitude of a vector.
 *
 */
static GFX_ALWAYS_INLINE double GFX_VEC_FUNC(magnitude)(

		const GFX_VEC_NAME* a)
{
	return sqrt((double)GFX_VEC_FUNC(magnitude_squared)(a));
}

/**
 * Normalize a vector.
 *
 * @param dest Destination vector.
 *
 */
static GFX_ALWAYS_INLINE GFX_VEC_NAME* GFX_VEC_FUNC(normalize)(

		GFX_VEC_NAME*        dest,
		const GFX_VEC_NAME*  a)
{
	double mag = GFX_VEC_FUNC(magnitude)(a);
	if(mag) mag = 1.0 / mag;

	size_t i;
	for(i = 0; i < GFX_VEC_SIZE; ++i)
		dest->data[i] = a->data[i] * mag;

	return dest;
}

/**
 * Compares a vector against 0, component wise.
 *
 * @return If the vector is zero, a non-zero value is returned.
 *
 */
static GFX_ALWAYS_INLINE int GFX_VEC_FUNC(is_zero)(

		const GFX_VEC_NAME* a)
{
	size_t i;
	for(i = 0; i < GFX_VEC_SIZE; ++i)
		if(a->data[i]) return 0;

	return 1;
}


#ifdef __cplusplus
}
#endif

#undef GFX_VEC_NAME
#undef GFX_VEC_FUNC
#undef GFX_VEC_ALIGN

#endif // TEMPLATE
