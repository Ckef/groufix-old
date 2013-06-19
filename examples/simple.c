
#include <groufix.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	/* Really this is for testing purposes, in no way will this be the final usage */

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

	GFX_Platform_WindowAttributes attr;
	attr.name   = "HEIRO";
	attr.screen = scr;
	attr.width  = 800;
	attr.height = 600;
	attr.x      = 100;
	attr.y      = 100;

	GFX_Platform_Window window = _gfx_platform_create_window(&attr);
	_gfx_platform_window_show(window);

	GFX_Platform_ContextAttributes attr2;
	attr2.redBits   = 4;
	attr2.blueBits  = 4;
	attr2.greenBits = 4;

	GFX_Platform_Context context = _gfx_platform_create_context(window, &attr2);

	while(_gfx_platform_is_initialized())
	{
		_gfx_platform_poll_events();
		_gfx_platform_context_swap_buffers(context);
	}

	gfx_terminate();

	return 0;
}
