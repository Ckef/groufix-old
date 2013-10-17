
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

	GFXWindow* window1 = gfx_window_create(NULL, depth, "Window Unos", 800, 600, GFX_WINDOW_RESIZABLE);
	GFXWindow* window2 = gfx_window_create(NULL, depth, "Window Deux", 800, 600, 0);

	GFXContext context = gfx_window_get_context(window1);
	printf("%i.%i\n", context.major, context.minor);

	context = gfx_window_get_context(window2);
	printf("%i.%i\n", context.major, context.minor);


	/* Specify a triangle */
	float triangle[] = { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f };
	GFXBuffer* buffer = gfx_buffer_create(GFX_BUFFER_WRITE, GFX_VERTEX_BUFFER, sizeof(triangle), triangle, 0, 0);

	GFXVertexAttribute attr;
	attr.size          = 3;
	attr.type.unpacked = GFX_FLOAT;
	attr.interpret     = GFX_INTERPRET_FLOAT;
	attr.stride        = 0;
	attr.offset        = 0;
	attr.divisor       = 0;

	GFXDrawCall call;
	call.primitive = GFX_TRIANGLES;
	call.first     = 0;
	call.count     = 3;

	GFXVertexLayout* layout = gfx_vertex_layout_create();
	gfx_vertex_layout_set_attribute(layout, 0, &attr, buffer);
	unsigned short index = gfx_vertex_layout_push(layout, &call);


	/* Setup a loop */
	while(gfx_poll_events() && gfx_get_num_windows())
	{
		/* Draw a triangle */
		gfx_vertex_layout_draw(layout, 1, index);

		gfx_window_swap_buffers(window1);
		gfx_window_swap_buffers(window2);

		/* Print all the errors! */
		GFXError error;
		while(gfx_errors_peek(&error))
		{
			print_error(error);
			gfx_errors_pop();
		}
	}

	gfx_vertex_layout_free(layout);
	gfx_buffer_free(buffer);

	gfx_window_free(window1);
	gfx_window_free(window2);

	gfx_terminate();

	return 0;
}
