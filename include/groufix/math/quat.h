/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#ifndef GFX_MATH_QUAT_H
#define GFX_MATH_QUAT_H

#include "groufix/utils.h"

#include <math.h>
#include <string.h>

#define GFX_QUAT_CREATE_NAME(type) GFX_NAME(gfx, GFX_CAT(type, quat))
#define GFX_QUAT_CREATE_FUNC(type,postfix) GFX_NAME(GFX_QUAT_CREATE_NAME(type), postfix)

#endif // GFX_MATH_QUAT_H


/* Invalid defines */
#if !defined(GFX_QUAT_TYPE) && defined(GFX_QUAT_DATA)
	#error "Missing define for GFX_QUAT_TYPE"

#elif !defined(GFX_QUAT_DATA) && defined(GFX_QUAT_TYPE)
	#error "Missing define for GFX_QUAT_DATA"


/* Load all default datatypes */
#elif !defined(GFX_QUAT_TYPE)

	#define GFX_QUAT_TYPE
	#define GFX_QUAT_DATA float
	#include "groufix/math/quat.h"
	#undef GFX_QUAT_DATA
	#undef GFX_QUAT_TYPE

	#define GFX_QUAT_TYPE d
	#define GFX_QUAT_DATA double
	#include "groufix/math/quat.h"
	#undef GFX_QUAT_DATA
	#undef GFX_QUAT_TYPE

#else


/********************************************************
 * Template definition
 *******************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/* Name & Function */
#define GFX_QUAT_NAME GFX_QUAT_CREATE_NAME(GFX_QUAT_TYPE)
#define GFX_QUAT_FUNC(postfix) GFX_QUAT_CREATE_FUNC(GFX_QUAT_TYPE, postfix)

/* Alignment */
#if GFX_QUAT_DATA == float
	#define GFX_QUAT_ALIGN GFX_SSE_ALIGN
#else
	#define GFX_QUAT_ALIGN GFX_SSE_NO_ALIGN
#endif

/* Matrix specific */
#ifdef GFX_QUAT_USE_MAT
	#ifndef GFX_MATH_MAT_H
		#error "Need to include groufix/math/mat.h to use GFX_QUAT_USE_MAT"
	#endif
	#define GFX_MAT_NAME GFX_MAT_CREATE_NAME(3, GFX_QUAT_TYPE)
#endif

/* Vector specific */
#ifdef GFX_QUAT_USE_VEC
	#ifndef GFX_MATH_VEC_H
		#error "Need to include groufix/math/vec.h to use GFX_QUAT_USE_VEC"
	#endif
	#define GFX_VEC_NAME GFX_VEC_CREATE_NAME(3, GFX_QUAT_TYPE)
#endif


/********************************************************
 * Quaternion Template
 *******************************************************/
typedef GFX_QUAT_ALIGN
{
	/** Components */
	GFX_QUAT_DATA data[4];

} GFX_QUAT_NAME;


/**
 * Returns a value of the quaternion.
 *
 */
inline GFX_QUAT_DATA* GFX_QUAT_FUNC(get)(

		GFX_QUAT_NAME*  a,
		size_t          component)
{
	return a->data + component;
}

/**
 * Sets the value of all a quaternion's components to 0.
 *
 * @return The given quaternion itself.
 *
 */
inline GFX_QUAT_NAME* GFX_QUAT_FUNC(set_zero)(

		GFX_QUAT_NAME* a)
{
	return (GFX_QUAT_NAME*)memset(a, 0, sizeof(GFX_QUAT_NAME));
}

/**
 * Add two quaternions.
 *
 * @param dest Destination quaternion.
 *
 */
inline GFX_QUAT_NAME* GFX_QUAT_FUNC(add)(

		GFX_QUAT_NAME*  dest,
		GFX_QUAT_NAME*  a,
		GFX_QUAT_NAME*  b)
{
	size_t i;
	for(i = 0; i < 4; ++i)
		dest->data[i] = a->data[i] + b->data[i];

	return dest;
}

/**
 * Subtract two quaternions.
 *
 * @param dest Destination quaternion.
 *
 */
inline GFX_QUAT_NAME* GFX_QUAT_FUNC(sub)(

		GFX_QUAT_NAME*  dest,
		GFX_QUAT_NAME*  a,
		GFX_QUAT_NAME*  b)
{
	size_t i;
	for(i = 0; i < 4; ++i)
		dest->data[i] = a->data[i] - b->data[i];

	return dest;
}

/**
 * Multiplies two quaternions.
 *
 * @param dest Destination quaternion.
 *
 */
