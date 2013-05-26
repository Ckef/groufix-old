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
 * \brief Takes the transpose of a matrix.
 *
 * \param dest Destination matrix.
 *
 */
inline MAT_NAME *MAT_FUNC(transpose)(MAT_NAME *dest, MAT_NAME *a)
{
	MAT_NAME res;
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

#if MAT_SIZE == 2
/**
 * \brief Computes the determinant of a matrix.
 *
 */
inline MAT_TYPE MAT_FUNC(determinant)(MAT_NAME *a)
{
	return a->data[0] * a->data[3] - a->data[2] * a->data[1];
}

/**
 * \brief Takes the inverse of a matrix.
 *
 * \param dest Destination matrix.
 * \return Non-zero if it could take the inverse, otherwise zero.
 *
 */
inline int MAT_FUNC(inverse)(MAT_NAME *dest, MAT_NAME *a)
{
	/* Check if determinant is non-zero */
	double det = MAT_FUNC(determinant)(a);
	if(!det) return 0;

	MAT_NAME res;
	det = 1.0 / det;
	res.data[0] = det * +a->data[3];
	res.data[1] = det * -a->data[1];
	res.data[2] = det * -a->data[2];
	res.data[3] = det * +a->data[0];

	*dest = res;
	return 1;
}

#elif MAT_SIZE == 3
/**
 * \brief Computes the determinant of a matrix.
 *
 */
inline MAT_TYPE MAT_FUNC(determinant)(MAT_NAME *a)
{
	return
		a->data[0] * a->data[4] * a->data[8] +
		a->data[3] * a->data[7] * a->data[2] +
		a->data[6] * a->data[1] * a->data[5] -
		a->data[0] * a->data[7] * a->data[5] -
		a->data[3] * a->data[1] * a->data[8] -
		a->data[6] * a->data[4] * a->data[2];
}

/**
 * \brief Takes the inverse of a matrix.
 *
 * \param dest Destination matrix.
 * \return Non-zero if it could take the inverse, otherwise zero.
 *
 */
inline int MAT_FUNC(inverse)(MAT_NAME *dest, MAT_NAME *a)
{
	/* Compute adjugate matrix */
	MAT_NAME adj;
	adj.data[0] = +(a->data[4] * a->data[8] - a->data[7] * a->data[5]);
	adj.data[1] = -(a->data[1] * a->data[8] - a->data[7] * a->data[2]);
	adj.data[2] = +(a->data[1] * a->data[5] - a->data[4] * a->data[2]);
	adj.data[3] = -(a->data[3] * a->data[8] - a->data[6] * a->data[5]);
	adj.data[4] = +(a->data[0] * a->data[8] - a->data[6] * a->data[2]);
	adj.data[5] = -(a->data[0] * a->data[5] - a->data[3] * a->data[2]);
	adj.data[6] = +(a->data[3] * a->data[7] - a->data[6] * a->data[4]);
	adj.data[7] = -(a->data[0] * a->data[7] - a->data[6] * a->data[1]);
	adj.data[8] = +(a->data[0] * a->data[4] - a->data[3] * a->data[1]);

	/* Check if determinant is non-zero */
	double det =
		a->data[0] * adj.data[0] +
		a->data[3] * adj.data[1] +
		a->data[6] * adj.data[2];

	if(!det) return 0;

	/* Write 1/det * adjugate to destination */
	MAT_FUNC(mult_scalar)(dest, &adj, 1.0 / det);

	return 1;
}

#elif MAT_SIZE == 4
/**
 * \brief Computes the determinant of a matrix.
 *
 */
inline MAT_TYPE MAT_FUNC(determinant)(MAT_NAME *a)
{
	/* Determinants of 2x2 submatrices */
	MAT_TYPE S0 = a->data[0] * a->data[5]  - a->data[4]  * a->data[1];
	MAT_TYPE S1 = a->data[0] * a->data[9]  - a->data[8]  * a->data[1];
	MAT_TYPE S2 = a->data[0] * a->data[13] - a->data[12] * a->data[1];
	MAT_TYPE S3 = a->data[4] * a->data[9]  - a->data[8]  * a->data[5];
	MAT_TYPE S4 = a->data[4] * a->data[13] - a->data[12] * a->data[5];
	MAT_TYPE S5 = a->data[8] * a->data[13] - a->data[12] * a->data[9];

	MAT_TYPE C5 = a->data[10] * a->data[15] - a->data[14] * a->data[11];
	MAT_TYPE C4 = a->data[6]  * a->data[15] - a->data[14] * a->data[7];
	MAT_TYPE C3 = a->data[6]  * a->data[11] - a->data[10] * a->data[7];
	MAT_TYPE C2 = a->data[2]  * a->data[15] - a->data[14] * a->data[3];
	MAT_TYPE C1 = a->data[2]  * a->data[11] - a->data[10] * a->data[3];
	MAT_TYPE C0 = a->data[2]  * a->data[7]  - a->data[6]  * a->data[3];

	return S0 * C5 - S1 * C4 + S2 * C3 + S3 * C2 - S4 * C1 + S5 * C0;
}

/**
 * \brief Takes the inverse of a matrix.
 *
 * \param dest Destination matrix.
 * \return Non-zero if it could take the inverse, otherwise zero.
 *
 */
inline int MAT_FUNC(inverse)(MAT_NAME *dest, MAT_NAME *a)
{
	/* Determinants of 2x2 submatrices */
	MAT_TYPE S0 = a->data[0] * a->data[5]  - a->data[4]  * a->data[1];
	MAT_TYPE S1 = a->data[0] * a->data[9]  - a->data[8]  * a->data[1];
	MAT_TYPE S2 = a->data[0] * a->data[13] - a->data[12] * a->data[1];
	MAT_TYPE S3 = a->data[4] * a->data[9]  - a->data[8]  * a->data[5];
	MAT_TYPE S4 = a->data[4] * a->data[13] - a->data[12] * a->data[5];
	MAT_TYPE S5 = a->data[8] * a->data[13] - a->data[12] * a->data[9];

	MAT_TYPE C5 = a->data[10] * a->data[15] - a->data[14] * a->data[11];
	MAT_TYPE C4 = a->data[6]  * a->data[15] - a->data[14] * a->data[7];
	MAT_TYPE C3 = a->data[6]  * a->data[11] - a->data[10] * a->data[7];
	MAT_TYPE C2 = a->data[2]  * a->data[15] - a->data[14] * a->data[3];
	MAT_TYPE C1 = a->data[2]  * a->data[11] - a->data[10] * a->data[3];
	MAT_TYPE C0 = a->data[2]  * a->data[7]  - a->data[6]  * a->data[3];

	/* Check if determinant is non-zero */
	double det = S0 * C5 - S1 * C4 + S2 * C3 + S3 * C2 - S4 * C1 + S5 * C0;
	if(!det) return 0;

	MAT_NAME res;
	det = 1.0 / det;
	res.data[0]  = det * (a->data[5]  * C5 - a->data[9]  * C4 + a->data[14] * C3);
	res.data[1]  = det * (a->data[9]  * C2 - a->data[1]  * C5 - a->data[13] * C1);
	res.data[2]  = det * (a->data[1]  * C4 - a->data[5]  * C2 + a->data[13] * C0);
	res.data[3]  = det * (a->data[5]  * C1 - a->data[1]  * C3 - a->data[9]  * C0);
	res.data[4]  = det * (a->data[8]  * C4 - a->data[4]  * C5 - a->data[12] * C3);
	res.data[5]  = det * (a->data[0]  * C5 - a->data[8]  * C2 + a->data[12] * C1);
	res.data[6]  = det * (a->data[4]  * C2 - a->data[0]  * C4 - a->data[12] * C0);
	res.data[7]  = det * (a->data[0]  * C3 - a->data[4]  * C1 + a->data[8]  * C0);
	res.data[8]  = det * (a->data[7]  * S5 - a->data[11] * S4 + a->data[15] * S3);
	res.data[9]  = det * (a->data[11] * S2 - a->data[3]  * S5 - a->data[15] * S1);
	res.data[10] = det * (a->data[3]  * S4 - a->data[7]  * S2 + a->data[15] * S0);
	res.data[11] = det * (a->data[7]  * S1 - a->data[3]  * S3 - a->data[11] * S0);
	res.data[12] = det * (a->data[10] * S4 - a->data[6]  * S5 - a->data[14] * S3);
	res.data[13] = det * (a->data[2]  * S5 - a->data[10] * S2 + a->data[14] * S1);
	res.data[14] = det * (a->data[6]  * S2 - a->data[2]  * S4 - a->data[14] * S0);
	res.data[15] = det * (a->data[2]  * S3 - a->data[6]  * S1 + a->data[10] * S0);

	*dest = res;
	return 1;
}
#endif

#ifdef MAT_USE_VEC
/**
 * \brief Returns a column of a matrix as vector.
 *
 */
inline VEC_NAME *MAT_FUNC(get_column)(MAT_NAME *a, size_t column)
{
	return (VEC_NAME*)(a->data + (column * MAT_SIZE));
}

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
