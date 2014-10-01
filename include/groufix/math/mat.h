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

#ifndef GFX_MATH_MAT_H
#define GFX_MATH_MAT_H

#include "groufix/utils.h"

#include <float.h>
#include <math.h>
#include <string.h>

#define GFX_MAT_CREATE_NAME(size,type) GFX_NAME(gfx, GFX_CAT(GFX_CAT(type, mat), size))
#define GFX_MAT_CREATE_FUNC(size,type,postfix) GFX_NAME(GFX_MAT_CREATE_NAME(size, type), postfix)

#endif // GFX_MATH_MAT_H


/* Invalid defines */
#if !defined(GFX_MAT_TYPE) && defined(GFX_MAT_DATA)
	#error "Missing define for GFX_MAT_TYPE"

#elif !defined(GFX_MAT_DATA) && defined(GFX_MAT_TYPE)
	#error "Missing define for GFX_MAT_DATA"


/* Load all default sizes */
#elif !defined(GFX_MAT_SIZE)

	#define GFX_MAT_SIZE 2
	#include "groufix/math/mat.h"
	#undef GFX_MAT_SIZE

	#define GFX_MAT_SIZE 3
	#include "groufix/math/mat.h"
	#undef GFX_MAT_SIZE

	#define GFX_MAT_SIZE 4
	#include "groufix/math/mat.h"
	#undef GFX_MAT_SIZE


/* Load all default datatypes */
#elif !defined(GFX_MAT_TYPE)

	#define GFX_MAT_TYPE
	#define GFX_MAT_DATA float
	#include "groufix/math/mat.h"
	#undef GFX_MAT_DATA
	#undef GFX_MAT_TYPE

	#define GFX_MAT_TYPE d
	#define GFX_MAT_DATA double
	#include "groufix/math/mat.h"
	#undef GFX_MAT_DATA
	#undef GFX_MAT_TYPE

#else


/********************************************************
 * Template definition
 *******************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/* Name & Function */
#define GFX_MAT_NAME GFX_MAT_CREATE_NAME(GFX_MAT_SIZE, GFX_MAT_TYPE)
#define GFX_MAT_FUNC(postfix) GFX_MAT_CREATE_FUNC(GFX_MAT_SIZE, GFX_MAT_TYPE, postfix)
#define GFX_MAT_STORE (GFX_MAT_SIZE * GFX_MAT_SIZE)

/* Alignment */
#if GFX_MAT_SIZE == 4 && GFX_MAT_DATA == float
	#define GFX_MAT_ALIGN GFX_SSE_ALIGN

#elif GFX_MAT_SIZE == 2 && GFX_MAT_DATA == double
	#define GFX_MAT_ALIGN GFX_SSE_ALIGN

#else
	#define GFX_MAT_ALIGN GFX_SSE_NO_ALIGN
#endif

/* Vector specific */
#ifdef GFX_MAT_USE_VEC
	#ifndef GFX_MATH_VEC_H
		#error "Need to include groufix/math/vec.h to use GFX_MAT_USE_VEC"
	#endif
	#define GFX_VEC_NAME GFX_VEC_CREATE_NAME(GFX_MAT_SIZE, GFX_MAT_TYPE)
	#define GFX_VEC_FUNC(postfix) GFX_VEC_CREATE_FUNC(GFX_MAT_SIZE, GFX_MAT_TYPE, postfix)
#endif


/********************************************************
 * Matrix Template
 *******************************************************/
typedef GFX_MAT_ALIGN
{
	/** Components */
	GFX_MAT_DATA data[GFX_MAT_STORE];

} GFX_MAT_NAME;


/**
 * Returns a value of the matrix.
 *
 */
inline GFX_MAT_DATA* GFX_MAT_FUNC(get)(

		GFX_MAT_NAME*  a,
		size_t         row,
		size_t         column)
{
	return a->data + (row + GFX_MAT_SIZE * column);
}

/**
 * Sets the value of all a matrix components to 0.
 *
 * @return The given matrix itself.
 *
 */
inline GFX_MAT_NAME* GFX_MAT_FUNC(set_zero)(

		GFX_MAT_NAME* a)
{
	return (GFX_MAT_NAME*)memset(a, 0, sizeof(GFX_MAT_NAME));
}

/**
 * Add two matrices.
 *
 * @param dest Destination matrix.
 *
 */
inline GFX_MAT_NAME* GFX_MAT_FUNC(add)(

		GFX_MAT_NAME*  dest,
		GFX_MAT_NAME*  a,
		GFX_MAT_NAME*  b)
{
	size_t i;
	for(i = 0; i < GFX_MAT_STORE; ++i)
		dest->data[i] = a->data[i] + b->data[i];

	return dest;
}

/**
 * Subtracts two matrices.
 *
 * @param dest Destination matrix.
 *
 */
