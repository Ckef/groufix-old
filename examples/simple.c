
#include <groufix.h>
#include <stdio.h>
#include <stdlib.h>

void windowClose(GFXWindow* window)
{
	gfx_window_free(window);
	printf("%i window(s) open\n", (unsigned int)gfx_get_num_windows());
}

int main()
{
	/* Really this is for testing purposes, in no way will this be the final usage */

	if(!gfx_init()) return 0;

	GFXWindowDepth depth;
	depth.redBits   = 4;
	depth.greenBits = 4;
	depth.blueBits  = 4;

	GFXWindow* window1 = gfx_window_create(NULL, &depth, "Window Unos", 800, 600, 100, 100);
	GFXWindow* window2 = gfx_window_create(NULL, &depth, "Window Deux", 800, 600, 300, 300);
	window1->callbacks.windowClose = windowClose;
	window2->callbacks.windowClose = windowClose;

	while(gfx_poll_events() && gfx_get_num_windows())
	{
		_gfx_platform_context_swap_buffers(window1->handle);
		_gfx_platform_context_swap_buffers(window2->handle);
	}

	gfx_terminate();

	return 0;
}
