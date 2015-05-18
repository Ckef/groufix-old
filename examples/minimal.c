
#include <groufix.h>

int main()
{
	GFXContext context;
	context.major = 0;
	context.minor = 0;

	if(!gfx_init(context)) return 0;

	GFXDisplayMode mode;
	mode.width           = 800;
	mode.height          = 600;
	mode.depth.redBits   = 8;
	mode.depth.greenBits = 8;
	mode.depth.blueBits  = 8;

	GFXWindow* window = gfx_window_create(NULL, mode, "Groufix Window", 100, 100, GFX_WINDOW_RESIZABLE);

	while(gfx_poll_events() && gfx_window_is_open(window));

	gfx_window_free(window);
	gfx_terminate();

	return 0;
}
