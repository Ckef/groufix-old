/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#define GL_GLEXT_PROTOTYPES
#include "groufix/core/renderer.h"

#include <string.h>


/********************************************************
 * GL core & GL ES emulators
 *******************************************************/

static void _gfx_gl_error_instanced_base_attributes(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_INSTANCED_BASE_ATTRIBUTES is incompatible with this context."
	);
}

static void _gfx_gl_error_program_map(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_PROGRAM_MAP is incompatible with this context."
	);
}

void APIENTRY _gfx_gl_bind_buffers_range(

		GLenum             target,
		GLuint             first,
		GLsizei            count,
		const GLuint*      buffers,
		const GLintptr*    offsets,
		const GLsizeiptr*  sizes)
{
	GFX_CONT_INIT_UNSAFE;

	GLsizei i;
	for(i = 0; i < count; ++i)
	{
		if(!buffers)
			GFX_REND_GET.BindBufferBase(target, first + i, 0);

		else GFX_REND_GET.BindBufferRange(
			target,
			first + i,
			buffers[i],
			offsets[i],
			sizes[i]);
	}
}

void APIENTRY _gfx_gl_bind_program_pipeline(

		GLuint pipeline)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.UseProgram(pipeline);
}

void APIENTRY _gfx_gl_bind_texture_unit(

		GLuint  unit,
		GLuint  texture)
{
	/* No-op */
}

void APIENTRY _gfx_gl_bind_vertex_buffer(

		GLuint    index,
		GLuint    buffer,
		GLintptr  offset,
		GLsizei   stride)
{
	/* No-op */
}

void APIENTRY _gfx_gl_buffer_storage(

		GLenum         target,
		GLsizeiptr     size,
		const GLvoid*  data,
		GLbitfield     flags)
{
	/* No-op */
}

void APIENTRY _gfx_gl_copy_named_buffer_sub_data(

		GLuint      readBuffer,
		GLuint      writeBuffer,
		GLintptr    readOffset,
		GLintptr    writeOffset,
		GLsizeiptr  size)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_REND_GET.BindBuffer(GL_COPY_READ_BUFFER, readBuffer);
	GFX_REND_GET.BindBuffer(GL_COPY_WRITE_BUFFER, writeBuffer);

	GFX_REND_GET.CopyBufferSubData(
		GL_COPY_READ_BUFFER,
		GL_COPY_WRITE_BUFFER,
		readOffset,
		writeOffset,
		size
	);
}

void APIENTRY _gfx_gl_create_buffers(

		GLsizei  n,
		GLuint*  buffers)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.GenBuffers(n, buffers);
}

void APIENTRY _gfx_gl_create_framebuffers(

		GLsizei  n,
		GLuint*  ids)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.GenFramebuffers(n, ids);
}

void APIENTRY _gfx_gl_create_program_pipelines(

		GLsizei  n,
		GLuint*  pipelines)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.GenProgramPipelines(n, pipelines);
}

void APIENTRY _gfx_gl_create_samplers(

		GLsizei  n,
		GLuint*  samplers)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.GenSamplers(n, samplers);
}

void APIENTRY _gfx_gl_create_textures(

		GLenum   target,
		GLsizei  n,
		GLuint*  textures)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.GenTextures(n, textures);
}

void APIENTRY _gfx_gl_create_vertex_arrays(

		GLsizei  n,
		GLuint*  arrays)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.GenVertexArrays(n, arrays);
}

void APIENTRY _gfx_gl_debug_message_callback(

		GFX_DEBUGPROC  callback,
		const GLvoid*  userParam)
{
	/* No-op */
}

void APIENTRY _gfx_gl_debug_message_control(

		GLenum         source,
		GLenum         type,
		GLenum         severity,
		GLsizei        count,
		const GLuint*  ids,
		GLboolean      enabled)
{
	/* No-op */
}

void APIENTRY _gfx_gl_delete_program_pipelines(

		GLsizei        n,
		const GLuint*  pipelines)
{
	_gfx_gl_error_program_map();
}

