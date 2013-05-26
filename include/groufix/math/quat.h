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

#ifndef GFX_MATH_QUAT_H
#define GFX_MATH_QUAT_H

#include <math.h>
#include <string.h>
#include "groufix/utils.h"

#define QUAT_CREATE_NAME(type) NAME(quat, type)

#endif // GFX_MATH_QUAT_H


/* Load all default datatypes */
#if !defined(QUAT_TYPE)

	#define QUAT_TYPE float
	#include "groufix/math/quat.h"
	#undef QUAT_TYPE

	#define QUAT_TYPE double
	#include "groufix/math/quat.h"
	#undef QUAT_TYPE

#else

/* Name & Function */
#define QUAT_NAME QUAT_CREATE_NAME(QUAT_TYPE)
#define QUAT_FUNC(postfix) NAME(QUAT_NAME, postfix)

/* Matrix specific */
#ifdef QUAT_USE_MAT
	#ifndef GFX_MATH_MAT_H
		#error "Need to include groufix/math/mat.h to use QUAT_USE_MAT"
	#endif
	#define MAT_NAME MAT_CREATE_NAME(3, QUAT_TYPE)
#endif

/*  Vector specific */
#ifdef QUAT_USE_VEC
	#ifndef GFX_MATH_VEC_H
		#error "Need to include groufix/math/vec.h to use QUAT_USE_VEC"
	#endif
	#define VEC_NAME VEC_CREATE_NAME(3, QUAT_TYPE)
#endif

/** \brief Quaternion defintion */
typedef struct
{
	/** Components */
	QUAT_TYPE data[4];
}
QUAT_NAME;

/**
 * \brief Returns a value of the quaternion.
 *
 */
inline QUAT_TYPE *QUAT_FUNC(get)(QUAT_NAME *a, size_t component)
{
	return a->data + component;
}

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
	dest->data[0] = +a->data[0];
	dest->data[1] = -a->data[1];
	dest->data[2] = -a->data[2];
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

#ifdef QUAT_USE_VEC
/**
 * \brief Computes a quaternion from an angle and an axis.
 *
 * \param dest  Destination quaternion.
 * \param angle Angle in radians.
 * \param axis  Axis to 'rotate' around.
 *
 * This method assumes the axis is of unit length (magnitude or magnitude_squared = 1).
 *
 */
inline QUAT_NAME *QUAT_FUNC(from_angle_axis)(QUAT_NAME *dest, double angle, VEC_NAME *axis)
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
 * \brief Computes an angle and axis from a quaternion.
 *
 * \param src   Source quaternion.
 * \param angle Angle in radians.
 * \param axis  Axis to 'rotate' around.
 *
 * This method assumes the quaternion is of unit length (norm or norm_squared = 1).
 *
 */
inline void QUAT_FUNC(to_angle_axis)(QUAT_NAME *src, double *angle, VEC_NAME *axis)
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
 * \brief Multiplies a 'position' vector by a quaternion.
 *
 * \param dest Destination vector.
 *
 * This method assumes the quaternion is of unit length (norm or norm_squared = 1).
 *
 */
inline VEC_NAME *QUAT_FUNC(mult_vec)(VEC_NAME *dest, QUAT_NAME *a, VEC_NAME *b)
{
	/* want = q * v * q^-1 (sandwich product)
	 * in which v = (0, bx, by, bz) and q^-1 = (aw, -ax, -ay, -az) (conjugate)
	 * because of the 0 we write it out to spare some calculations
	 * first we compute v * q^-1
	 */
	QUAT_NAME res;
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
#endif // QUAT_USE_VEC

#ifdef QUAT_USE_MAT
/**
 * \brief Computes the 3x3 rotation matrix equivalent to a quaternion.
 *
 * \param dest Destination matrix.
 *
 * This method assumes the quaternion is of unit length (norm or norm_squared = 1).
 *
 */
inline MAT_NAME *QUAT_FUNC(to_matrix)(MAT_NAME *dest, QUAT_NAME *a)
{
	QUAT_TYPE x2 = a->data[1] + a->data[1];
	QUAT_TYPE y2 = a->data[2] + a->data[2];
	QUAT_TYPE z2 = a->data[3] + a->data[3];

	QUAT_TYPE wx = x2 * a->data[0];
	QUAT_TYPE wy = y2 * a->data[0];
	QUAT_TYPE wz = z2 * a->data[0];
	QUAT_TYPE xx = x2 * a->data[1];
	QUAT_TYPE xy = y2 * a->data[1];
	QUAT_TYPE xz = z2 * a->data[1];
	QUAT_TYPE yy = y2 * a->data[2];
	QUAT_TYPE yz = z2 * a->data[2];
	QUAT_TYPE zz = z2 * a->data[3];

	dest->data[0] = 1 - yy - zz; dest->data[3] = xy - wz; dest->data[6] = xz + wy;
	dest->data[1] = xy + wz; dest->data[4] = 1 - xx - zz; dest->data[7] = yz - wx;
	dest->data[2] = xz - wy; dest->data[5] = yz + wx; dest->data[8] = 1 - xx - yy;

	return dest;
}
#endif // QUAT_USE_MAT

#undef QUAT_NAME
#undef QUAT_FUNC

#undef MAT_NAME
#undef VEC_NAME
#endif // TEMPLATE
