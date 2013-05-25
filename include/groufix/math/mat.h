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

#ifndef GFX_MATH_MAT_H
#define GFX_MATH_MAT_H

#include <math.h>
#include <string.h>
#include "groufix/utils.h"

#define MAT_CREATE_NAME(size,type) NAME(CAT(mat, size), type)

#endif // GFX_MATH_MAT_H


/* Load all default sizes */
#if !defined(MAT_SIZE)

	#define MAT_SIZE 2
	#include "groufix/math/mat.h"
	#undef MAT_SIZE

	#define MAT_SIZE 3
	#include "groufix/math/mat.h"
	#undef MAT_SIZE

	#define MAT_SIZE 4
	#include "groufix/math/mat.h"
	#undef MAT_SIZE


/* Load all default datatypes */
#elif !defined(MAT_TYPE)

	#define MAT_TYPE float
	#include "groufix/math/mat.h"
	#undef MAT_TYPE

	#define MAT_TYPE double
	#include "groufix/math/mat.h"
	#undef MAT_TYPE

#else

/* Name & Function */
#define MAT_NAME MAT_CREATE_NAME(MAT_SIZE, MAT_TYPE)
#define MAT_FUNC(postfix) NAME(MAT_NAME, postfix)
#define MAT_STORE (MAT_SIZE * MAT_SIZE)

/* Vector specific */
#ifdef MAT_USE_VEC
	#ifndef GFX_MATH_VEC_H
		#error "Need to include groufix/math/vec.h to use MAT_USE_VEC"
	#endif
	#define VEC_NAME VEC_CREATE_NAME(MAT_SIZE, MAT_TYPE)
	#define VEC_FUNC(postfix) NAME(VEC_NAME, postfix)
#endif

/** \brief Matrix defintion */
typedef struct
{
	/** Components */
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

#ifdef MAT_USE_VEC
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
#endif // MAT_USE_VEC

#undef MAT_NAME
#undef MAT_FUNC
#undef MAT_STORE

#undef VEC_NAME
#undef VEC_FUNC
#endif // TEMPLATE