void APIENTRY _gfx_gl_disable_vertex_array_attrib(

		GLuint  vao,
		GLuint  index)
{
	GFX_CONT_INIT_UNSAFE;

	if(GFX_REND_GET.vao != vao)
	{
		GFX_REND_GET.vao = vao;
		GFX_REND_GET.BindVertexArray(vao);
	}

	GFX_REND_GET.DisableVertexAttribArray(index);
}

void APIENTRY _gfx_gl_draw_arrays_instanced_base_instance(

		GLenum   mode,
		GLint    first,
		GLsizei  count,
		GLsizei  primcount,
		GLuint   baseinstance)
{
	_gfx_gl_error_instanced_base_attributes();
}

void APIENTRY _gfx_gl_draw_elements_instanced_base_instance(

		GLenum         mode,
		GLsizei        count,
		GLenum         type,
		const GLvoid*  indices,
		GLsizei        primcount,
		GLuint         baseinstance)
{
	_gfx_gl_error_instanced_base_attributes();
}

void APIENTRY _gfx_gl_draw_elements_instanced_base_vertex_base_instance(

		GLenum         mode,
		GLsizei        count,
		GLenum         type,
		const GLvoid*  indices,
		GLsizei        primcount,
		GLint          basevertex,
		GLuint         baseinstance)
{
	_gfx_gl_error_instanced_base_attributes();
}

void APIENTRY _gfx_gl_enable_vertex_array_attrib(

		GLuint  vao,
		GLuint  index)
{
	GFX_CONT_INIT_UNSAFE;

	if(GFX_REND_GET.vao != vao)
	{
		GFX_REND_GET.vao = vao;
		GFX_REND_GET.BindVertexArray(vao);
	}

	GFX_REND_GET.EnableVertexAttribArray(index);
}

void APIENTRY _gfx_gl_generate_texture_mipmap(

		GLuint texture)
{
	/* No-op */
}

void APIENTRY _gfx_gl_gen_program_pipelines(

		GLsizei  n,
		GLuint*  pipelines)
{
	_gfx_gl_error_program_map();
}

void APIENTRY _gfx_gl_get_named_buffer_sub_data(

		GLuint      buffer,
		GLintptr    offset,
		GLsizeiptr  size,
		void*       data)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_REND_GET.BindBuffer(GL_ARRAY_BUFFER, buffer);
	GFX_REND_GET.GetBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void APIENTRY _gfx_gl_invalidate_buffer_sub_data(

		GLuint      buffer,
		GLintptr    offset,
		GLsizeiptr  length)
{
	GFX_CONT_INIT_UNSAFE;

	void* map = GFX_REND_GET.MapNamedBufferRange(
		buffer,
		offset,
		length,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT
	);

	if(map)
	{
		if(!GFX_REND_GET.UnmapNamedBuffer(buffer)) gfx_errors_push(
			GFX_ERROR_MEMORY_CORRUPTION,
			"Orphaning a buffer might have corrupted its memory."
		);
	}
}

void* APIENTRY _gfx_gl_map_named_buffer_range(

		GLuint      buffer,
		GLintptr    offset,
		GLsizeiptr  length,
		GLbitfield  access)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_REND_GET.BindBuffer(GL_ARRAY_BUFFER, buffer);
	return GFX_REND_GET.MapBufferRange(GL_ARRAY_BUFFER, offset, length, access);
}

void APIENTRY _gfx_gl_named_buffer_data(

		GLuint         buffer,
		GLsizeiptr     size,
		const GLvoid*  data,
		GLenum         usage)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_REND_GET.BindBuffer(GL_ARRAY_BUFFER, buffer);
	GFX_REND_GET.BufferData(GL_ARRAY_BUFFER, size, data, usage);
}

