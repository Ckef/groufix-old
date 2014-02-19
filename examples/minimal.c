
#include <groufix.h>

int main()
{
	if(!gfx_init()) return 0;

	GFXColorDepth depth;
	depth.redBits   = 8;
	depth.greenBits = 8;
	depth.blueBits  = 8;

	GFXWindow* window = gfx_window_create(NULL, depth, "Groufix Window", 100, 100, 800, 600, GFX_WINDOW_RESIZABLE);

	while(gfx_window_is_open(window))
	{
		gfx_poll_events();
	}

	gfx_window_free(window);

	gfx_terminate();

	return 0;
}
