#include <stdio.h>

#include "groufix/math.h"
#include "groufix/window.h"

int main()
{
	if(!_gfx_platform_init()) puts("nuuuuuh!");
	else puts("yeeeeeh!");

	void* mon = _gfx_platform_get_monitor(0);
	printf("%i monitor(s)\n%i x %i\n",
		_gfx_platform_get_num_monitors(),
		_gfx_platform_monitor_get_width(mon),
		_gfx_platform_monitor_get_height(mon)
	);

	getchar();

	_gfx_platform_terminate();

	return 0;
}
