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

#ifndef GFX_MATH_VEC_H
#define GFX_MATH_VEC_H

#include "groufix/utils.h"

#include <math.h>
#include <string.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

#define GFX_VEC_CREATE_NAME(size,type) GFX_NAME(GFX_CAT(gfx_vec, size), type)
#define GFX_VEC_CREATE_FUNC(size,type,postfix) GFX_NAME(GFX_VEC_CREATE_NAME(size, type), postfix)

#endif // GFX_MATH_VEC_H


/* Load all default sizes */
#if !defined(GFX_VEC_SIZE)

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

	#define GFX_VEC_TYPE char
	#include "groufix/math/vec.h"
	#undef GFX_VEC_TYPE

	#define GFX_VEC_TYPE uchar
	#include "groufix/math/vec.h"
	#undef GFX_VEC_TYPE

	#define GFX_VEC_TYPE short
	#include "groufix/math/vec.h"
	#undef GFX_VEC_TYPE

	#define GFX_VEC_TYPE ushort
	#include "groufix/math/vec.h"
	#undef GFX_VEC_TYPE

	#define GFX_VEC_TYPE int
	#include "groufix/math/vec.h"
	#undef GFX_VEC_TYPE

	#define GFX_VEC_TYPE uint
	#include "groufix/math/vec.h"
	#undef GFX_VEC_TYPE

	#define GFX_VEC_TYPE float
	#include "groufix/math/vec.h"
	#undef GFX_VEC_TYPE

	#define GFX_VEC_TYPE double
	#include "groufix/math/vec.h"
	#undef GFX_VEC_TYPE

#else


/********************************************************
 * Template definition
 *******************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/* Name & Function */
#define GFX_VEC_NAME GFX_VEC_CREATE_NAME(GFX_VEC_SIZE, GFX_VEC_TYPE)
#define GFX_VEC_FUNC(postfix) GFX_VEC_CREATE_FUNC(GFX_VEC_SIZE, GFX_VEC_TYPE, postfix)


/********************************************************
 * Vector Template
 *******************************************************/
typedef struct
{
	/** Components */
	GFX_VEC_TYPE data[GFX_VEC_SIZE];

} GFX_VEC_NAME;


/**
 * Returns a value of the vector.
 *
 */
inline GFX_VEC_TYPE* GFX_VEC_FUNC(get)(GFX_VEC_NAME* a, size_t component)
{
	return a->data + component;
}

/**
 * Sets the value of all a vector's components to 0.
 *
 * @return The given vector itself.
 *
 */
inline GFX_VEC_NAME* GFX_VEC_FUNC(set_zero)(GFX_VEC_NAME* a)
{
	return (GFX_VEC_NAME*)memset(a, 0, sizeof(GFX_VEC_NAME));
}

/**
 * Add two vectors.
 *
 * @param dest Destination vector.
 *
 */
inline GFX_VEC_NAME* GFX_VEC_FUNC(add)(GFX_VEC_NAME* dest, GFX_VEC_NAME* a, GFX_VEC_NAME* b)
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
inline GFX_VEC_NAME* GFX_VEC_FUNC(sub)(GFX_VEC_NAME* dest, GFX_VEC_NAME* a, GFX_VEC_NAME* b)
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
inline GFX_VEC_NAME* GFX_VEC_FUNC(mult)(GFX_VEC_NAME* dest, GFX_VEC_NAME* a, GFX_VEC_NAME* b)
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
inline GFX_VEC_NAME* GFX_VEC_FUNC(scale)(GFX_VEC_NAME* dest, GFX_VEC_NAME* a, GFX_VEC_TYPE scalar)
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
inline GFX_VEC_TYPE GFX_VEC_FUNC(dot)(GFX_VEC_NAME* a, GFX_VEC_NAME* b)
{
	GFX_VEC_TYPE dot = 0;
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
inline GFX_VEC_NAME* GFX_VEC_FUNC(cross)(GFX_VEC_NAME* dest, GFX_VEC_NAME* a, GFX_VEC_NAME* b)
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
inline GFX_VEC_TYPE GFX_VEC_FUNC(magnitude_squared)(GFX_VEC_NAME* a)
{
	GFX_VEC_TYPE dot = 0;
	size_t i;
	for(i = 0; i < GFX_VEC_SIZE; ++i)
	{
		GFX_VEC_TYPE *val = a->data + i;
		dot += (*val) * (*val);
	}
	return dot;
}

/**
 * Take the magnitude of a vector.
 *
 */
inline double GFX_VEC_FUNC(magnitude)(GFX_VEC_NAME* a)
{
	return sqrt((double)GFX_VEC_FUNC(magnitude_squared)(a));
}

/**
 * Normalize a vector.
 *
 * @param dest Destination vector.
 *
 */
inline GFX_VEC_NAME* GFX_VEC_FUNC(normalize)(GFX_VEC_NAME* dest, GFX_VEC_NAME* a)
{
	double mag = GFX_VEC_FUNC(magnitude)(a);
	GFX_VEC_TYPE scale = (GFX_VEC_TYPE)(mag ? 1.0 / mag : 0.0);

	return GFX_VEC_FUNC(scale)(dest, a, scale);
}

/**
 * Compares a vector against 0, component wise.
 *
 * @return If the vector is zero, a non-zero value is returned.
 *
 */
inline int GFX_VEC_FUNC(is_zero)(GFX_VEC_NAME* a)
{
	size_t i;
	for(i = 0; i < GFX_VEC_SIZE; ++i)
		if(a->data[i]) return 0;

	return 1;
}

#undef GFX_VEC_NAME
#undef GFX_VEC_FUNC

#ifdef __cplusplus
}
#endif

#endif // TEMPLATE
