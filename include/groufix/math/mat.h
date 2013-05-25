/*****************************************************
 * Groufix  :  Graphics Engine produced by Ckef Worx *
 * www      :  http://www.ejb.ckef-worx.com          *
 *                                                   *
 * Copyright (C) Stef Velzel :: All Rights Reserved  *
 *****************************************************/

#ifndef GFX_MATH_MAT_H
#define GFX_MATH_MAT_H

// Include vector if asked
#ifdef MAT_INCLUDE_VEC
	#include "groufix/math/vec.h"
#else

	// Includes
	#include <math.h>
	#include <string.h>
	#include "groufix/utils.h"

#endif

// Create a matrix type name
#define MAT_CREATE_NAME(size,type) NAME(CAT(mat, size), type)

#endif // GFX_MATH_MAT_H


/////////////////////////////////////////////////
// Load all default sizes
/////////////////////////////////////////////////
#if !defined(MAT_SIZE)

	// Default 2x2 matrix
	#define MAT_SIZE 2
	#include "groufix/math/mat.h"
	#undef MAT_SIZE

	// Default 3x3 matrix
	#define MAT_SIZE 3
	#include "groufix/math/mat.h"
	#undef MAT_SIZE

	// Default 4x4 matrix
	#define MAT_SIZE 4
	#include "groufix/math/mat.h"
	#undef MAT_SIZE


/////////////////////////////////////////////////
// Load all default datatypes
/////////////////////////////////////////////////
#elif !defined(MAT_TYPE)

	#define MAT_TYPE float
	#include "groufix/math/mat.h"
	#undef MAT_TYPE

	#define MAT_TYPE double
	#include "groufix/math/mat.h"
	#undef MAT_TYPE


/////////////////////////////////////////////////
// Matrix Template definition
/////////////////////////////////////////////////
#else

// Name & Function
#define MAT_NAME MAT_CREATE_NAME(MAT_SIZE, MAT_TYPE)
#define MAT_FUNC(postfix) NAME(MAT_NAME, postfix)
#define MAT_STORE (MAT_SIZE * MAT_SIZE)

// Vector specific
#ifdef MAT_INCLUDE_VEC
	#define VEC_NAME VEC_CREATE_NAME(MAT_SIZE, MAT_TYPE)
	#define VEC_FUNC(postfix) NAME(VEC_NAME, postfix)
#endif

/** \brief Matrix defintion */
typedef struct
{
	// Components
	MAT_TYPE data[MAT_STORE];
}
MAT_NAME;

/**
 * \brief Returns a value of the matrix.
 *
 */
inline MAT_TYPE *MAT_FUNC(get)(MAT_NAME *a, size_t row, size_t column)
{
	return a->data + (row + MAT_SIZE * column);
}

/**
 * \brief Sets the value of all a matrix components to 0.
 *
 * \return The given matrix itself.
 *
 */
inline MAT_NAME *MAT_FUNC(set_zero)(MAT_NAME *a)
{
	return (MAT_NAME*)memset(a, 0, sizeof(MAT_NAME));
}

/**
 * \brief Add two matrices.
 *
 * \param dest Destination matrix.
 *
 */
inline MAT_NAME *MAT_FUNC(add)(MAT_NAME *dest, MAT_NAME *a, MAT_NAME *b)
{
	size_t i;
	for(i = 0; i < MAT_STORE; ++i)
		dest->data[i] = a->data[i] + b->data[i];

	return dest;
}

/**
 * \brief Subtracts two matrices.
 *
 * \param dest Destination matrix.
 *
 */
inline MAT_NAME *MAT_FUNC(sub)(MAT_NAME *dest, MAT_NAME *a, MAT_NAME *b)
{
	size_t i;
	for(i = 0; i < MAT_STORE; ++i)
		dest->data[i] = a->data[i] - b->data[i];

	return dest;
}

/**
 * \brief Multiplies two matrices.
 *
 * \param dest Destination matrix.
 *
 */
inline MAT_NAME *MAT_FUNC(mult)(MAT_NAME *dest, MAT_NAME *a, MAT_NAME *b)
{
	MAT_NAME res;
	MAT_FUNC(set_zero)(&res);

	size_t r, c, k;
	for(c = 0; c < MAT_STORE; c += MAT_SIZE)
		for(r = 0; r < MAT_SIZE; ++r)
		{
			MAT_TYPE *val = res.data + (r + c);
			for(k = 0; k < MAT_SIZE; ++k)
				*val += a->data[r + k * MAT_SIZE] * b->data[k + c];
		}

	*dest = res;
	return dest;
}

/**
 * \brief Multiplies a matrix by a scalar.
 *
 * \param dest Destination matrix.
 *
 */
inline MAT_NAME *MAT_FUNC(mult_scalar)(MAT_NAME *dest, MAT_NAME *a, MAT_TYPE scalar)
{
	size_t i;
	for(i = 0; i < MAT_STORE; ++i)
		dest->data[i] = a->data[i] * scalar;

	return dest;
}

#ifdef MAT_INCLUDE_VEC
/**
 * \brief Multiplies a matrix by a vector.
 *
 * \param dest Destination vector.
 *
 */
inline VEC_NAME *MAT_FUNC(mult_vec)(VEC_NAME *dest, MAT_NAME *a, VEC_NAME *b)
{
	VEC_NAME res;
	VEC_FUNC(set_zero)(&res);

	size_t r, c;
	for(r = 0; r < MAT_SIZE; ++r)
	{
		MAT_TYPE *val = res.data + r;
		for(c = 0; c < MAT_SIZE; ++c)
			*val += a->data[r + c * MAT_SIZE] * b->data[c];
	}
	*dest = res;
	return dest;
}
#endif

/**
 * \brief Take the transpose of a matrix.
 *
 * \param dest Destination matrix.
 *
 */
inline MAT_NAME *MAT_FUNC(transpose)(MAT_NAME *dest, MAT_NAME *a)
{
	MAT_NAME res;
	MAT_FUNC(set_zero)(&res);

	size_t r, c, c2;
	for(c = 0, c2 = 0; c < MAT_SIZE; ++c, c2 += MAT_SIZE)
		for(r = 0; r < MAT_SIZE; ++r)
			res.data[r + c2] = a->data[c + r * MAT_SIZE];

	*dest = res;
	return dest;
}

/**
 * \brief Compares a matrix against 0, component wise.
 *
 * \return If the matrix is zero, a non-zero value is returned.
 *
 */
inline int MAT_FUNC(is_zero)(MAT_NAME *a)
{
	size_t i;
	for(i = 0; i < MAT_STORE; ++i)
		if(a->data[i]) return 0;

	return 1;
}

#undef MAT_NAME
#undef MAT_FUNC
#undef MAT_STORE

#undef VEC_NAME
#undef VEC_FUNC
#endif // TEMPLATE