void APIENTRY _gfx_gl_named_buffer_storage(

		GLuint         buffer,
		GLsizeiptr     size,
		const GLvoid*  data,
		GLbitfield     flags)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_REND_GET.BindBuffer(GL_ARRAY_BUFFER, buffer);
	GFX_REND_GET.BufferStorage(GL_ARRAY_BUFFER, size, data, flags);
}

void APIENTRY _gfx_gl_named_buffer_sub_data(

		GLuint         buffer,
		GLintptr       offset,
		GLsizeiptr     size,
		const GLvoid*  data)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_REND_GET.BindBuffer(GL_ARRAY_BUFFER, buffer);
	GFX_REND_GET.BufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void APIENTRY _gfx_gl_named_framebuffer_draw_buffers(

		GLuint         framebuffer,
		GLsizei        n,
		const GLenum*  bufs)
{
	GFX_CONT_INIT_UNSAFE;

	if(GFX_REND_GET.fbos[0] != framebuffer)
	{
		GFX_REND_GET.fbos[0] = framebuffer;
		GFX_REND_GET.BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	}

	GFX_REND_GET.DrawBuffers(n, bufs);
}

void APIENTRY _gfx_gl_named_framebuffer_texture(

		GLuint  framebuffer,
		GLenum  attach,
		GLuint  texture,
		GLint   level)
{
	GFX_CONT_INIT_UNSAFE;

	if(GFX_REND_GET.fbos[0] != framebuffer)
	{
		GFX_REND_GET.fbos[0] = framebuffer;
		GFX_REND_GET.BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	}

	GFX_REND_GET.FramebufferTexture(
		GL_DRAW_FRAMEBUFFER,
		attach,
		texture,
		level
	);
}

void APIENTRY _gfx_gl_named_framebuffer_texture_layer(

		GLuint  framebuffer,
		GLenum  attach,
		GLuint  texture,
		GLint   level,
		GLint   layer)
{
	GFX_CONT_INIT_UNSAFE;

	if(GFX_REND_GET.fbos[0] != framebuffer)
	{
		GFX_REND_GET.fbos[0] = framebuffer;
		GFX_REND_GET.BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	}

	GFX_REND_GET.FramebufferTextureLayer(
		GL_DRAW_FRAMEBUFFER,
		attach,
		texture,
		level,
		layer
	);
}

void APIENTRY _gfx_gl_patch_parameter_i(

		GLenum  pname,
		GLint   value)
{
	/* No-op */
}

void APIENTRY _gfx_gl_program_uniform_1fv(

		GLuint          program,
		GLint           location,
		GLsizei         count,
		const GLfloat*  value)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.Uniform1fv(location, count, value);
}

void APIENTRY _gfx_gl_program_uniform_1iv(

		GLuint        program,
		GLint         location,
		GLsizei       count,
		const GLint*  value)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.Uniform1iv(location, count, value);
}

void APIENTRY _gfx_gl_program_uniform_1uiv(

		GLuint         program,
		GLint          location,
		GLsizei        count,
		const GLuint*  value)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.Uniform1uiv(location, count, value);
}

void APIENTRY _gfx_gl_program_uniform_2fv(

		GLuint          program,
		GLint           location,
		GLsizei         count,
		const GLfloat*  value)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.Uniform2fv(location, count, value);
}

void APIENTRY _gfx_gl_program_uniform_2iv(

		GLuint        program,
		GLint         location,
		GLsizei       count,
		const GLint*  value)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.Uniform2iv(location, count, value);
}

void APIENTRY _gfx_gl_program_uniform_2uiv(

		GLuint         program,
		GLint          location,
		GLsizei        count,
		const GLuint*  value)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.Uniform2uiv(location, count, value);
}

void APIENTRY _gfx_gl_program_uniform_3fv(

		GLuint          program,
		GLint           location,
		GLsizei         count,
		const GLfloat*  value)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.Uniform3fv(location, count, value);
}

void APIENTRY _gfx_gl_program_uniform_3iv(

		GLuint        program,
		GLint         location,
		GLsizei       count,
		const GLint*  value)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.Uniform3iv(location, count, value);
}

