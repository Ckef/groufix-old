
#include <groufix.h>
#include <stdio.h>

int main()
{
	/* Really this is for testing purposes, in no way will this be the final usage */

	if(!gfx_init()) return 0;


	/* Setup 2 windows and fetch the OpenGL context version */

	GFXColorDepth depth;
	depth.redBits   = 8;
	depth.greenBits = 8;
	depth.blueBits  = 8;

	GFXWindow* window1 = gfx_window_create(NULL, depth, "Window Unos", 800, 600, 100, 100);
	GFXWindow* window2 = gfx_window_create(NULL, depth, "Window Deux", 800, 600, 300, 300);

	GFXContext context = gfx_window_get_context(window1);
	printf("%i %i\n", context.major, context.minor);


	/* Some triangle data setup */

	GFXHardwareBuffer* triangleBuf = NULL;
	GFXHardwareLayout* triangle = NULL;
	GFXHardwareContext contx = gfx_hardware_get_context();
	if(contx)
	{
		GFXHardwareAttribute attr;
		attr.size = 2;
		attr.type = GFX_FLOAT;
		attr.interpret = GFX_LAYOUT_FLOAT;
		attr.stride = 0;
		attr.offset = 0;
		attr.divisor = 0;

		float triangleArr[] = {
			-0.5f, -0.5f,
			 0.5f, -0.5f,
			 0.0f,  0.5f
		};

		triangleBuf = gfx_hardware_buffer_create(
			GFX_BUFFER_VERTEX_ARRAY,
			GFX_BUFFER_STATIC_WRITE,
			sizeof(triangleArr), triangleArr, contx);

		triangle = gfx_hardware_layout_create(contx);
		gfx_hardware_layout_enable_attribute(0, contx);
		gfx_hardware_layout_set_attribute(0, &attr, triangleBuf, contx);
	}


	/* Setup a loop */

	while(gfx_poll_events() && gfx_get_num_windows())
	{
		/* Err.. not using shaders, hoping your drivers provide default ones.. */
		if(contx) gfx_hardware_draw(GFX_TRIANGLES, 0, 3, contx);

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

	/* Free memory */
	gfx_hardware_layout_free(triangle, contx);
	gfx_hardware_buffer_free(triangleBuf, contx);

	gfx_terminate();

	return 0;
}
