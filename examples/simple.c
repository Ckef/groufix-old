
#include <groufix.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	/* Really this is for testing purposes, in no way will this be the final usage */

	if(!gfx_init()) return 0;

	GFX_Platform_Screen scr = _gfx_platform_get_default_screen();

	GFX_Platform_Attributes attr;
	attr.name      = "HEIRO";
	attr.screen    = scr;
	attr.width     = 800;
	attr.height    = 600;
	attr.x         = 100;
	attr.y         = 100;
	attr.redBits   = 4;
	attr.greenBits = 4;
	attr.blueBits  = 4;

	GFX_Platform_Window window = _gfx_platform_create_window(&attr);
	_gfx_platform_window_show(window);

	while(gfx_poll_events())
	{
		_gfx_platform_window_swap_buffers(window);
	}

	gfx_terminate();

	return 0;
}