void APIENTRY _gfx_gl_program_uniform_3uiv(

		GLuint         program,
		GLint          location,
		GLsizei        count,
		const GLuint*  value)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.Uniform3uiv(location, count, value);
}

void APIENTRY _gfx_gl_program_uniform_4fv(

		GLuint          program,
		GLint           location,
		GLsizei         count,
		const GLfloat*  value)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.Uniform4fv(location, count, value);
}

void APIENTRY _gfx_gl_program_uniform_4iv(

		GLuint        program,
		GLint         location,
		GLsizei       count,
		const GLint*  value)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.Uniform4iv(location, count, value);
}

void APIENTRY _gfx_gl_program_uniform_4uiv(

		GLuint         program,
		GLint          location,
		GLsizei        count,
		const GLuint*  value)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.Uniform4uiv(location, count, value);
}

void APIENTRY _gfx_gl_program_uniform_matrix_2fv(

		GLuint          program,
		GLint           location,
		GLsizei         count,
		GLboolean       transpose,
		const GLfloat*  value)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.UniformMatrix2fv(location, count, transpose, value);
}

void APIENTRY _gfx_gl_program_uniform_matrix_3fv(

		GLuint          program,
		GLint           location,
		GLsizei         count,
		GLboolean       transpose,
		const GLfloat*  value)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.UniformMatrix3fv(location, count, transpose, value);
}

void APIENTRY _gfx_gl_program_uniform_matrix_4fv(

		GLuint          program,
		GLint           location,
		GLsizei         count,
		GLboolean       transpose,
		const GLfloat*  value)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.UniformMatrix4fv(location, count, transpose, value);
}

void APIENTRY _gfx_gl_texture_buffer(

		GLuint  texture,
		GLenum  format,
		GLuint  buffer)
{
	/* No-op */
}

void APIENTRY _gfx_gl_texture_parameter_f(

		GLuint   texture,
		GLenum   pname,
		GLfloat  param)
{
	/* No-op */
}

void APIENTRY _gfx_gl_texture_parameter_i(

		GLuint  texture,
		GLenum  pname,
		GLint   param)
{
	/* No-op */
}

void APIENTRY _gfx_gl_texture_storage_2d(

		GLuint   texture,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w,
		GLsizei  h)
{
	/* No-op */
}

void APIENTRY _gfx_gl_texture_storage_2d_multisample(

		GLuint     texture,
		GLsizei    samples,
		GLenum     internalFormat,
		GLsizei    w,
		GLsizei    h,
		GLboolean  f)
{
	/* No-op */
}

void APIENTRY _gfx_gl_texture_storage_3d(

		GLuint   texture,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w,
		GLsizei  h,
		GLsizei  d)
{
	/* No-op */
}

void APIENTRY _gfx_gl_texture_storage_3d_multisample(

		GLuint     texture,
		GLsizei    samples,
		GLenum     internalFormat,
		GLsizei    w,
		GLsizei    h,
		GLsizei    d,
		GLboolean  f)
{
	/* No-op */
}

void APIENTRY _gfx_gl_texture_sub_image_2d(

		GLuint         texture,
		GLint          level,
		GLint          xoffset,
		GLint          yoffset,
		GLsizei        w,
		GLsizei        h,
		GLenum         format,
		GLenum         type,
		const GLvoid*  pixels)
{
	/* No-op */
}

void APIENTRY _gfx_gl_texture_sub_image_3d(

		GLuint         texture,
		GLint          level,
		GLint          xoffset,
		GLint          yoffset,
		GLint          zoffset,
		GLsizei        w,
		GLsizei        h,
		GLsizei        d,
		GLenum         format,
		GLenum         type,
		const GLvoid*  pixels)
{
	/* No-op */
}