inline GFX_MAT_NAME* GFX_MAT_FUNC(sub)(

		GFX_MAT_NAME*  dest,
		GFX_MAT_NAME*  a,
		GFX_MAT_NAME*  b)
{
	size_t i;
	for(i = 0; i < GFX_MAT_STORE; ++i)
		dest->data[i] = a->data[i] - b->data[i];

	return dest;
}

/**
 * Multiplies two matrices.
 *
 * @param dest Destination matrix.
 *
 */
inline GFX_MAT_NAME* GFX_MAT_FUNC(mult)(

		GFX_MAT_NAME*  dest,
		GFX_MAT_NAME*  a,
		GFX_MAT_NAME*  b)
{
	GFX_MAT_NAME res;
	GFX_MAT_FUNC(set_zero)(&res);

	size_t r, c, k;
	for(c = 0; c < GFX_MAT_STORE; c += GFX_MAT_SIZE)
		for(r = 0; r < GFX_MAT_SIZE; ++r)
		{
			GFX_MAT_DATA* val = res.data + (r + c);
			for(k = 0; k < GFX_MAT_SIZE; ++k)
				*val += a->data[r + k * GFX_MAT_SIZE] * b->data[k + c];
		}

	*dest = res;
	return dest;
}

/**
 * Multiplies a matrix by a scalar.
 *
 * @param dest Destination matrix.
 *
 */
inline GFX_MAT_NAME* GFX_MAT_FUNC(mult_scalar)(

		GFX_MAT_NAME*  dest,
		GFX_MAT_NAME*  a,
		GFX_MAT_DATA   scalar)
{
	size_t i;
	for(i = 0; i < GFX_MAT_STORE; ++i)
		dest->data[i] = a->data[i] * scalar;

	return dest;
}

/**
 * Takes the transpose of a matrix.
 *
 * @param dest Destination matrix.
 *
 */
inline GFX_MAT_NAME* GFX_MAT_FUNC(transpose)(

		GFX_MAT_NAME*  dest,
		GFX_MAT_NAME*  a)
{
	GFX_MAT_NAME res;

	size_t r, c, c2;
	for(c = 0, c2 = 0; c < GFX_MAT_SIZE; ++c, c2 += GFX_MAT_SIZE)
		for(r = 0; r < GFX_MAT_SIZE; ++r)
			res.data[r + c2] = a->data[c + r * GFX_MAT_SIZE];

	*dest = res;
	return dest;
}

/**
 * Compares a matrix against 0, component wise.
 *
 * @return If the matrix is zero, a non-zero value is returned.
 *
 */
inline int GFX_MAT_FUNC(is_zero)(

		GFX_MAT_NAME* a)
{
	size_t i;
	for(i = 0; i < GFX_MAT_STORE; ++i)
		if(a->data[i]) return 0;

	return 1;
}

#if GFX_MAT_SIZE == 2
/**
 * Computes the determinant of a matrix.
 *
 */
inline GFX_MAT_DATA GFX_MAT_FUNC(determinant)(

		GFX_MAT_NAME* a)
{
	return a->data[0] * a->data[3] - a->data[2] * a->data[1];
}

/**
 * Takes the inverse of a matrix.
 *
 * @param dest Destination matrix.
 * @return Non-zero if it could take the inverse, otherwise zero.
 *
 */
