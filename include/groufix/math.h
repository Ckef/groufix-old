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
extern const long double GFX_MATH_PI;
extern const long double GFX_MATH_TWO_PI;
extern const long double GFX_MATH_HALF_PI;
extern const long double GFX_MATH_RAD_TO_DEG;
extern const long double GFX_MATH_DEG_TO_RAD;


#ifdef __cplusplus
}
#endif

#endif // GFX_MATH_H
