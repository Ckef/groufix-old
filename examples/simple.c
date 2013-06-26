
#include <groufix.h>
#include <stdio.h>

int main()
{
	/* Really this is for testing purposes, in no way will this be the final usage */

	if(!gfx_init()) return 0;

	GFXWindowDepth depth;
	depth.redBits   = 8;
	depth.greenBits = 8;
	depth.blueBits  = 8;

	GFXWindow* window1 = gfx_window_create(NULL, &depth, "Window Unos", 800, 600, 100, 100);
	GFXWindow* window2 = gfx_window_create(NULL, &depth, "Window Deux", 800, 600, 300, 300);

	unsigned short major, minor;

	if(_gfx_platform_context_get(window1->handle, &major, &minor))
		printf("%i.%i\n", major, minor);

	if(_gfx_platform_context_get(window2->handle, &major, &minor))
		printf("%i.%i\n", major, minor);

	while(gfx_poll_events() && gfx_get_num_windows())
	{
		_gfx_platform_context_swap_buffers(window1->handle);
		_gfx_platform_context_swap_buffers(window2->handle);
	}

	gfx_terminate();

	return 0;
}
