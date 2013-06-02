
#include <groufix.h>
#include <stdio.h>

int main()
{
	if(!gfx_init()) puts("nuuuuuh!");
	else puts("yeeeeeh!");

	void* scr = _gfx_platform_get_default_screen();
	printf("%p\n%i screen(s)\n%i x %i\n", scr,
		_gfx_platform_get_num_screens(),
		_gfx_platform_screen_get_width(scr),
		_gfx_platform_screen_get_height(scr)
	);

	getchar();

	gfx_terminate();

	return 0;
}