GLboolean APIENTRY _gfx_gl_unmap_named_buffer(

		GLuint buffer)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_REND_GET.BindBuffer(GL_ARRAY_BUFFER, buffer);
	return GFX_REND_GET.UnmapBuffer(GL_ARRAY_BUFFER);
}

void APIENTRY _gfx_gl_use_program_stages(

		GLuint      pipeline,
		GLbitfield  stages,
		GLuint      program)
{
	_gfx_gl_error_program_map();
}

void APIENTRY _gfx_gl_vertex_array_attrib_binding(

		GLuint  vao,
		GLuint  attrib,
		GLuint  binding)
{
	GFX_CONT_INIT_UNSAFE;

	if(GFX_REND_GET.vao != vao)
	{
		GFX_REND_GET.vao = vao;
		GFX_REND_GET.BindVertexArray(vao);
	}

	GFX_REND_GET.VertexAttribBinding(attrib, binding);
}

void APIENTRY _gfx_gl_vertex_array_attrib_format(

		GLuint     vao,
		GLuint     index,
		GLint      size,
		GLenum     type,
		GLboolean  normalized,
		GLuint     offset)
{
	GFX_CONT_INIT_UNSAFE;

	if(GFX_REND_GET.vao != vao)
	{
		GFX_REND_GET.vao = vao;
		GFX_REND_GET.BindVertexArray(vao);
	}

	GFX_REND_GET.VertexAttribFormat(index, size, type, normalized, offset);
}

void APIENTRY _gfx_gl_vertex_array_attrib_i_format(

		GLuint  vao,
		GLuint  index,
		GLint   size,
		GLenum  type,
		GLuint  offset)
{
	GFX_CONT_INIT_UNSAFE;

	if(GFX_REND_GET.vao != vao)
	{
		GFX_REND_GET.vao = vao;
		GFX_REND_GET.BindVertexArray(vao);
	}

	GFX_REND_GET.VertexAttribIFormat(index, size, type, offset);
}

void APIENTRY _gfx_gl_vertex_array_binding_divisor(

		GLuint  vao,
		GLuint  index,
		GLuint  divisor)
{
	GFX_CONT_INIT_UNSAFE;

	if(GFX_REND_GET.vao != vao)
	{
		GFX_REND_GET.vao = vao;
		GFX_REND_GET.BindVertexArray(vao);
	}

	GFX_REND_GET.VertexBindingDivisor(index, divisor);
}

void APIENTRY _gfx_gl_vertex_array_element_buffer(

		GLuint  vao,
		GLuint  buffer)
{
	GFX_CONT_INIT_UNSAFE;

	if(GFX_REND_GET.vao != vao)
	{
		GFX_REND_GET.vao = vao;
		GFX_REND_GET.BindVertexArray(vao);
	}

	GFX_REND_GET.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
}

void APIENTRY _gfx_gl_vertex_array_vertex_buffer(

		GLuint    vao,
		GLuint    index,
		GLuint    buffer,
		GLintptr  offset,
		GLsizei   stride)
{
	GFX_CONT_INIT_UNSAFE;

	if(GFX_REND_GET.vao != vao)
	{
		GFX_REND_GET.vao = vao;
		GFX_REND_GET.BindVertexArray(vao);
	}

	GFX_REND_GET.BindVertexBuffer(index, buffer, offset, stride);
}

void APIENTRY _gfx_gl_vertex_attrib_binding(

		GLuint  attrib,
		GLuint  binding)
{
	/* No-op */
}

void APIENTRY _gfx_gl_vertex_attrib_format(

		GLuint     index,
		GLint      size,
		GLenum     type,
		GLboolean  normalized,
		GLuint     offset)
{
	/* No-op */
}

void APIENTRY _gfx_gl_vertex_attrib_i_format(

		GLuint  index,
		GLint   size,
		GLenum  type,
		GLuint  offset)
{
	/* No-op */
}

void APIENTRY _gfx_gl_vertex_binding_divisor(

		GLuint  index,
		GLuint  divisor)
{
	/* No-op */
}


#if defined(GFX_GLES)

