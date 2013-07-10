
#include <groufix.h>
#include <stdio.h>

int main()
{
	/* Really this is for testing purposes, in no way will this be the final usage */

	if(!gfx_init()) return 0;

	GFXColorDepth depth;
	depth.redBits   = 8;
	depth.greenBits = 8;
	depth.blueBits  = 8;

	GFXWindow* window1 = gfx_window_create(NULL, depth, "Window Unos", 800, 600, 100, 100);
	GFXWindow* window2 = gfx_window_create(NULL, depth, "Window Deux", 800, 600, 300, 300);

	GFXContext cont = gfx_window_get_context(window1);
	printf("%i.%i\n", cont.major, cont.minor);

	while(gfx_poll_events() && gfx_get_num_windows())
	{
		gfx_window_swap_buffers(window1);
		gfx_window_swap_buffers(window2);

		/* Print all the errors! */
		GFXError error;
		while(gfx_errors_peek(&error))
		{
			puts(error.message);
			puts(error.description);
			puts("");

			gfx_errors_pop();
		}
	}

	gfx_terminate();

	return 0;
}
