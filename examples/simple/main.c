
#include <groufix.h>
#include <stdio.h>

int main()
{
	if(!gfx_init()) puts("nuuuuuh!");
	else puts("yeeeeeh!");

	void* mon = _gfx_platform_get_screen(0);
	printf("%p\n", mon);
	printf("%i monitor(s)\n%i x %i\n",
		_gfx_platform_get_num_screens(),
		_gfx_platform_screen_get_width(mon),
		_gfx_platform_screen_get_height(mon)
	);

	getchar();

	gfx_terminate();

	return 0;
}
