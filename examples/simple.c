
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
	float triangle[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};
	GFXBuffer* buffer = gfx_buffer_create(GFX_BUFFER_WRITE, GFX_VERTEX_BUFFER, sizeof(triangle), triangle, 0, 0);

	GFXVertexAttribute attr;
	attr.size          = 3;
	attr.type.unpacked = GFX_FLOAT;
	attr.interpret     = GFX_INTERPRET_FLOAT;
	attr.stride        = sizeof(float) * 6;
	attr.offset        = 0;
	attr.divisor       = 0;

	GFXDrawCall call;
	call.primitive = GFX_TRIANGLES;
	call.first     = 0;
	call.count     = 3;

	GFXVertexLayout* layout = gfx_vertex_layout_create(1);
	gfx_vertex_layout_set_attribute(layout, 0, &attr, buffer);
	attr.offset = sizeof(float) * 3;
	gfx_vertex_layout_set_attribute(layout, 1, &attr, buffer);
	gfx_vertex_layout_set(layout, 0, &call);


	/* Shaders! */
	const char* vertSrc =
		"#version 150\n"
		"in vec4 position;"
		"in vec3 color;"
		"out vec3 fragColor;"
		"void main() {"
		"gl_Position = position;"
		"fragColor = color;"
		"}";
	const char* fragSrc =
		"#version 150\n"
		"in vec3 fragColor;"
		"out vec3 outColor;"
		"void main() {"
		"outColor = fragColor;"
		"}";

	GFXShader* vert = gfx_shader_create(GFX_VERTEX_SHADER);
	gfx_shader_set_source(vert, 1, NULL, &vertSrc);
	GFXShader* frag = gfx_shader_create(GFX_FRAGMENT_SHADER);
	gfx_shader_set_source(frag, 1, NULL, &fragSrc);

	GFXShader* shaders[] = { vert, frag };

	GFXProgram* program = gfx_program_create();
	gfx_program_set_attribute(program, 0, "position");
	gfx_program_set_attribute(program, 1, "color");
	gfx_program_link(program, 2, shaders);

	gfx_shader_free(vert);
	gfx_shader_free(frag);


	/* Pipeline */
	GFXPipe pipe;
	GFXPipeline* pipeline = gfx_pipeline_create();

	unsigned short i = gfx_pipeline_push_bucket(pipeline, 0, GFX_BUCKET_SORT_ALL);
	gfx_pipeline_get(pipeline, i, NULL, NULL, &pipe);
	gfx_pipeline_set_state(pipeline, i, GFX_STATE_DEFAULT | GFX_CLEAR_COLOR);

	GFXBatchUnit* unit = gfx_bucket_insert(pipe.bucket, 0, 1);
	gfx_bucket_set_source(unit, program, layout);
	gfx_bucket_set_draw_calls(unit, 0, 1);
	gfx_bucket_set_mode(unit, GFX_BATCH_DIRECT, 0);

	unsigned short ip = gfx_pipeline_push_process(pipeline, 0);
	gfx_pipeline_get(pipeline, ip, NULL, NULL, &pipe);
	gfx_pipe_process_set_source(pipe.process, program);
	gfx_pipe_process_set_target(pipe.process, window2);


	/* Setup a loop */
	gfx_hardware_poll_errors(NULL);
	while(gfx_get_num_windows())
	{
		gfx_poll_events();

		/* Execute pipeline & swap buffers */
		gfx_pipeline_execute(pipeline);

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


	/* Free all the things */
	gfx_vertex_layout_free(layout);
	gfx_buffer_free(buffer);
	gfx_program_free(program);
	gfx_pipeline_free(pipeline);

	gfx_window_free(window1);
	gfx_window_free(window2);

	gfx_terminate();

	return 0;
}
