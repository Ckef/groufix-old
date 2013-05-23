#include <stdio.h>

#include "Groufix/Math/vec.h"

int main()
{
	vec3_float vec;
	vec[0] = -5.0f;
	vec[1] = 76.0f;
	vec[2] = -4298.0f;

	vec3_float_normalize(vec, vec);
	vec3_float_scale(vec, vec, 5.0f);

	printf("%f %f %f : %f\n", vec[0], vec[1], vec[2], vec3_float_magnitude(vec));

	return 0;
}