/********************************************************
 * GL ES emulators
 *******************************************************/

static void _gfx_gles_error_buffer_texture(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_BUFFER_TEXTURE is incompatible with this context."
	);
}

static void _gfx_gles_error_layered_multisample_texture(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE is incompatible with this context."
	);
}

static void _gfx_gles_error_multisample_texture(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_MULTISAMPLE_TEXTURE is incompatible with this context."
	);
}

static void _gfx_gles_error_vertex_base(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_VERTEX_BASE is incompatible with this context."
	);
}

void APIENTRY _gfx_gles_draw_elements_base_vertex(

		GLenum         mode,
		GLsizei        count,
		GLenum         type,
		const GLvoid*  indices,
		GLint          basevertex)
{
	_gfx_gles_error_vertex_base();
}

void APIENTRY _gfx_gles_draw_elements_instanced_base_vertex(

		GLenum         mode,
		GLsizei        count,
		GLenum         type,
		const GLvoid*  indices,
		GLsizei        primcount,
		GLint          basevertex)
{
	_gfx_gles_error_vertex_base();
}

void APIENTRY _gfx_gles_framebuffer_texture(

		GLenum  target,
		GLenum  attach,
		GLuint  texture,
		GLint   level)
{
	_gfx_gles_error_buffer_texture();
}

void APIENTRY _gfx_gles_get_buffer_sub_data(

		GLenum      target,
		GLintptr    offset,
		GLsizeiptr  size,
		GLvoid*     data)
{
	void* map = glMapBufferRange(
		target,
		offset,
		size,
		GL_MAP_READ_BIT
	);

	if(map)
	{
		memcpy(data, map, size);

		if(!glUnmapBuffer(target)) gfx_errors_push(
			GFX_ERROR_MEMORY_CORRUPTION,
			"Reading from a buffer might have corrupted its memory."
		);
	}
}

void APIENTRY _gfx_gles_named_framebuffer_texture_2d(

		GLuint  framebuffer,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	GFX_CONT_INIT_UNSAFE;

	if(GFX_REND_GET.fbos[0] != framebuffer)
	{
		GFX_REND_GET.fbos[0] = framebuffer;
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	}

	glFramebufferTexture2D(
		GL_DRAW_FRAMEBUFFER,
		attach,
		textarget,
		texture,
		level
	);
}

void APIENTRY _gfx_gles_polygon_mode(

		GLenum  face,
		GLenum  mode)
{
	/* No-op */
}

void APIENTRY _gfx_gles_tex_buffer(

		GLenum  target,
		GLenum  internalFormat,
		GLuint  buffer)
{
	_gfx_gles_error_buffer_texture();
}

void APIENTRY _gfx_gles_tex_image_2d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalformat,
		GLsizei    w,
		GLsizei    h,
		GLboolean  f)
{
	_gfx_gles_error_multisample_texture();
}

void APIENTRY _gfx_gles_tex_image_3d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalformat,
		GLsizei    w,
		GLsizei    h,
		GLsizei    d,
		GLboolean  f)
{
	_gfx_gles_error_layered_multisample_texture();
}

void APIENTRY _gfx_gles_tex_storage_2d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalFormat,
		GLsizei    w,
		GLsizei    h,
		GLboolean  f)
{
	_gfx_gles_error_multisample_texture();
}

void APIENTRY _gfx_gles_tex_storage_3d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalFormat,
		GLsizei    w,
		GLsizei    h,
		GLsizei    d,
		GLboolean  f)
{
	_gfx_gles_error_layered_multisample_texture();
}


#elif defined(GFX_GL)

/********************************************************
 * GL core emulators
 *******************************************************/

static void _gfx_gl_error_program_binary(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_PROGRAM_BINARY is incompatible with this context."
	);
}

void APIENTRY _gfx_gl_bind_sampler(

		GLuint  unit,
		GLuint  sampler)
{
	/* No-op */
}

