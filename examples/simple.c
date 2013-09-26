
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

	GFXWindow* window1 = gfx_window_create(NULL, depth, "Window Unos", 800, 600, 100, 100, GFX_WINDOW_RESIZABLE);
	GFXWindow* window2 = gfx_window_create(NULL, depth, "Window Deux", 800, 600, 300, 300, 0);

	GFXContext context = gfx_window_get_context(window1);
	printf("%i.%i\n", context.major, context.minor);

	context = gfx_window_get_context(window2);
	printf("%i.%i\n", context.major, context.minor);


	/* Testing segmented buffers */
	float data[] = { 0.0f, 5.0f, -8.0f };
	float data2[] = { 50.0f, 76.4f, 4.4f };

	/* 1 backbuffer + 2 segments per buffer = 4 total segments */
	GFXBuffer* buffer = gfx_buffer_create(GFX_BUFFER_WRITE, GFX_VERTEX_BUFFER, sizeof(data) * 2, NULL, 1, 2);
	gfx_buffer_write(buffer, sizeof(data), data, 0);

	gfx_buffer_swap(buffer);
	void* ptr = gfx_buffer_map_segment(buffer, GFX_BUFFER_WRITE);
	memcpy(ptr, data2, sizeof(data2));
	gfx_buffer_unmap(buffer);

	gfx_buffer_swap(buffer);
	gfx_buffer_swap(buffer);
	gfx_buffer_swap(buffer);
	ptr = gfx_buffer_map_segment(buffer, GFX_BUFFER_READ);
	memcpy(data2, ptr, sizeof(data2));
	gfx_buffer_unmap(buffer);

	gfx_buffer_swap(buffer);
	ptr = gfx_buffer_map_segment(buffer, GFX_BUFFER_READ);
	memcpy(data, ptr, sizeof(data));
	gfx_buffer_unmap(buffer);

	gfx_buffer_free(buffer);

	printf("%f %f %f\n%f %f %f\n", data[0], data[1], data[2], data2[0], data2[1], data2[2]);


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
