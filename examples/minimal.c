
#include <groufix.h>

int main()
{
	GFXContext context;
	context.major = 0;
	context.minor = 0;

	if(!gfx_init(context, GFX_ERROR_MODE_NORMAL))
		return 0;

	GFXBitDepth depth = {{ 8, 8, 8, 0 }};

	GFXWindow* window = gfx_window_create(
		NULL,
		0,
		&depth,
		"Groufix Window",
		100, 100,
		800, 600,
		GFX_WINDOW_RESIZABLE
	);

	while(gfx_poll_events() && gfx_window_is_open(window));

	gfx_window_free(window);
	gfx_terminate();

	return 0;
}
