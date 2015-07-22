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

#ifdef __cplusplus
extern "C" {
#endif


/* Mathematical constants */
extern GFX_API const long double GFX_MATH_PI;
extern GFX_API const long double GFX_MATH_TWO_PI;
extern GFX_API const long double GFX_MATH_HALF_PI;
extern GFX_API const long double GFX_MATH_RAD_TO_DEG;
extern GFX_API const long double GFX_MATH_DEG_TO_RAD;


#ifdef __cplusplus
}
#endif

#endif // GFX_MATH_H
