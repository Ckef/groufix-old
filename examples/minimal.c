
#include <groufix.h>

int main()
{
	if(!gfx_init()) return 0;

	GFXColorDepth depth;
	depth.redBits   = 8;
	depth.greenBits = 8;
	depth.blueBits  = 8;

	GFXWindow* window = gfx_window_create(NULL, depth, "Groufix Window", 800, 600, 100, 100);

	while(gfx_poll_events() && gfx_get_num_windows())
	{
		gfx_window_swap_buffers(window);
	}

	gfx_terminate();

	return 0;
}
