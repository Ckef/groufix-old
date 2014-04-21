
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
	gfx_set_error_mode(GFX_ERROR_MODE_DEBUG);


	/* Setup 2 windows */
	GFXColorDepth depth;
	depth.redBits   = 8;
	depth.greenBits = 8;
	depth.blueBits  = 8;

	GFXWindow* window1 = gfx_window_create(NULL, depth, "Window Unos", 100, 100, 800, 600, GFX_WINDOW_RESIZABLE);
	GFXWindow* window2 = gfx_window_create(NULL, depth, "Window Deux", 200, 200, 800, 600, GFX_WINDOW_RESIZABLE);


	/* Specify a triangle */
	float triangle[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	GFXSharedBuffer buffer;
	gfx_shared_buffer_init(&buffer, GFX_VERTEX_BUFFER, sizeof(triangle), triangle);

	GFXVertexAttribute attr;
	attr.size          = 3;
	attr.type.unpacked = GFX_FLOAT;
	attr.interpret     = GFX_INTERPRET_FLOAT;
	attr.stride        = sizeof(float) * 6;
	attr.divisor       = 0;

	GFXDrawCall call;
	call.primitive = GFX_TRIANGLES;
	call.first     = 0;
	call.count     = 3;

	GFXVertexLayout* layout = gfx_vertex_layout_create(1);
	gfx_vertex_layout_set_attribute(layout, 0, &attr);
	gfx_vertex_layout_set_attribute(layout, 1, &attr);
	gfx_vertex_layout_set_attribute_shared_buffer(layout, 0, &buffer, 0);
	gfx_vertex_layout_set_attribute_shared_buffer(layout, 1, &buffer, sizeof(float) * 3);
	gfx_vertex_layout_set_draw_call(layout, 0, &call);


	/* Texture */
	GFXTextureFormat format;
	format.components    = 3;
	format.type.unpacked = GFX_UNSIGNED_BYTE;
	format.interpret     = GFX_INTERPRET_NORMALIZED;

	GFXTexture* tex = gfx_texture_create(GFX_TEXTURE_2D, format, 0, 800, 600, 1);

	GFXTextureImage image;
	image.texture = tex;
	image.mipmap  = 0;
	image.layer   = 0;


	/* Shaders! */
	const char* vertSrc =
		"in vec4 position;"
		"in vec3 color;"
		"out vec3 fragColor;"
		"void main() {"
		"gl_Position = position;"
		"fragColor = color;"
		"}";
	const char* fragSrc =
		"in vec3 fragColor;"
		"out vec3 outColor;"
		"void main() {"
		"outColor = fragColor;"
		"}";

	const char* vertSrc2 =
		"in ivec4 data;"
		"out vec2 coord;"
		"void main() {"
		"gl_Position = vec4(data.xy, 0, 1);"
		"coord = data.zw;"
		"}";
	const char* fragSrc2 =
		"in vec2 coord;"
		"out vec3 color;"
		"uniform sampler2D tex;"
		"void main() {"
		"color = vec3(1.0f) - texture(tex, coord).rgb;"
		"}";

	const char* fragSrc3 =
		"in vec2 coord;"
		"out vec3 color;"
		"uniform sampler2D tex;"
		"void main() {"
		"color = texture(tex, coord).rgb;"
		"}";

	GFXShader* vert = gfx_shader_create(GFX_VERTEX_SHADER);
	GFXShader* frag = gfx_shader_create(GFX_FRAGMENT_SHADER);
	gfx_shader_set_source(vert, 1, &vertSrc);
	gfx_shader_set_source(frag, 1, &fragSrc);

	GFXShader* shaders[] = { vert, frag };

	GFXProgram* program = gfx_program_create();
	gfx_program_set_attribute(program, 0, "position");
	gfx_program_set_attribute(program, 1, "color");
	gfx_program_link(program, 2, shaders, 0);

	gfx_shader_set_source(vert, 1, &vertSrc2);
	gfx_shader_set_source(frag, 1, &fragSrc2);

	GFXProgram* program2 = gfx_program_create();
	gfx_program_set_attribute(program2, 0, "data");
	gfx_program_link(program2, 2, shaders, 0);

	gfx_shader_set_source(frag, 1, &fragSrc3);

	GFXProgram* program3 = gfx_program_create();
	gfx_program_set_attribute(program3, 0, "data");
	gfx_program_link(program3, 2, shaders, 0);

	gfx_shader_free(vert);
	gfx_shader_free(frag);


	/* Property map */
	GFXPropertyMap* map = gfx_property_map_create(program, 0);
	GFXPropertyMap* map2 = gfx_property_map_create(program2, 1);
	GFXPropertyMap* map3 = gfx_property_map_create(program3, 1);
	gfx_property_map_forward_named(map2, 0, 0, "tex");
	gfx_property_map_set_sampler(map2, 0, 0, tex);
	gfx_property_map_forward_named(map3, 0, 0, "tex");
	gfx_property_map_set_sampler(map3, 0, 0, tex);


	/* Pipeline */
	GFXPipeline* pipeline = gfx_pipeline_create();

	char targets[] = { 0 };
	gfx_pipeline_target(pipeline, 800, 600, 1, targets);
	gfx_pipeline_attach(pipeline, image, GFX_COLOR_ATTACHMENT, 0);

	GFXPipe* pipe = gfx_pipeline_push_bucket(pipeline, 0, GFX_BUCKET_SORT_ALL);
	gfx_pipe_set_state(pipe, GFX_STATE_DEFAULT | GFX_CLEAR_COLOR);

	GFXVertexSource source = { 0, 1, 0, 0 };
	size_t src = gfx_bucket_add_source(pipe->bucket, layout);
	gfx_bucket_set_source(pipe->bucket, src, source);
	gfx_bucket_insert(pipe->bucket, src, map, 0, 1);

	pipe = gfx_pipeline_push_process(pipeline);
	gfx_pipe_process_set_source(pipe->process, map2, 0);
	gfx_pipe_process_set_target(pipe->process, window1, 1);

	pipe = gfx_pipeline_push_process(pipeline);
	gfx_pipe_process_set_source(pipe->process, map3, 0);
	gfx_pipe_process_set_target(pipe->process, window2, 1);


	/* Setup a loop */
	while(gfx_get_num_windows())
	{
		gfx_poll_events();

		/* Execute pipeline & swap buffers */
		gfx_pipeline_execute(pipeline, 0);

		/* Print time */
		double time = gfx_get_time();
		gfx_set_time(0.0);

		printf("%f\n", 1.0 / time);

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
	gfx_shared_buffer_clear(&buffer);
	gfx_property_map_free(map);
	gfx_property_map_free(map2);
	gfx_property_map_free(map3);
	gfx_program_free(program);
	gfx_program_free(program2);
	gfx_program_free(program3);
	gfx_texture_free(tex);
	gfx_pipeline_free(pipeline);

	gfx_window_free(window1);
	gfx_window_free(window2);

	gfx_terminate();

	return 0;
}