inline GFX_QUAT_NAME* GFX_QUAT_FUNC(mult)(

		GFX_QUAT_NAME*  dest,
		GFX_QUAT_NAME*  a,
		GFX_QUAT_NAME*  b)
{
	GFX_QUAT_NAME res;
	res.data[0] = a->data[0] * b->data[0] - a->data[1] * b->data[1] - a->data[2] * b->data[2] - a->data[3] * b->data[3];
	res.data[1] = a->data[0] * b->data[1] + a->data[1] * b->data[0] + a->data[2] * b->data[3] - a->data[3] * b->data[2];
	res.data[2] = a->data[0] * b->data[2] - a->data[1] * b->data[3] + a->data[2] * b->data[0] + a->data[3] * b->data[1];
	res.data[3] = a->data[0] * b->data[3] + a->data[1] * b->data[2] - a->data[2] * b->data[1] + a->data[3] * b->data[0];

	*dest = res;

	return dest;
}

/**
 * Scales a quaternion by a scalar.
 *
 * @param dest Destination quaternion.
 *
 */
inline GFX_QUAT_NAME* GFX_QUAT_FUNC(scale)(

		GFX_QUAT_NAME*  dest,
		GFX_QUAT_NAME*  a,
		GFX_QUAT_DATA   scalar)
{
	size_t i;
	for(i = 0; i < 4; ++i)
		dest->data[i] = a->data[i] * scalar;

	return dest;
}

/**
 * Takes the conjugate of a quaternion.
 *
 * @param dest Destination quaternion.
 *
 */
inline GFX_QUAT_NAME* GFX_QUAT_FUNC(conjugate)(

		GFX_QUAT_NAME*  dest,
		GFX_QUAT_NAME*  a)
{
	dest->data[0] = +a->data[0];
	dest->data[1] = -a->data[1];
	dest->data[2] = -a->data[2];
	dest->data[3] = -a->data[3];

	return dest;
}

/**
 * Take the squared norm of a quaternion.
 *
 */
inline GFX_QUAT_DATA GFX_QUAT_FUNC(norm_squared)(

		GFX_QUAT_NAME* a)
{
	GFX_QUAT_DATA norm = 0;

	size_t i;
	for(i = 0; i < 4; ++i)
	{
		GFX_QUAT_DATA *val = a->data + i;
		norm += (*val) * (*val);
	}
	return norm;
}

/**
 * Take the norm of a quaternion.
 *
 */
inline double GFX_QUAT_FUNC(norm)(

		GFX_QUAT_NAME* a)
{
	return sqrt((double)GFX_QUAT_FUNC(norm_squared)(a));
}

/**
 * Normalize a quaternion.
 *
 * @param dest Destination quaternion.
 *
 */
inline GFX_QUAT_NAME* GFX_QUAT_FUNC(normalize)(

		GFX_QUAT_NAME*  dest,
		GFX_QUAT_NAME*  a)
{
	double norm = GFX_QUAT_FUNC(norm)(a);
	GFX_QUAT_DATA scale = (GFX_QUAT_DATA)(norm ? 1.0 / norm : 0.0);

	return GFX_QUAT_FUNC(scale)(dest, a, scale);
}

/**
 * Takes the inverse of a quaternion.
 *
 * @param dest Destination quaternion.
 *
 */
inline GFX_QUAT_NAME* GFX_QUAT_FUNC(inverse)(

		GFX_QUAT_NAME*  dest,
		GFX_QUAT_NAME*  a)
{
	double normSq = GFX_QUAT_FUNC(norm_squared)(a);
	GFX_QUAT_DATA scale = (GFX_QUAT_DATA)(normSq ? 1.0f / normSq : 0.0);

	GFX_QUAT_FUNC(conjugate)(dest, a);
	return GFX_QUAT_FUNC(scale)(dest, dest, scale);
}

/**
 * Compares a quaternion against 0, component wise.
 *
 * @return If the quaternion is zero, a non-zero value is returned.
 *
 */
inline int GFX_QUAT_FUNC(is_zero)(

		GFX_QUAT_NAME* a)
{
	size_t i;
	for(i = 0; i < 4; ++i)
		if(a->data[i]) return 0;

	return 1;
}

#ifdef GFX_QUAT_USE_VEC
/**
 * Computes a quaternion from an angle and an axis.
 *
 * @param dest  Destination quaternion.
 * @param angle Angle in radians.
 * @param axis  Axis to 'rotate' around.
 *
 * This method assumes the axis is of unit length (magnitude or magnitude_squared = 1).
 *
 */
inline GFX_QUAT_NAME* GFX_QUAT_FUNC(from_angle_axis)(

		GFX_QUAT_NAME*  dest,
		double          angle,
		GFX_VEC_NAME*   axis)
{
	double half = angle * 0.5;
	double scale = sin(half);

	dest->data[1] = axis->data[0] * scale;
	dest->data[2] = axis->data[1] * scale;
	dest->data[3] = axis->data[2] * scale;
	dest->data[0] = cos(half);

	return dest;
}

