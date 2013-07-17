
#include <groufix.h>
#include <stdio.h>
#include <stdlib.h>

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

	const GFXHardwareContext cnt = gfx_hardware_get_context();
	if(cnt)
	{
		float data[] = { 5.6f, 9.2f, 781.0f };
		size_t size = sizeof(float) * 3;
		GFXHardwareBuffer buff = gfx_hardware_buffer_create(GFX_BUFFER_VERTEX_ARRAY, cnt);
		gfx_hardware_buffer_allocate(GFX_BUFFER_VERTEX_ARRAY, size, data, GFX_BUFFER_STATIC_WRITE, cnt);

		float* read = malloc(size);
		gfx_hardware_buffer_read(GFX_BUFFER_VERTEX_ARRAY, 0, size, read, cnt);

		printf("%f %f %f\n", read[0], read[1], read[2]);

		gfx_hardware_buffer_free(buff, cnt);
	}

	while(gfx_poll_events() && gfx_get_num_windows())
	{
		gfx_window_swap_buffers(window1);
		gfx_window_swap_buffers(window2);

		/* Print all the errors! */
		GFXError error;
		while(gfx_errors_peek(&error))
		{
			printf("[Error #%x]: %s\n", error.code, error.description);
			gfx_errors_pop();
		}
	}

	gfx_terminate();

	return 0;
}
