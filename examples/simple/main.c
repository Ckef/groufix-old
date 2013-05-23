#include <stdio.h>

#include "groufix/math/vec.h"

int main()
{
	vec3_float vec;
	vec.data[0] = -5.0f;
	vec.data[1] = 76.0f;
	vec.data[2] = -4298.0f;

	vec3_float_normalize(&vec, &vec);
	vec3_float_scale(&vec, &vec, 5.0f);

	printf("%f %f %f : %f\n", vec.data[0], vec.data[1], vec.data[2], vec3_float_magnitude(&vec));

	return 0;
}
