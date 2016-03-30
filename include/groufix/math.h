/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#ifndef GFX_MATH_H
#define GFX_MATH_H

/* Mathematical structures */
#define GFX_MAT_USE_VEC
#define GFX_QUAT_USE_VEC
#define GFX_QUAT_USE_MAT
#include "groufix/math/vec.h"
#include "groufix/math/mat.h"
#include "groufix/math/quat.h"


/********************************************************
 * Mathematical definitions
 *******************************************************/

/** Mathematical constants */
GFX_API const long double GFX_MATH_PI;
GFX_API const long double GFX_MATH_TWO_PI;
GFX_API const long double GFX_MATH_HALF_PI;
GFX_API const long double GFX_MATH_RAD_TO_DEG;
GFX_API const long double GFX_MATH_DEG_TO_RAD;


#endif // GFX_MATH_H
