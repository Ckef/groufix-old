
#include <groufix.h>
#include <stdio.h>


void print_error(GFXError error)
{
	printf("[Error #%x]: %s\n", error.code, error.description);
}

int main()
{
	/* Really this is for testing purposes, in no way will this be the final usage */

	GFXContext context;
	context.major = 0;
	context.minor = 0;

	if(!gfx_init(context, GFX_ERROR_MODE_DEBUG))
		return 0;


	/* Setup 2 windows */
	GFXBitDepth depth = {{ 8, 8, 8, 0 }};

	GFXWindow* window1 = gfx_window_create(NULL, 0, &depth, "Window Unos", 100, 100, 800, 600, GFX_WINDOW_RESIZABLE);
	GFXWindow* window2 = gfx_window_create(NULL, 0, &depth, "Window Deux", 200, 200, 800, 600, GFX_WINDOW_RESIZABLE);


	/* Test stuff */
	float floats[] = { 1.0f, 2.0f, 3.0f, 4.0f };
	float floats2[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	GFXBuffer* buff = gfx_buffer_create(
		GFX_BUFFER_READ | GFX_BUFFER_WRITE | GFX_BUFFER_MAP_READ | GFX_BUFFER_MAP_WRITE,
		sizeof(floats),
		NULL,
		0);

	gfx_buffer_write(buff, sizeof(floats), floats, 0);
	gfx_buffer_read(buff, sizeof(floats), floats2, 0);

	printf("\n%f %f %f %f", floats2[0], floats2[1], floats2[2], floats2[3]);

	size_t buffSize = sizeof(floats);
	float* buffPtr = gfx_buffer_map(buff, &buffSize, 0);

	printf("\n%f %f %f %f", buffPtr[0], buffPtr[1], buffPtr[2], buffPtr[3]);

	gfx_buffer_unmap(buff);


	/* Setup a loop */
	puts("\n");

	while(gfx_poll_events() && gfx_get_num_windows())
	{
		/* Print all the errors! */
		GFXError error;
		while(gfx_errors_peek(&error))
		{
			print_error(error);
			gfx_errors_pop();
		}
	}


	/* Free all the things */
	gfx_buffer_free(buff);
	gfx_window_free(window1);
	gfx_window_free(window2);

	gfx_terminate();

	return 0;
}
