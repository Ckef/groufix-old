/*****************************************************
 * Groufix  :  Graphics Engine produced by Ckef Worx *
 * www      :  http://www.ejb.ckef-worx.com          *
 *                                                   *
 * Copyright (C) Stef Velzel :: All Rights Reserved  *
 *****************************************************/

#ifndef GFX_MATH_VEC_H
#define GFX_MATH_VEC_H

// Includes
#include <math.h>
#include <string.h>
#include "groufix/utils.h"

// One worded datatypes
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

// Create a vector type name
#define VEC_CREATE_NAME(size,type) NAME(CAT(vec, size), type)

#endif // GFX_MATH_VEC_H


/////////////////////////////////////////////////
// Load all default sizes
/////////////////////////////////////////////////
#if !defined(VEC_SIZE)

	// Default 2D vector
	#define VEC_SIZE 2
	#include "groufix/math/vec.h"
	#undef VEC_SIZE

	// Default 3D vector
	#define VEC_SIZE 3
	#include "groufix/math/vec.h"
	#undef VEC_SIZE

	// Default 4D vector
	#define VEC_SIZE 4
	#include "groufix/math/vec.h"
	#undef VEC_SIZE


/////////////////////////////////////////////////
// Load all default datatypes
/////////////////////////////////////////////////
#elif !defined(VEC_TYPE)

	#define VEC_TYPE char
	#include "groufix/math/vec.h"
	#undef VEC_TYPE

	#define VEC_TYPE uchar
	#include "groufix/math/vec.h"
	#undef VEC_TYPE

	#define VEC_TYPE short
	#include "groufix/math/vec.h"
	#undef VEC_TYPE

	#define VEC_TYPE ushort
	#include "groufix/math/vec.h"
	#undef VEC_TYPE

	#define VEC_TYPE int
	#include "groufix/math/vec.h"
	#undef VEC_TYPE

	#define VEC_TYPE uint
	#include "groufix/math/vec.h"
	#undef VEC_TYPE

	#define VEC_TYPE float
	#include "groufix/math/vec.h"
	#undef VEC_TYPE

	#define VEC_TYPE double
	#include "groufix/math/vec.h"
	#undef VEC_TYPE


/////////////////////////////////////////////////
// Vector Template definition
/////////////////////////////////////////////////
#else

// Name & Function
#define VEC_NAME VEC_CREATE_NAME(VEC_SIZE, VEC_TYPE)
#define VEC_FUNC(postfix) NAME(VEC_NAME, postfix)

/** \brief Vector defintion */
typedef struct
{
	// Components
	VEC_TYPE data[VEC_SIZE];
}
VEC_NAME;

/**
 * \brief Sets the value of all a vector's components to 0.
 *
 * \return The given vector itself.
 *
 */
inline VEC_NAME *VEC_FUNC(set_zero)(VEC_NAME *a)
{
	return (VEC_NAME*)memset(a, 0, sizeof(VEC_NAME));
}

/**
 * \brief Add two vectors.
 *
 * \param dest Destination vector.
 *
 */
inline VEC_NAME *VEC_FUNC(add)(VEC_NAME *dest, VEC_NAME *a, VEC_NAME *b)
{
	size_t i;
	for(i = 0; i < VEC_SIZE; ++i)
		dest->data[i] = a->data[i] + b->data[i];

	return dest;
}

/**
 * \brief Subtract two vectors.
 *
 * \param dest Destination vector.
 *
 */
inline VEC_NAME *VEC_FUNC(sub)(VEC_NAME *dest, VEC_NAME *a, VEC_NAME *b)
{
	size_t i;
	for(i = 0; i < VEC_SIZE; ++i)
		dest->data[i] = a->data[i] - b->data[i];

	return dest;
}

/**
 * \brief Multiplies two vectors component wise.
 *
 * \param dest Destination vector.
 *
 */
inline VEC_NAME *VEC_FUNC(mult)(VEC_NAME *dest, VEC_NAME *a, VEC_NAME *b)
{
	size_t i;
	for(i = 0; i < VEC_SIZE; ++i)
		dest->data[i] = a->data[i] * b->data[i];

	return dest;
}

/**
 * \brief Scales a vector by a scalar.
 *
 * \param dest Destination vector.
 *
 */
inline VEC_NAME *VEC_FUNC(scale)(VEC_NAME *dest, VEC_NAME *a, VEC_TYPE scalar)
{
	size_t i;
	for(i = 0; i < VEC_SIZE; ++i)
		dest->data[i] = a->data[i] * scalar;

	return dest;
}

/**
 * \brief Take the dot product of two vectors.
 *
 */
inline VEC_TYPE VEC_FUNC(dot)(VEC_NAME *a, VEC_NAME *b)
{
	VEC_TYPE dot = 0;
	size_t i;
	for(i = 0; i < VEC_SIZE; ++i)
		dot += a->data[i] * b->data[i];

	return dot;
}

#if VEC_SIZE == 3
/**
 * \brief Take the cross product of two 3D vectors.
 *
 * \param dest Destination vector.
 *
 */
inline VEC_NAME *VEC_FUNC(cross)(VEC_NAME *dest, VEC_NAME *a, VEC_NAME *b)
{
	VEC_NAME res;
	res.data[0] = a->data[1] * b->data[2] - a->data[2] * b->data[1];
	res.data[1] = a->data[2] * b->data[0] - a->data[0] * b->data[2];
	res.data[2] = a->data[0] * b->data[1] - a->data[1] * b->data[0];

	*dest = res;
	return dest;
}
#endif

/**
 * \brief Take the squared magnitude of a vector.
 *
 */
inline VEC_TYPE VEC_FUNC(magnitude_squared)(VEC_NAME *a)
{
	VEC_TYPE dot = 0;
	size_t i;
	for(i = 0; i < VEC_SIZE; ++i)
	{
		VEC_TYPE *val = a->data + i;
		dot += (*val) * (*val);
	}
	return dot;
}

/**
 * \brief Take the magnitude of a vector.
 *
 */
inline double VEC_FUNC(magnitude)(VEC_NAME *a)
{
	return sqrt((double)VEC_FUNC(magnitude_squared)(a));
}

/**
 * \brief Normalize a vector.
 *
 * \param dest Destination vector.
 *
 */
inline VEC_NAME *VEC_FUNC(normalize)(VEC_NAME *dest, VEC_NAME *a)
{
	double mag = VEC_FUNC(magnitude)(a);
	VEC_TYPE scale = (VEC_TYPE)(mag ? 1.0 / mag : 0.0);

	return VEC_FUNC(scale)(dest, a, scale);
}

/**
 * \brief Compares a vector against 0, component wise.
 *
 * \return If the vector is zero, a non-zero value is returned.
 *
 */
inline int VEC_FUNC(is_zero)(VEC_NAME *a)
{
	size_t i;
	for(i = 0; i < VEC_SIZE; ++i)
		if(a->data[i]) return 0;

	return 1;
}

#undef VEC_NAME
#undef VEC_FUNC
#endif // TEMPLATE