inline int GFX_MAT_FUNC(inverse)(

		GFX_MAT_NAME*  dest,
		GFX_MAT_NAME*  a)
{
	/* Check if determinant is non-zero */
	double det = GFX_MAT_FUNC(determinant)(a);
	if(fabs(det) <= DBL_EPSILON) return 0;

	GFX_MAT_NAME res;
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
 * Computes the determinant of a matrix.
 *
 */
inline GFX_MAT_DATA GFX_MAT_FUNC(determinant)(

		GFX_MAT_NAME* a)
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
 * Takes the inverse of a matrix.
 *
 * @param dest Destination matrix.
 * @return Non-zero if it could take the inverse, otherwise zero.
 *
 */
inline int GFX_MAT_FUNC(inverse)(

		GFX_MAT_NAME*  dest,
		GFX_MAT_NAME*  a)
{
	/* Compute adjugate matrix */
	GFX_MAT_NAME adj;
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

	if(fabs(det) <= DBL_EPSILON) return 0;

	/* Write 1/det * adjugate to destination */
	GFX_MAT_FUNC(mult_scalar)(dest, &adj, 1.0 / det);

	return 1;
}

#elif GFX_MAT_SIZE == 4
/**
 * Computes the determinant of a matrix.
 *
 */
inline GFX_MAT_DATA GFX_MAT_FUNC(determinant)(

		GFX_MAT_NAME* a)
{
	/* Determinants of 2x2 submatrices */
	GFX_MAT_DATA S0 = a->data[0] * a->data[5]  - a->data[4]  * a->data[1];
	GFX_MAT_DATA S1 = a->data[0] * a->data[9]  - a->data[8]  * a->data[1];
	GFX_MAT_DATA S2 = a->data[0] * a->data[13] - a->data[12] * a->data[1];
	GFX_MAT_DATA S3 = a->data[4] * a->data[9]  - a->data[8]  * a->data[5];
	GFX_MAT_DATA S4 = a->data[4] * a->data[13] - a->data[12] * a->data[5];
	GFX_MAT_DATA S5 = a->data[8] * a->data[13] - a->data[12] * a->data[9];

	GFX_MAT_DATA C5 = a->data[10] * a->data[15] - a->data[14] * a->data[11];
	GFX_MAT_DATA C4 = a->data[6]  * a->data[15] - a->data[14] * a->data[7];
	GFX_MAT_DATA C3 = a->data[6]  * a->data[11] - a->data[10] * a->data[7];
	GFX_MAT_DATA C2 = a->data[2]  * a->data[15] - a->data[14] * a->data[3];
	GFX_MAT_DATA C1 = a->data[2]  * a->data[11] - a->data[10] * a->data[3];
	GFX_MAT_DATA C0 = a->data[2]  * a->data[7]  - a->data[6]  * a->data[3];

	return S0 * C5 - S1 * C4 + S2 * C3 + S3 * C2 - S4 * C1 + S5 * C0;
}

/**
 * Takes the inverse of a matrix.
 *
 * @param dest Destination matrix.
 * @return Non-zero if it could take the inverse, otherwise zero.
 *
 */
inline int GFX_MAT_FUNC(inverse)(

		GFX_MAT_NAME*  dest,
		GFX_MAT_NAME*  a)
{
	/* Determinants of 2x2 submatrices */
	GFX_MAT_DATA S0 = a->data[0] * a->data[5]  - a->data[4]  * a->data[1];
	GFX_MAT_DATA S1 = a->data[0] * a->data[9]  - a->data[8]  * a->data[1];
	GFX_MAT_DATA S2 = a->data[0] * a->data[13] - a->data[12] * a->data[1];
	GFX_MAT_DATA S3 = a->data[4] * a->data[9]  - a->data[8]  * a->data[5];
	GFX_MAT_DATA S4 = a->data[4] * a->data[13] - a->data[12] * a->data[5];
	GFX_MAT_DATA S5 = a->data[8] * a->data[13] - a->data[12] * a->data[9];

	GFX_MAT_DATA C5 = a->data[10] * a->data[15] - a->data[14] * a->data[11];
	GFX_MAT_DATA C4 = a->data[6]  * a->data[15] - a->data[14] * a->data[7];
	GFX_MAT_DATA C3 = a->data[6]  * a->data[11] - a->data[10] * a->data[7];
	GFX_MAT_DATA C2 = a->data[2]  * a->data[15] - a->data[14] * a->data[3];
	GFX_MAT_DATA C1 = a->data[2]  * a->data[11] - a->data[10] * a->data[3];
	GFX_MAT_DATA C0 = a->data[2]  * a->data[7]  - a->data[6]  * a->data[3];

	/* Check if determinant is non-zero */
	double det = S0 * C5 - S1 * C4 + S2 * C3 + S3 * C2 - S4 * C1 + S5 * C0;
	if(fabs(det) <= DBL_EPSILON) return 0;

	GFX_MAT_NAME res;
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

#ifdef GFX_MAT_USE_VEC
/**
 * Returns a column of a matrix as vector.
 *
 */
inline GFX_VEC_NAME* GFX_MAT_FUNC(get_column)(

		GFX_MAT_NAME*  a,
		size_t         column)
{
	return (GFX_VEC_NAME*)(a->data + (column * GFX_MAT_SIZE));
}

/**
 * Multiplies a matrix by a vector.
 *
 * @param dest Destination vector.
 *
 */
inline GFX_VEC_NAME* GFX_MAT_FUNC(mult_vec)(

		GFX_VEC_NAME*  dest,
		GFX_MAT_NAME*  a,
		GFX_VEC_NAME*  b)
{
	GFX_VEC_NAME res;
	GFX_VEC_FUNC(set_zero)(&res);

	size_t r, c;
	for(r = 0; r < GFX_MAT_SIZE; ++r)
	{
		GFX_MAT_DATA* val = res.data + r;
		for(c = 0; c < GFX_MAT_SIZE; ++c)
			*val += a->data[r + c * GFX_MAT_SIZE] * b->data[c];
	}
	*dest = res;

	return dest;
}
#endif // GFX_MAT_USE_VEC

#undef GFX_MAT_NAME
#undef GFX_MAT_FUNC
#undef GFX_MAT_STORE
#undef GFX_MAT_ALIGN

#undef GFX_VEC_NAME
#undef GFX_VEC_FUNC

#ifdef __cplusplus
}
#endif

#endif // TEMPLATE
