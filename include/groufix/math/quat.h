/*****************************************************
 * Groufix  :  Graphics Engine produced by Ckef Worx *
 * www      :  http://www.ejb.ckef-worx.com          *
 *                                                   *
 * Copyright (C) Stef Velzel :: All Rights Reserved  *
 *****************************************************/

#ifndef GFX_MATH_QUAT_H
#define GFX_MATH_QUAT_H

// Include matrix or vector if asked
#if defined(QUAT_INCLUDE_MAT)
	#undef MAT_INCLUDE_VEC
	#define MAT_INCLUDE_VEC
	#undef QUAT_INCLUDE_VEC
	#define QUAT_INCLUDE_VEC
	#include "groufix/math/mat.h"
#elif defined(QUAT_INCLUDE_VEC)
	#include "groufix/math/vec.h"
#else

	// Includes
	#include <math.h>
	#include <string.h>
	#include "groufix/utils.h"

#endif

// Create a quaternion type name
#define QUAT_CREATE_NAME(type) NAME(quat, type)

#endif // GFX_MATH_QUAT_H


/////////////////////////////////////////////////
// Load all default datatypes
/////////////////////////////////////////////////
#if !defined(QUAT_TYPE)

	#define QUAT_TYPE float
	#include "groufix/math/quat.h"
	#undef QUAT_TYPE

	#define QUAT_TYPE double
	#include "groufix/math/quat.h"
	#undef QUAT_TYPE


/////////////////////////////////////////////////
// Quaternion Template definition
/////////////////////////////////////////////////
#else

// Name & Function
#define QUAT_NAME QUAT_CREATE_NAME(QUAT_TYPE)
#define QUAT_FUNC(postfix) NAME(QUAT_NAME, postfix)

// Matrix specific
#ifdef QUAT_INCLUDE_MAT
	#define MAT_NAME MAT_CREATE_NAME(3, QUAT_TYPE)
	#define MAT_FUNC(postfix) NAME(MAT_NAME, postfix)
	#define MAT_STORE 9
#endif

// Vector specific
#ifdef QUAT_INCLUDE_VEC
	#define VEC_NAME VEC_CREATE_NAME(3, QUAT_TYPE)
	#define VEC_FUNC(postfix) NAME(VEC_NAME, postfix)
#endif

/** \brief Quaternion defintion */
typedef struct
{
	// Components
	QUAT_TYPE data[4];
}
QUAT_NAME;

/**
 * \brief Sets the value of all a quaternion's components to 0.
 *
 * \return The given quaternion itself.
 *
 */
inline QUAT_NAME *QUAT_FUNC(set_zero)(QUAT_NAME *a)
{
	return (QUAT_NAME*)memset(a, 0, sizeof(QUAT_NAME));
}

/**
 * \brief Add two quaternions.
 *
 * \param dest Destination quaternion.
 *
 */
inline QUAT_NAME *QUAT_FUNC(add)(QUAT_NAME *dest, QUAT_NAME *a, QUAT_NAME *b)
{
	size_t i;
	for(i = 0; i < 4; ++i)
		dest->data[i] = a->data[i] + b->data[i];

	return dest;
}

/**
 * \brief Subtract two quaternions.
 *
 * \param dest Destination quaternion.
 *
 */
inline QUAT_NAME *QUAT_FUNC(sub)(QUAT_NAME *dest, QUAT_NAME *a, QUAT_NAME *b)
{
	size_t i;
	for(i = 0; i < 4; ++i)
		dest->data[i] = a->data[i] - b->data[i];

	return dest;
}

/**
 * \brief Multiplies two quaternions.
 *
 * \param dest Destination quaternion.
 *
 */
inline QUAT_NAME *QUAT_FUNC(mult)(QUAT_NAME *dest, QUAT_NAME *a, QUAT_NAME *b)
{
	QUAT_NAME res;
	res.data[0] = a->data[0] * b->data[0] - a->data[1] * b->data[1] - a->data[2] * b->data[2] - a->data[3] * b->data[3];
	res.data[1] = a->data[0] * b->data[1] + a->data[1] * b->data[0] + a->data[2] * b->data[3] - a->data[3] * b->data[2];
	res.data[2] = a->data[0] * b->data[2] - a->data[1] * b->data[3] + a->data[2] * b->data[0] + a->data[3] * b->data[1];
	res.data[3] = a->data[0] * b->data[3] + a->data[1] * b->data[2] - a->data[2] * b->data[1] + a->data[3] * b->data[0];

	*dest = res;
	return dest;
}

/**
 * \brief Scales a quaternion by a scalar.
 *
 * \param dest Destination quaternion.
 *
 */
inline QUAT_NAME *QUAT_FUNC(scale)(QUAT_NAME *dest, QUAT_NAME *a, QUAT_TYPE scalar)
{
	size_t i;
	for(i = 0; i < 4; ++i)
		dest->data[i] = a->data[i] * scalar;

	return dest;
}

/**
 * \brief Takes the conjugate of a quaternion.
 *
 * \param dest Destination quaternion.
 *
 */
inline QUAT_NAME *QUAT_FUNC(conjugate)(QUAT_NAME *dest, QUAT_NAME *a)
{
	dest->data[0] = a->data[0];
	dest->data[1] = -a->data[1];
	dest->data[2] = -a->data[1];
	dest->data[3] = -a->data[3];

	return dest;
}

/**
 * \brief Take the squared norm of a quaternion.
 *
 */
inline QUAT_TYPE QUAT_FUNC(norm_squared)(QUAT_NAME *a)
{
	QUAT_TYPE norm = 0;
	size_t i;
	for(i = 0; i < 4; ++i)
	{
		QUAT_TYPE *val = a->data + i;
		norm += (*val) * (*val);
	}
	return norm;
}

/**
 * \brief Take the norm of a quaternion.
 *
 */
inline double QUAT_FUNC(norm)(QUAT_NAME *a)
{
	return sqrt((double)QUAT_FUNC(norm_squared)(a));
}

/**
 * \brief Normalize a quaternion.
 *
 * \param dest Destination quaternion.
 *
 */
inline QUAT_NAME *QUAT_FUNC(normalize)(QUAT_NAME *dest, QUAT_NAME *a)
{
	double norm = QUAT_FUNC(norm)(a);
	QUAT_TYPE scale = (QUAT_TYPE)(norm ? 1.0 / norm : 0.0);

	return QUAT_FUNC(scale)(dest, a, scale);
}

/**
 * \brief Takes the inverse of a quaternion.
 *
 * \param dest Destination quaternion.
 *
 */
inline QUAT_NAME *QUAT_FUNC(inverse)(QUAT_NAME *dest, QUAT_NAME *a)
{
	double normSq = QUAT_FUNC(norm_squared)(a);
	QUAT_TYPE scale = (QUAT_TYPE)(normSq ? 1.0f / normSq : 0.0);

	QUAT_FUNC(conjugate)(dest, a);
	return QUAT_FUNC(scale)(dest, dest, scale);
}

/**
 * \brief Compares a quaternion against 0, component wise.
 *
 * \return If the quaternion is zero, a non-zero value is returned.
 *
 */
inline int QUAT_FUNC(is_zero)(QUAT_NAME *a)
{
	size_t i;
	for(i = 0; i < 4; ++i)
		if(a->data[i]) return 0;

	return 1;
}

#undef QUAT_NAME
#undef QUAT_FUNC

#undef MAT_NAME
#undef MAT_FUNC
#undef MAT_STORE

#undef VEC_NAME
#undef VEC_FUNC
#endif // TEMPLATE
