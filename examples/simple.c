
#include <groufix.h>
#include <groufix/scene.h>

#include <stdio.h>

static GFXProgramMap* programMap;


void print_error(GFXError error)
{
	printf("[Error #%x]: %s\n", error.code, error.description);
}

GFXMesh* create_mesh()
{
	GFXMesh* mesh = gfx_mesh_create();
	GFXMeshLayout id = gfx_mesh_add_layout(mesh, 1);
	GFXVertexLayout* layout = gfx_mesh_get_layout(mesh, id);

	GFXVertexAttribute attr;
	attr.size          = 3;
	attr.type.unpacked = GFX_FLOAT;
	attr.interpret     = GFX_INTERPRET_FLOAT;
	attr.offset        = 0;

	GFXDrawCall call;
	call.primitive = GFX_TRIANGLES;
	call.first     = 0;
	call.count     = 3;

	gfx_vertex_layout_set_attribute(layout, 0, attr, 0);
	attr.offset = sizeof(float) * 3;
	gfx_vertex_layout_set_attribute(layout, 1, attr, 0);
	gfx_vertex_layout_set_draw_call(layout, 0, call);

	float triangle[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	GFXMeshBuffer buff = gfx_mesh_add_buffer(mesh, sizeof(triangle), triangle);
	gfx_mesh_set_vertex_buffer(mesh, id, buff, 0, 0, sizeof(float) * 6);
	gfx_mesh_add(mesh, 0, id, 0);

	return mesh;
}

GFXMaterial* create_material()
{
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

	GFXShader* vert = gfx_shader_create(GFX_VERTEX_SHADER);
	GFXShader* frag = gfx_shader_create(GFX_FRAGMENT_SHADER);
	gfx_shader_set_source(vert, 1, &vertSrc, NULL);
	gfx_shader_set_source(frag, 1, &fragSrc, NULL);

	GFXShader* shaders[] = { vert, frag };

	programMap = gfx_program_map_create();
	GFXProgram* program = gfx_program_map_add(programMap, GFX_ALL_SHADERS, 1);
	gfx_program_set_attribute(program, 0, "position");
	gfx_program_set_attribute(program, 1, "color");
	gfx_program_link(program, 2, shaders, 0);

	GFXMaterial* mat = gfx_material_create();
	gfx_material_add(mat, 0, programMap, 0);

	gfx_shader_free(vert);
	gfx_shader_free(frag);

	return mat;
}

int main()
{
	/* Really this is for testing purposes, in no way will this be the final usage */

	GFXContext context;
	context.major = 0;
	context.minor = 0;

	if(!gfx_init(context))
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


	/* Pipeline */
	GFXPipeline* pipeline = gfx_pipeline_create();

	char targets[] = { 0 };
	GFXViewport viewport = { 0, 0, 800, 600 };
	pipeline->viewport = viewport;
	gfx_pipeline_target(pipeline, 1, targets);

	GFXPipe* bucket = gfx_pipeline_push_bucket(pipeline, 0, GFX_BUCKET_SORT_ALL);
	gfx_pipe_get_state(bucket)->render.state = GFX_STATE_DEFAULT | GFX_CLEAR_COLOR;


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

	gfx_pipeline_attach(pipeline, image, GFX_COLOR_ATTACHMENT, 0);


	/* Post processing */
	const char* vertSrc =
		"in ivec4 data;"
		"out vec2 coord;"
		"void main() {"
		"gl_Position = vec4(data.xy, 0, 1);"
		"coord = data.zw;"
		"}";

	const char* fragSrcA =
		"in vec2 coord;"
		"out vec3 color;"
		"uniform sampler2D tex;"
		"void main() {"
		"color = vec3(1.0f) - texture(tex, coord).rgb;"
		"}";
	const char* fragSrcB =
		"in vec2 coord;"
		"out vec3 color;"
		"uniform sampler2D tex;"
		"void main() {"
		"color = texture(tex, coord).rgb;"
		"}";

	GFXShader* vert = gfx_shader_create(GFX_VERTEX_SHADER);
	GFXShader* frag = gfx_shader_create(GFX_FRAGMENT_SHADER);
	gfx_shader_set_source(vert, 1, &vertSrc, NULL);
	gfx_shader_set_source(frag, 1, &fragSrcA, NULL);

	GFXShader* shaders[] = { vert, frag };

	GFXPipe* pipeA = gfx_pipeline_push_process(pipeline, window1, 0);
	GFXProgram* programA = gfx_pipe_process_add(pipeA->process, GFX_ALL_SHADERS, 1);
	gfx_program_set_attribute(programA, 0, "data");
	gfx_program_link(programA, 2, shaders, 0);

	gfx_shader_set_source(frag, 1, &fragSrcB, NULL);

	GFXPipe* pipeB = gfx_pipeline_push_process(pipeline, window2, 0);
	GFXProgram* programB = gfx_pipe_process_add(pipeB->process, GFX_ALL_SHADERS, 1);
	gfx_program_set_attribute(programB, 0, "data");
	gfx_program_link(programB, 2, shaders, 0);

	gfx_shader_free(vert);
	gfx_shader_free(frag);


	/* Property map */
	GFXSampler sampler =
	{
		.minFilter = GFX_FILTER_LINEAR,
		.mipFilter = GFX_FILTER_NEAREST,
		.magFilter = GFX_FILTER_LINEAR,

		.maxAnisotropy = 1.0f,

		.lodMin = 0,
		.lodMax = 0,

		.wrapS = GFX_WRAP_REPEAT,
		.wrapT = GFX_WRAP_REPEAT,
		.wrapR = GFX_WRAP_REPEAT
	};

	GFXPropertyMap* mapA = gfx_pipe_process_get_map(pipeA->process, 1);
	GFXPropertyMap* mapB = gfx_pipe_process_get_map(pipeB->process, 1);
	gfx_property_map_forward_named(mapA, 0, 0, 0, GFX_FRAGMENT_SHADER, "tex");
	gfx_property_map_set_sampler(mapA, 0, 0, sampler);
	gfx_property_map_set_texture(mapA, 0, 0, tex);
	gfx_property_map_forward_named(mapB, 0, 0, 0, GFX_FRAGMENT_SHADER, "tex");
	gfx_property_map_set_sampler_share(mapB, 0, 0, mapA, 0, 0);
	gfx_property_map_set_texture(mapB, 0, 0, tex);


	/* Mesh and material */
	GFXMaterial* material = create_material();
	GFXMesh* mesh = create_mesh();


	/* Batch */
	GFXBatch* batch = gfx_batch_create(
		bucket->bucket, material, mesh, 0, 0, 1, 1);
	gfx_batch_set_level(
		batch, 0, 0, 0, 1);
	gfx_batch_set(
		batch, 0, 1, 1);


	/* Setup a loop */
	while(gfx_poll_events(), gfx_get_num_windows())
	{
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
	gfx_batch_free(batch);
	gfx_mesh_free(mesh);
	gfx_material_free(material);
	gfx_program_map_free(programMap);
	gfx_texture_free(tex);
	gfx_pipeline_free(pipeline);

	gfx_window_free(window1);
	gfx_window_free(window2);

	gfx_terminate();

	return 0;
}
