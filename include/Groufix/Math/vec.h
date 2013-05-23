/*****************************************************
 * Groufix  :  Graphics Engine produced by Ckef Worx *
 * www      :  http://www.ejb.ckef-worx.com          *
 *                                                   *
 * Copyright (C) Stef Velzel :: All Rights Reserved  *
 *****************************************************/

#ifndef GFX_MATH_VEC_H
#define GFX_MATH_VEC_H

#include <math.h>
#include "Groufix/utils.h"

// One worded datatypes
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

#endif // GFX_MATH_VEC_H

/////////////////////////////////////////////////
// Load all default sizes
/////////////////////////////////////////////////
#if !defined(VEC_SIZE)

	// Default 2D vector
	#define VEC_SIZE 2
	#include "Groufix/Math/vec.h"
	#undef VEC_SIZE

	// Default 3D vector
	#define VEC_SIZE 3
	#include "Groufix/Math/vec.h"
	#undef VEC_SIZE

	// Default 4D vector
	#define VEC_SIZE 4
	#include "Groufix/Math/vec.h"
	#undef VEC_SIZE

/////////////////////////////////////////////////
// Load all default datatypes
/////////////////////////////////////////////////
#elif !defined(VEC_TYPE)

	#define VEC_TYPE char
	#include "Groufix/Math/vec.h"
	#undef VEC_TYPE

	#define VEC_TYPE uchar
	#include "Groufix/Math/vec.h"
	#undef VEC_TYPE

	#define VEC_TYPE short
	#include "Groufix/Math/vec.h"
	#undef VEC_TYPE

	#define VEC_TYPE ushort
	#include "Groufix/Math/vec.h"
	#undef VEC_TYPE

	#define VEC_TYPE int
	#include "Groufix/Math/vec.h"
	#undef VEC_TYPE

	#define VEC_TYPE uint
	#include "Groufix/Math/vec.h"
	#undef VEC_TYPE

	#define VEC_TYPE float
	#include "Groufix/Math/vec.h"
	#undef VEC_TYPE

	#define VEC_TYPE double
	#include "Groufix/Math/vec.h"
	#undef VEC_TYPE

/////////////////////////////////////////////////
// Vector Template definition
/////////////////////////////////////////////////
#else

	// Name & Function
	#undef VEC_NAME
	#undef VEC_FUNC
	#define VEC_NAME NAME(CAT(vec, VEC_SIZE), VEC_TYPE)
	#define VEC_FUNC(postfix) NAME(VEC_NAME, postfix)

/** \brief Vector defintion */
typedef VEC_TYPE VEC_NAME[VEC_SIZE];

/**
 * \brief Add two vectors.
 *
 * \param dest Destination vector.
 *
 */
inline VEC_TYPE *VEC_FUNC(add)(VEC_TYPE *dest, VEC_TYPE *a, VEC_TYPE *b)
{
	unsigned int i;
	for(i = 0; i < VEC_SIZE; ++i)
		dest[i] = a[i] + b[i];

	return dest;
}

/**
 * \brief Subtract two vectors.
 *
 * \param dest Destination vector.
 *
 */
inline VEC_TYPE *VEC_FUNC(sub)(VEC_TYPE *dest, VEC_TYPE *a, VEC_TYPE *b)
{
	unsigned int i;
	for(i = 0; i < VEC_SIZE; ++i)
		dest[i] = a[i] - b[i];

	return dest;
}

/**
 * \brief Multiplies two vectors component wise.
 *
 * \param dest Destination vector.
 *
 */
inline VEC_TYPE *VEC_FUNC(mult)(VEC_TYPE *dest, VEC_TYPE *a, VEC_TYPE *b)
{
	unsigned int i;
	for(i = 0; i < VEC_SIZE; ++i)
		dest[i] = a[i] * b[i];

	return dest;
}

/**
 * \brief Scales a vector by a scalar.
 *
 * \param dest Destination vector.
 *
 */
inline VEC_TYPE *VEC_FUNC(scale)(VEC_TYPE *dest, VEC_TYPE *a, VEC_TYPE scalar)
{
	unsigned int i;
	for(i = 0; i < VEC_SIZE; ++i)
		dest[i] = a[i] * scalar;

	return dest;
}

/**
 * \brief Take the dot product of two vectors.
 *
 */
inline VEC_TYPE VEC_FUNC(dot)(VEC_TYPE *a, VEC_TYPE *b)
{
	VEC_TYPE dot = 0;
	unsigned int i;
	for(i = 0; i < VEC_SIZE; ++i)
		dot += a[i] * b[i];

	return dot;
}

#if VEC_SIZE == 3
/**
 * \brief Take the cross product of two 3D vectors.
 *
 * \param dest Destination vector.
 *
 */
inline VEC_TYPE *VEC_FUNC(cross)(VEC_TYPE *dest, VEC_TYPE *a, VEC_TYPE *b)
{
	dest[0] = a[1] * b[2] - a[2] * b[1];
	dest[1] = a[2] * b[0] - a[0] * b[2];
	dest[2] = a[0] * b[1] - a[1] * b[0];

	return dest;
}
#endif

/**
 * \brief Compares a vector against 0, component wise.
 *
 * \return If the vector is not zero, a non-zero value is returned.
 *
 */
inline int VEC_FUNC(is_zero)(VEC_TYPE *a)
{
	unsigned int i;
	for(i = 0; i < VEC_SIZE; ++i)
		if(a[i]) return 0;

	return 1;
}

/**
 * \brief Take the squared magnitude of a vector.
 *
 */
inline VEC_TYPE VEC_FUNC(magnitude_squared)(VEC_TYPE *a)
{
	VEC_TYPE dot = 0;
	unsigned int i;
	for(i = 0; i < VEC_SIZE; ++i)
	{
		VEC_TYPE *val = a + i;
		dot += (*val) * (*val);
	}
	return dot;
}

/**
 * \brief Take the magnitude of a vector.
 *
 */
inline double VEC_FUNC(magnitude)(VEC_TYPE *a)
{
	return sqrt((double)VEC_FUNC(magnitude_squared)(a));
}

/**
 * \brief Normalize a vector.
 *
 * \param dest Destination vector.
 *
 */
inline VEC_TYPE *VEC_FUNC(normalize)(VEC_TYPE *dest, VEC_TYPE *a)
{
	double mag = VEC_FUNC(magnitude)(a);
	VEC_TYPE scale = (VEC_TYPE)(mag ? 1.0 / mag : 1.0);

	return VEC_FUNC(scale)(dest, a, scale);
}

#endif