void APIENTRY _gfx_gl_delete_samplers(

		GLsizei        n,
		const GLuint*  samplers)
{
	/* No-op */
}

void APIENTRY _gfx_gl_gen_samplers(

		GLsizei  n,
		GLuint*  samplers)
{
	/* No-op */
}

void APIENTRY _gfx_gl_get_program_binary(

		GLuint    program,
		GLsizei   bufsize,
		GLsizei*  length,
		GLenum*   binaryFormat,
		void*     binary)
{
	if(length) *length = 0;

	_gfx_gl_error_program_binary();
}

void APIENTRY _gfx_gl_named_framebuffer_texture_2d(

		GLuint  framebuffer,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.NamedFramebufferTexture(framebuffer, attach, texture, level);
}

void APIENTRY _gfx_gl_program_binary(

		GLuint         program,
		GLenum         binaryFormat,
		const GLvoid*  binary,
		GLsizei        length)
{
	_gfx_gl_error_program_binary();
}

void APIENTRY _gfx_gl_program_parameter_i(

		GLuint  program,
		GLenum  pname,
		GLint   value)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_PROGRAM_BINARY and GFX_EXT_PROGRAM_MAP are incompatible with this context."
	);
}

void APIENTRY _gfx_gl_sampler_parameter_f(

		GLuint   sampler,
		GLenum   pname,
		GLfloat  param)
{
	/* No-op */
}

void APIENTRY _gfx_gl_sampler_parameter_i(

		GLuint  sampler,
		GLenum  pname,
		GLint   param)
{
	/* No-op */
}

void APIENTRY _gfx_gl_tex_storage_2d(

		GLenum   target,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w,
		GLsizei  h)
{
	GFX_CONT_INIT_UNSAFE;

	GLenum first =
		(target == GL_TEXTURE_CUBE_MAP) ? GL_TEXTURE_CUBE_MAP_POSITIVE_X : target;
	GLenum last =
		(target == GL_TEXTURE_CUBE_MAP) ? GL_TEXTURE_CUBE_MAP_NEGATIVE_Z : target;

	GLsizei l;
	for(l = 0; l < levels; ++l)
	{
		GLenum face;
		for(face = first; face <= last; ++face)
			GFX_REND_GET.TexImage2D(face, l, internalFormat, w, h, 0, GL_RED, GL_BYTE, NULL);

		w >>= (w > 1) ? 1 : 0;
		h >>= (h > 1) ? 1 : 0;
	}
}

void APIENTRY _gfx_gl_tex_storage_2d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalFormat,
		GLsizei    w,
		GLsizei    h,
		GLboolean  f)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.TexImage2DMultisample(target, samples, internalFormat, w, h, f);
}

void APIENTRY _gfx_gl_tex_storage_3d(

		GLenum   target,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w,
		GLsizei  h,
		GLsizei  d)
{
	GFX_CONT_INIT_UNSAFE;

	GLsizei df =
		(target == GL_TEXTURE_3D || target == GL_PROXY_TEXTURE_3D) ? 1 : 0;

	GLsizei l;
	for(l = 0; l < levels; ++l)
	{
		GFX_REND_GET.TexImage3D(target, l, internalFormat, w, h, d, 0, GL_RED, GL_BYTE, NULL);
		w >>= (w > 1) ? 1 : 0;
		h >>= (h > 1) ? 1 : 0;
		d >>= (d > 1) ? df : 0;
	}
}

void APIENTRY _gfx_gl_tex_storage_3d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalFormat,
		GLsizei    w,
		GLsizei    h,
		GLsizei    d,
		GLboolean  f)
{
	GFX_CONT_INIT_UNSAFE;
	GFX_REND_GET.TexImage3DMultisample(target, samples, internalFormat, w, h, d, f);
}

void APIENTRY _gfx_gl_vertex_attrib_divisor(

		GLuint  index,
		GLuint  divisor)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_INSTANCED_ATTRIBUTES is incompatible with this context."
	);
}

#endif
