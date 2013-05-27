#include <stdio.h>

#include "groufix/math.h"
#include "groufix/window.h"

int main()
{
	// Create matrix
	mat4_float mat;
	mat4_float_set_zero(&mat);
	*mat4_float_get(&mat,0,0) = 2.0f;
	*mat4_float_get(&mat,1,1) = 3.0f;
	*mat4_float_get(&mat,2,2) = 1.0f;
	*mat4_float_get(&mat,3,3) = 1.0f;
	*mat4_float_get(&mat,0,3) = 1.0f;

	// Cast column to vector
	vec4_float* col = mat4_float_get_column(&mat, 3);
	*vec4_float_get(col, 0) = 2.0f;
	printf("(%f %f %f %f)\n",
		*vec4_float_get(col, 0),
		*vec4_float_get(col, 1),
		*vec4_float_get(col, 2),
		*vec4_float_get(col, 3)
	);

	// Matrix arithmetics
	mat4_float_mult(&mat, &mat, &mat);
	mat4_float_transpose(&mat, &mat);
	mat4_float_inverse(&mat, &mat);

	// Constant
	printf("%.16f\n", MATH_PI);

	// Print matrix
	printf("[%+f %+f %+f %+f]\n[%+f %+f %+f %+f]\n[%+f %+f %+f %+f]\n[%+f %+f %+f %+f]\n",
		*mat4_float_get(&mat,0,0), *mat4_float_get(&mat,0,1), *mat4_float_get(&mat,0,2), *mat4_float_get(&mat,0,3),
		*mat4_float_get(&mat,1,0), *mat4_float_get(&mat,1,1), *mat4_float_get(&mat,1,2), *mat4_float_get(&mat,1,3),
		*mat4_float_get(&mat,2,0), *mat4_float_get(&mat,2,1), *mat4_float_get(&mat,2,2), *mat4_float_get(&mat,2,3),
		*mat4_float_get(&mat,3,0), *mat4_float_get(&mat,3,1), *mat4_float_get(&mat,3,2), *mat4_float_get(&mat,3,3)
	);

	return 0;
}
