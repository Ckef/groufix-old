
#include <groufix.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	if(!gfx_init()) puts("nuuuuuh!");
	else puts("yeeeeeh!");

	unsigned int width, height;
	GFX_Platform_Screen scr = _gfx_platform_get_default_screen();
	_gfx_platform_screen_get_size(scr, &width, &height);
	printf("%i screen(s)\n%i x %i\n",
		_gfx_platform_get_num_screens(),
		width,
		height
	);

	GFX_Platform_Attributes attr;
	attr.name   = "HEIRO";
	attr.screen = scr;
	attr.width  = 800;
	attr.height = 600;
	attr.x      = 100;
	attr.y      = 100;

	GFX_Platform_Window window = _gfx_platform_create_window(&attr);
	_gfx_platform_window_show(window);

	while(_gfx_platform_is_initialized())
	{
		_gfx_platform_poll_events();
	}

	gfx_terminate();

	return 0;
}
