
#include <groufix.h>
#include <stdio.h>

void print_error(GFXError error)
{
	printf("[Error #%x]: %s\n", error.code, error.description);
}

int main()
{
	/* Really this is for testing purposes, in no way will this be the final usage */

	if(!gfx_init())
	{
		GFXError error;
		if(gfx_errors_peek(&error)) print_error(error);

		return 0;
	}


	/* Setup 2 windows and fetch the OpenGL context version */
	GFXColorDepth depth;
	depth.redBits   = 8;
	depth.greenBits = 8;
	depth.blueBits  = 8;

	GFXWindow* window1 = gfx_window_create(NULL, depth, "Window Unos", 800, 600, 100, 100);
	GFXWindow* window2 = gfx_window_create(NULL, depth, "Window Deux", 800, 600, 300, 300);

	GFXContext context = gfx_window_get_context(window1);
	printf("%i.%i\n", context.major, context.minor);

	context = gfx_window_get_context(window2);
	printf("%i.%i\n", context.major, context.minor);


	/* Setup a loop */
	while(gfx_poll_events() && gfx_get_num_windows())
	{
		gfx_window_swap_all_buffers();


		/* Print all the errors! */
		GFXError error;
		while(gfx_errors_peek(&error))
		{
			print_error(error);
			gfx_errors_pop();
		}
	}

	gfx_terminate();

	return 0;
}
