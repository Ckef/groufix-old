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

#ifndef GFX_MATH_H
#define GFX_MATH_H

/* Mathematical structures */
#define MAT_USE_VEC
#define QUAT_USE_VEC
#define QUAT_USE_MAT
#include "groufix/math/vec.h"
#include "groufix/math/mat.h"
#include "groufix/math/quat.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Mathematical constants */
extern const double MATH_PI;
extern const double MATH_PI_TWO;
extern const double MATH_PI_HALF;
extern const double MATH_RAD_TO_DEG;
extern const double MATH_DEG_TO_RAD;


#ifdef __cplusplus
}
#endif

#endif // GFX_MATH_H