/**
 * Computes an angle and axis from a quaternion.
 *
 * @param src   Source quaternion.
 * @param angle Angle in radians.
 * @param axis  Axis to 'rotate' around.
 *
 * This method assumes the quaternion is of unit length (norm or norm_squared = 1).
 *
 */
inline void GFX_QUAT_FUNC(to_angle_axis)(

		GFX_QUAT_NAME*  src,
		double*         angle,
		GFX_VEC_NAME*   axis)
{
	double half = acos(src->data[0]);
	double scale = sin(half);
	if(scale) scale = 1.0 / scale;

	*angle = half * 2.0;
	axis->data[0] = src->data[1] * scale;
	axis->data[1] = src->data[2] * scale;
	axis->data[2] = src->data[3] * scale;
}

/**
 * Multiplies a 'position' vector by a quaternion.
 *
 * @param dest Destination vector.
 *
 * This method assumes the quaternion is of unit length (norm or norm_squared = 1).
 *
 */
inline GFX_VEC_NAME* GFX_QUAT_FUNC(mult_vec)(

		GFX_VEC_NAME*   dest,
		GFX_QUAT_NAME*  a,
		GFX_VEC_NAME*   b)
{
	/* want = q * v * q^-1 (sandwich product)
	 * in which v = (0, bx, by, bz) and q^-1 = (aw, -ax, -ay, -az) (conjugate)
	 * because of the 0 we write it out to spare some calculations
	 * first we compute v * q^-1
	 */
	GFX_QUAT_NAME res;
	res.data[0] = b->data[0] * a->data[1] + b->data[1] * a->data[2] + b->data[2] * a->data[3];
	res.data[1] = b->data[0] * a->data[0] - b->data[1] * a->data[3] + b->data[2] * a->data[2];
	res.data[2] = b->data[0] * a->data[3] + b->data[1] * a->data[0] - b->data[2] * a->data[1];
	res.data[3] = b->data[1] * a->data[1] + b->data[2] * a->data[0] - b->data[0] * a->data[2];

	/* next we compute q * the above */
	dest->data[0] = a->data[0] * res.data[1] + a->data[1] * res.data[0] + a->data[2] * res.data[3] - a->data[3] * res.data[2];
	dest->data[1] = a->data[0] * res.data[2] - a->data[1] * res.data[3] + a->data[2] * res.data[0] + a->data[3] * res.data[1];
	dest->data[2] = a->data[0] * res.data[3] + a->data[1] * res.data[2] - a->data[2] * res.data[1] + a->data[3] * res.data[0];

	return dest;
}
#endif // GFX_QUAT_USE_VEC

#ifdef GFX_QUAT_USE_MAT
/**
 * Computes the 3x3 rotation matrix equivalent to a quaternion.
 *
 * @param dest Destination matrix.
 *
 * This method assumes the quaternion is of unit length (norm or norm_squared = 1).
 *
 */
inline GFX_MAT_NAME* GFX_QUAT_FUNC(to_matrix)(

		GFX_MAT_NAME*   dest,
		GFX_QUAT_NAME*  a)
{
	GFX_QUAT_DATA x2 = a->data[1] + a->data[1];
	GFX_QUAT_DATA y2 = a->data[2] + a->data[2];
	GFX_QUAT_DATA z2 = a->data[3] + a->data[3];

	GFX_QUAT_DATA wx = x2 * a->data[0];
	GFX_QUAT_DATA wy = y2 * a->data[0];
	GFX_QUAT_DATA wz = z2 * a->data[0];
	GFX_QUAT_DATA xx = x2 * a->data[1];
	GFX_QUAT_DATA xy = y2 * a->data[1];
	GFX_QUAT_DATA xz = z2 * a->data[1];
	GFX_QUAT_DATA yy = y2 * a->data[2];
	GFX_QUAT_DATA yz = z2 * a->data[2];
	GFX_QUAT_DATA zz = z2 * a->data[3];

	dest->data[0] = 1 - yy - zz; dest->data[3] = xy - wz; dest->data[6] = xz + wy;
	dest->data[1] = xy + wz; dest->data[4] = 1 - xx - zz; dest->data[7] = yz - wx;
	dest->data[2] = xz - wy; dest->data[5] = yz + wx; dest->data[8] = 1 - xx - yy;

	return dest;
}
#endif // GFX_QUAT_USE_MAT

#undef GFX_QUAT_NAME
#undef GFX_QUAT_FUNC
#undef GFX_QUAT_ALIGN

#undef GFX_MAT_NAME
#undef GFX_VEC_NAME

#ifdef __cplusplus
}
#endif

#endif // TEMPLATE
