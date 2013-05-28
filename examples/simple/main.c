#include <stdio.h>

#include "groufix/math.h"
#include "groufix/window.h"

int main()
{
	if(!_gfx_platform_init()) puts("nuuuuuh!");
	else puts("yeeeeeh!");

	_gfx_platform_terminate();

	getchar();

	return 0;
}
