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

#include "groufix/core/renderer.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/** Internal renderer handle */
#if defined(GFX_RENDERER_GL)
typedef GLuint GFX_LayoutHandle;

#elif defined(GFX_RENDERER_VK)
typedef void* GFX_LayoutHandle;

#endif


/** Object flags associated with all layouts */
#if defined(GFX_RENDERER_GL)
const GFXRenderObjectFlags GFX_VERTEX_LAYOUT_OBJECT_FLAGS = 0;

#elif defined(GFX_RENDERER_VK)
const GFXRenderObjectFlags GFX_VERTEX_LAYOUT_OBJECT_FLAGS = 0;

#endif


/** Internal Layout */
typedef struct GFX_Layout
{
	/* Super class */
	GFXVertexLayout layout;

	/* Hidden data */
	GFX_RenderObjectID  id;
	GFX_LayoutHandle    handle;

} GFX_Layout;


/** Internal vertex buffer */
typedef struct GFX_Buffer
{
	GFXBuffer*     buffer;
	unsigned char  index; /* Index of the handle at the buffer */

	size_t         offset;
	size_t         stride;
	unsigned int   divisor;

} GFX_Buffer;


/******************************************************/
static inline GFX_Buffer* _gfx_layout_get_buffer(

		const GFX_Layout*  layout,
		unsigned char      index)
{
	return ((GFX_Buffer*)(layout + 1)) + index;
}

/******************************************************/
static inline GFXVertexAttribute* _gfx_layout_get_attribute(

		const GFX_Layout*  layout,
		unsigned char      index)
{
	void* attr = _gfx_layout_get_buffer(layout, layout->layout.buffers);
	return ((GFXVertexAttribute*)attr) + index;
}

/******************************************************/
static inline GFXVertexSource* _gfx_layout_get_source(

		const GFX_Layout*  layout,
		unsigned char      index)
{
	void* src = _gfx_layout_get_attribute(layout, layout->layout.attributes);
	return ((GFXVertexSource*)src) + index;
}

/******************************************************/
static inline int _gfx_layout_ref(

		const GFX_Layout* layout,
		GFX_CONT_ARG)
{
	return _gfx_render_object_id_reference(
		&((GFX_Layout*)layout)->id,
		GFX_VERTEX_LAYOUT_OBJECT_FLAGS,
		&GFX_CONT_GET.objects
	);
}

/******************************************************/
static inline int _gfx_layout_check(

		const GFX_Layout* layout,
		GFX_CONT_ARG)
{
	return !layout->handle ||
		(!GFX_CONT_EQ(NULL) && _gfx_layout_ref(layout, GFX_CONT_AS_ARG));
}

/******************************************************/
static void _gfx_layout_clear(

		GFX_Layout* layout,
		GFX_CONT_ARG)
{
#if defined(GFX_RENDERER_GL)

	GFX_REND_GET.DeleteVertexArrays(1, &layout->handle);

#endif

	memset(&layout->handle, 0, sizeof(GFX_LayoutHandle));
}

/******************************************************/
static int _gfx_layout_init(

		GFX_Layout* layout,
		GFX_CONT_ARG)
{
#if defined(GFX_RENDERER_GL)

	/* Allocate layout */
	GFX_REND_GET.CreateVertexArrays(1, &layout->handle);
	if(!layout->handle)
	{
		_gfx_layout_clear(layout, GFX_CONT_AS_ARG);
		return 0;
	}

#endif

	return 1;
}

/******************************************************/
static void _gfx_layout_obj_destruct(

		GFX_RenderObjectIDArg arg)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_Layout* layout = GFX_PTR_SUB_BYTES(arg, offsetof(GFX_Layout, id));
	_gfx_layout_clear(layout, GFX_CONT_AS_ARG);
}

/******************************************************/
static void _gfx_layout_obj_prepare(

		GFX_RenderObjectIDArg  arg,
		void**                 temp,
		int                    shared)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_Layout* layout = GFX_PTR_SUB_BYTES(arg, offsetof(GFX_Layout, id));
	_gfx_layout_clear(layout, GFX_CONT_AS_ARG);
}

/******************************************************/
static void _gfx_layout_obj_transfer(

		GFX_RenderObjectIDArg  arg,
		void**                 temp,
		int                    shared)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_Layout* layout = GFX_PTR_SUB_BYTES(arg, offsetof(GFX_Layout, id));
	_gfx_layout_init(layout, GFX_CONT_AS_ARG);
}

/******************************************************/
/** vtable for render object part of the layout */
static const GFX_RenderObjectFuncs _gfx_layout_obj_funcs =
{
	.destruct = _gfx_layout_obj_destruct,
	.prepare  = _gfx_layout_obj_prepare,
	.transfer = _gfx_layout_obj_transfer
};

/******************************************************/
GFXVertexLayout* gfx_vertex_layout_create(

		unsigned char  buffers,
		unsigned char  attributes,
		unsigned char  sources)
{
	/* Yeah, okay... no. */
	if(!attributes || !sources) return NULL;

	/* Create new layout, append buffers, attributes and sources at the end of the struct */
	size_t alloc =
		sizeof(GFX_Layout) +
		sizeof(GFX_Buffer) * buffers +
		sizeof(GFXVertexAttribute) * attributes +
		sizeof(GFXVertexSource) * sources;

	GFX_Layout* layout = calloc(1, alloc);
	if(!layout)
	{
		/* Out of memory error */
		gfx_errors_output(
			"[GFX Out of Memory]: Vertex Layout could not be allocated."
		);
		return NULL;
	}

	/* Initialize the layout */
	layout->layout.attributes = attributes;
	layout->layout.sources = sources;

	/* Initialize as object */
	if(!_gfx_render_object_id_init(
		&layout->id,
		1,
		GFX_VERTEX_LAYOUT_OBJECT_FLAGS,
		&_gfx_layout_obj_funcs,
		NULL))
	{
		free(layout);
		return NULL;
	}

	return (GFXVertexLayout*)layout;
}

/******************************************************/
void gfx_vertex_layout_free(

		GFXVertexLayout* layout)
{
	if(layout)
	{
		/* Check context */
		GFX_CONT_INIT_UNSAFE;

		if(!_gfx_layout_check((GFX_Layout*)layout, GFX_CONT_AS_ARG))
			return;

		/* Clear as object */
		/* Object clearing will call the destruct callback */
		_gfx_render_object_id_clear(&((GFX_Layout*)layout)->id);
		free(layout);
	}
}













































#include <stdlib.h>
#include <string.h>

/******************************************************/
/** Internal Vertex layout */
typedef struct GFX_Layout
{
	/* Super class */
	GFXVertexLayout layout;

	/* Hidden data */
	GFX_RenderObjectID  id;
	unsigned int        references;  /* Reference counter */
	GLuint              vao;         /* OpenGL handle */
	unsigned int        blocks;      /* Total number of blocks */

	GFXVector           attributes;  /* Stores GFX_Attribute */
	GFXVector           buffers;     /* Stores GFX_Buffer */
	GLuint              indexBuffer;
	size_t              indexOffset; /* Byte offset into index buffer */

} GFX_Layout;


/** Internal vertex source */
typedef struct GFX_Source
{
	GFXVertexSource  source;
	unsigned int     blocks; /* Number of times blocked */

} GFX_Source;


/** Internal vertex attribute */
typedef struct GFX_Attribute
{
	unsigned char     size;
	GLenum            type;
	GFXInterpretType  interpret;

	unsigned int      buffer; /* Vertex buffer index */
	GLuint            offset; /* Offset within the buffer */

} GFX_Attribute;


/** Internal vertex buffer binding */
typedef struct GFX_Buffer
{
	GLuint    buffer; /* 0 when empty */
	GLintptr  offset; /* Base offset for all vertex attributes */
	GLintptr  stride; /* Stride for all vertex attributes */
	GLuint    divisor;

} GFX_Buffer;


/******************************************************/
static void _gfx_layout_init_attrib(

		const GFX_Layout*     layout,
		unsigned int          index,
		const GFX_Attribute*  attr,
		GFX_CONT_ARG)
{
	if(GFX_REND_GET.intExt[GFX_INT_EXT_VERTEX_ATTRIB_BINDING])
	{
		/* Check if enabled */
		if(attr->size)
		{
			/* Set the attribute */
			GFX_REND_GET.EnableVertexArrayAttrib(
				layout->vao, index);
			GFX_REND_GET.VertexArrayAttribBinding(
				layout->vao, index, attr->buffer);

			/* Check integer value */
			if(attr->interpret & GFX_INTERPRET_INTEGER)
			{
				GFX_REND_GET.VertexArrayAttribIFormat(
					layout->vao,
					index,
					attr->size,
					attr->type,
					attr->offset);
			}
			else
			{
				GLboolean norm = attr->interpret & GFX_INTERPRET_NORMALIZED ?
					GL_TRUE : GL_FALSE;

				GFX_REND_GET.VertexArrayAttribFormat(
					layout->vao,
					index,
					attr->size,
					attr->type,
					norm,
					attr->offset);
			}

			/* Done */
			return;
		}
	}

	/* Fetch buffer if no separate buffers */
	else if(attr->buffer < gfx_vector_get_size(&layout->buffers))
	{
		GFX_Buffer* buff =
			gfx_vector_at(&layout->buffers, attr->buffer);

		/* Check if enabled */
		if(attr->size && buff->buffer)
		{
			/* Set the attribute */
			_gfx_gl_vertex_layout_bind(layout->vao, GFX_CONT_AS_ARG);
			GFX_REND_GET.BindBuffer(GL_ARRAY_BUFFER, buff->buffer);
			GFX_REND_GET.EnableVertexAttribArray(index);

			GLvoid* ptr = (GLvoid*)(buff->offset + attr->offset);

			/* Check integer value */
			if(attr->interpret & GFX_INTERPRET_INTEGER)
			{
				GFX_REND_GET.VertexAttribIPointer(
					index,
					attr->size,
					attr->type,
					buff->stride,
					ptr);
			}
			else
			{
				GLboolean norm = attr->interpret & GFX_INTERPRET_NORMALIZED ?
					GL_TRUE : GL_FALSE;

				GFX_REND_GET.VertexAttribPointer(
					index,
					attr->size,
					attr->type,
					norm,
					buff->stride,
					ptr);
			}

			/* Done */
			return;
		}
	}

	/* Disable it */
	GFX_REND_GET.DisableVertexArrayAttrib(layout->vao, index);
}

/******************************************************/
static void _gfx_layout_init_buff_divisor(

		const GFX_Layout*  layout,
		const GFX_Buffer*  buff,
		GFX_CONT_ARG)
{
	size_t index = gfx_vector_get_index(
			&layout->buffers, (const GFXVectorIterator)buff);
	size_t ind = gfx_vector_get_size(
			&layout->attributes);

	/* Iterate over all attributes and set divisors */
	while(ind--)
	{
		const GFX_Attribute* attr =
			gfx_vector_at(&layout->attributes, ind);

		if(attr->buffer == index)
		{
			_gfx_gl_vertex_layout_bind(
				layout->vao,
				GFX_CONT_AS_ARG);

			GFX_REND_GET.VertexAttribDivisor(
				ind,
				buff->divisor);
		}
	}
}

/******************************************************/
static int _gfx_layout_alloc_attribute(

		GFX_Layout*   layout,
		unsigned int  index,
		GFX_CONT_ARG)
{
	/* Check index */
	if(index >= GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_ATTRIBS]) return 0;
	size_t size = gfx_vector_get_size(&layout->attributes);

	if(index >= size)
	{
		/* Allocate enough memory */
		size = index + 1 - size;

		GFXVectorIterator it = gfx_vector_insert_range(
			&layout->attributes,
			size,
			NULL,
			layout->attributes.end
		);

		if(it == layout->attributes.end) return 0;

		/* Initialize to 0 to indicate empty attributes */
		memset(it, 0, sizeof(GFX_Attribute) * size);
	}

	return 1;
}

/******************************************************/
static int _gfx_layout_alloc_buffer(

		GFX_Layout*   layout,
		unsigned int  index,
		GFX_CONT_ARG)
{
	/* Check index */
	if(index >= GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_BUFFERS]) return 0;
	size_t size = gfx_vector_get_size(&layout->buffers);

	if(index >= size)
	{
		/* Allocate enough memory */
		size = index + 1 - size;

		GFXVectorIterator it = gfx_vector_insert_range(
			&layout->buffers,
			size,
			NULL,
			layout->buffers.end
		);

		if(it == layout->buffers.end) return 0;

		/* Initialize to 0 to indicate it is empty */
		memset(it, 0, sizeof(GFX_Buffer) * size);
	}

	return 1;
}

/******************************************************/
static int _gfx_layout_set_vertex_buffer(

		GFXVertexLayout*  layout,
		unsigned int      index,
		GLuint            buffer,
		size_t            offset,
		size_t            stride,
		GFX_CONT_ARG)
{
	if(stride > GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_STRIDE])
		return 0;

	GFX_Layout* internal = (GFX_Layout*)layout;
	if(!_gfx_layout_alloc_buffer(internal, index, GFX_CONT_AS_ARG))
		return 0;

	/* Set buffer */
	GFX_Buffer* set =
		gfx_vector_at(&internal->buffers, index);

	set->buffer = buffer;
	set->offset = offset;
	set->stride = stride;

	/* Initialize the buffer */
	if(GFX_REND_GET.intExt[GFX_INT_EXT_VERTEX_ATTRIB_BINDING])
	{
		GFX_REND_GET.VertexArrayVertexBuffer(
			internal->vao,
			index,
			buffer,
			offset,
			stride
		);
	}
	else
	{
		/* Iterate over all attributes and init them */
		size_t ind = gfx_vector_get_size(&internal->attributes);
		while(ind--)
		{
			const GFX_Attribute* attr =
				gfx_vector_at(&internal->attributes, ind);

			if(attr->buffer == index) _gfx_layout_init_attrib(
				internal,
				ind,
				attr,
				GFX_CONT_AS_ARG
			);
		}
	}

	return 1;
}

/******************************************************/
static int _gfx_layout_set_index_buffer(

		GFXVertexLayout*  layout,
		GLuint            buffer,
		size_t            offset)
{
	GFX_CONT_INIT(0);

	GFX_Layout* internal = (GFX_Layout*)layout;

	/* Check blocks of any source */
	if(internal->blocks) return 0;

	/* Set index buffer */
	internal->indexBuffer = buffer;
	internal->indexOffset = buffer ? offset : 0;

	/* Attach as index buffer to the layout */
	GFX_REND_GET.VertexArrayElementBuffer(
		internal->vao,
		buffer
	);

	return 1;
}

/******************************************************/
static void _gfx_layout_obj_free(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_Layout* layout = (GFX_Layout*)object;

	layout->id = id;
	layout->vao = 0;
}

/******************************************************/
static void _gfx_layout_obj_save(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_Layout* layout = (GFX_Layout*)object;

	/* Just don't clear the attribute or buffer vector */
	layout->id = id;
	GFX_REND_GET.DeleteVertexArrays(1, &layout->vao);
	layout->vao = 0;
}

/******************************************************/
static void _gfx_layout_obj_restore(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_Layout* layout = (GFX_Layout*)object;

	/* Create VAO */
	layout->id = id;
	GFX_REND_GET.CreateVertexArrays(1, &layout->vao);

	/* Restore index buffer */
	GFX_REND_GET.VertexArrayElementBuffer(
		layout->vao,
		layout->indexBuffer
	);

	/* Restore attributes */
	size_t index = gfx_vector_get_size(&layout->attributes);
	while(index--) _gfx_layout_init_attrib(
		layout,
		index,
		gfx_vector_at(&layout->attributes, index),
		GFX_CONT_AS_ARG
	);

	/* Restore buffers */
	index = gfx_vector_get_size(&layout->buffers);
	while(index--)
	{
		GFX_Buffer* buff = gfx_vector_at(
			&layout->buffers, index);

		if(GFX_REND_GET.intExt[GFX_INT_EXT_VERTEX_ATTRIB_BINDING])
		{
			GFX_REND_GET.VertexArrayVertexBuffer(
				layout->vao,
				index,
				buff->buffer,
				buff->offset,
				buff->stride);

			GFX_REND_GET.VertexArrayBindingDivisor(
				layout->vao,
				index,
				buff->divisor);
		}

		else
		{
			/* Not needed to restore buffers as attributes are already restored */
			_gfx_layout_init_buff_divisor(
				layout, buff, GFX_CONT_AS_ARG);
		}
	}
}

/******************************************************/
/* vtable for render object part of the layout */
static GFX_RenderObjectFuncs _gfx_layout_obj_funcs =
{
	_gfx_layout_obj_free,
	_gfx_layout_obj_save,
	_gfx_layout_obj_restore
};

/******************************************************/
GLuint _gfx_gl_vertex_layout_get_handle(

		const GFXVertexLayout* layout)
{
	return ((const GFX_Layout*)layout)->vao;
}

/******************************************************/
GLuint _gfx_gl_vertex_layout_get_index_buffer(

		const GFXVertexLayout*  layout,
		size_t*                 offset)
{
	const GFX_Layout* lay = (const GFX_Layout*)layout;
	*offset = lay->indexOffset;

	return lay->indexBuffer;
}

/******************************************************/
int _gfx_vertex_layout_block(

		GFXVertexLayout*  layout,
		unsigned char     index)
{
	/* Check index */
	if(index >= layout->sources) return 0;
	GFX_Layout* internal = (GFX_Layout*)layout;

	/* Check for overflow */
	if(!(internal->blocks + 1))
	{
		/* Overflow error */
		gfx_errors_push(
			GFX_ERROR_OVERFLOW,
			"Overflow occurred during Vertex Layout usage."
		);
		return 0;
	}

	/* Get source and increase block count */
	GFX_Source* source = ((GFX_Source*)(internal + 1)) + index;

	++internal->blocks;
	++source->blocks;

	return 1;
}

/******************************************************/
void _gfx_vertex_layout_unblock(

		GFXVertexLayout*  layout,
		unsigned char     index)
{
	/* Check index */
	if(index < layout->sources)
	{
		GFX_Layout* internal = (GFX_Layout*)layout;
		GFX_Source* source = ((GFX_Source*)(internal + 1)) + index;

		internal->blocks =
			internal->blocks ? internal->blocks - 1 : 0;
		source->blocks =
			source->blocks ? source->blocks - 1 : 0;
	}
}

/******************************************************/
GFXVertexLayout* gfx_vertex_layout_create(

		unsigned char sources)
{
	GFX_CONT_INIT(NULL);

	if(!sources) return NULL;

	/* Create new layout, append sources to end of struct */
	size_t size = sizeof(GFX_Layout) + sources * sizeof(GFX_Source);

	GFX_Layout* layout = calloc(1, size);
	if(!layout)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Vertex Layout could not be allocated."
		);
		return NULL;
	}

	/* Register as object */
	layout->id = _gfx_render_object_register(
		&GFX_CONT_GET.objects,
		layout,
		&_gfx_layout_obj_funcs
	);

	if(!layout->id.id)
	{
		free(layout);
		return NULL;
	}

	/* Initialize */
	layout->references = 1;
	layout->layout.sources = sources;
	GFX_REND_GET.CreateVertexArrays(1, &layout->vao);

	gfx_vector_init(&layout->attributes, sizeof(GFX_Attribute));
	gfx_vector_init(&layout->buffers, sizeof(GFX_Buffer));

	return (GFXVertexLayout*)layout;
}

/******************************************************/
int gfx_vertex_layout_share(

		GFXVertexLayout* layout)
{
	GFX_Layout* internal = (GFX_Layout*)layout;

	if(!(internal->references + 1))
	{
		/* Overflow error */
		gfx_errors_push(
			GFX_ERROR_OVERFLOW,
			"Overflow occurred during Vertex Layout referencing."
		);
		return 0;
	}

	++internal->references;
	return 1;
}

/******************************************************/
void gfx_vertex_layout_free(

		GFXVertexLayout* layout)
{
	if(layout)
	{
		GFX_Layout* internal = (GFX_Layout*)layout;

		/* Check references */
		if(!(--internal->references))
		{
			GFX_CONT_INIT_UNSAFE;

			/* Unregister as object */
			_gfx_render_object_unregister(internal->id);

			if(!GFX_CONT_EQ(NULL))
			{
				/* Delete VAO */
				if(GFX_REND_GET.vao == internal->vao)
					GFX_REND_GET.vao = 0;

				GFX_REND_GET.DeleteVertexArrays(1, &internal->vao);
			}

			/* Clear resources */
			gfx_vector_clear(&internal->attributes);
			gfx_vector_clear(&internal->buffers);

			free(layout);
		}
	}
}

/******************************************************/
int gfx_vertex_layout_set_attribute(

		GFXVertexLayout*           layout,
		unsigned int               index,
		const GFXVertexAttribute*  attr,
		unsigned int               buffer)
{
	GFX_CONT_INIT(0);

	if(
		!attr->size ||
		attr->type.unpacked == GFX_BIT ||
		attr->type.unpacked == GFX_NIBBLE ||
		attr->offset > GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_ATTRIB_OFFSET] ||
		buffer >= GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_BUFFERS])
	{
		return 0;
	}

	GFX_Layout* internal = (GFX_Layout*)layout;
	if(!_gfx_layout_alloc_attribute(internal, index, GFX_CONT_AS_ARG))
		return 0;

	/* Set attribute */
	GFX_Attribute* set =
		gfx_vector_at(&internal->attributes, index);

	int packed = _gfx_is_data_type_packed(attr->type);

	set->size      = attr->size;
	set->type      = packed ? attr->type.packed : attr->type.unpacked;
	set->interpret = attr->interpret;

	set->buffer    = buffer;
	set->offset    = attr->offset;

	/* Resolve how to interpret */
	set->interpret = (set->interpret & GFX_INTERPRET_DEPTH) ?
		GFX_INTERPRET_FLOAT : set->interpret;
	set->interpret = (set->interpret & GFX_INTERPRET_STENCIL) ?
		GFX_INTERPRET_INTEGER : set->interpret;
	set->interpret = packed && (set->interpret & GFX_INTERPRET_INTEGER) ?
		GFX_INTERPRET_FLOAT : set->interpret;

	/* Initialize attribute */
	_gfx_layout_init_attrib(internal, index, set, GFX_CONT_AS_ARG);

	return 1;
}

/******************************************************/
int gfx_vertex_layout_set_attribute_buffer(

		GFXVertexLayout*  layout,
		unsigned int      index,
		unsigned int      buffer)
{
	GFX_CONT_INIT(0);

	GFX_Layout* internal = (GFX_Layout*)layout;
	if(
		index >= gfx_vector_get_size(&internal->attributes) ||
		buffer >= GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_BUFFERS])
	{
		return 0;
	}

	/* Set attribute buffer */
	GFX_Attribute* set = gfx_vector_at(
		&internal->attributes,
		index
	);

	set->buffer = buffer;

	/* Skip the entire routine if separate buffers are available */
	if(GFX_REND_GET.intExt[GFX_INT_EXT_VERTEX_ATTRIB_BINDING])
		GFX_REND_GET.VertexArrayAttribBinding(
			internal->vao,
			index,
			buffer
		);

	else _gfx_layout_init_attrib(
		internal,
		index,
		set,
		GFX_CONT_AS_ARG
	);

	return 1;
}

/******************************************************/
int gfx_vertex_layout_set_source(

		GFXVertexLayout*        layout,
		unsigned char           index,
		const GFXVertexSource*  source)
{
	GFX_CONT_INIT(0);

	/* Check index */
	if(index >= layout->sources) return 0;
	GFX_Layout* internal = (GFX_Layout*)layout;

	/* Check extensions and bounds */
	if(
		source->primitive == GFX_PATCHES && (
		!GFX_CONT_GET.ext[GFX_EXT_TESSELLATION_SHADER] ||
		source->patchSize > GFX_CONT_GET.lim[GFX_LIM_MAX_PATCH_VERTICES]))
	{
		return 0;
	}

	/* Check blocks at source */
	GFX_Source* set = ((GFX_Source*)(internal + 1)) + index;
	if(set->blocks) return 0;

	/* Set the source */
	set->source = *source;
	if(source->primitive != GFX_PATCHES)
		set->source.patchSize = 0;

	return 1;
}

/******************************************************/
int gfx_vertex_layout_get_source(

		const GFXVertexLayout*  layout,
		unsigned char           index,
		GFXVertexSource*        source)
{
	/* Validate index */
	if(index >= layout->sources) return 0;

	/* Retrieve data */
	const GFX_Layout* internal = (const GFX_Layout*)layout;
	*source = ((const GFX_Source*)(internal + 1))[index].source;

	return 1;
}

/******************************************************/
int gfx_vertex_layout_set_vertex_buffer(

		GFXVertexLayout*  layout,
		unsigned int      index,
		const GFXBuffer*  buffer,
		size_t            offset,
		size_t            stride)
{
	GFX_CONT_INIT(0);

	GLuint buff = 0;
	if(buffer) buff = _gfx_gl_buffer_get_handle(buffer);

	return _gfx_layout_set_vertex_buffer(
		layout,
		index,
		buff,
		offset,
		stride,
		GFX_CONT_AS_ARG
	);
}

/******************************************************/
int gfx_vertex_layout_set_shared_vertex_buffer(

		GFXVertexLayout*        layout,
		unsigned int            index,
		const GFXSharedBuffer*  buffer,
		size_t                  offset,
		size_t                  stride)
{
	GFX_CONT_INIT(0);

	GLuint buff = 0;
	if(buffer)
	{
		buff = _gfx_gl_shared_buffer_get_handle(buffer);
		offset += buffer->offset;
	}

	return _gfx_layout_set_vertex_buffer(
		layout,
		index,
		buff,
		offset,
		stride,
		GFX_CONT_AS_ARG
	);
}

/******************************************************/
int gfx_vertex_layout_set_vertex_divisor(

		GFXVertexLayout*  layout,
		unsigned int      index,
		unsigned int      divisor)
{
	GFX_CONT_INIT(0);

	if(!GFX_CONT_GET.ext[GFX_EXT_INSTANCED_ATTRIBUTES])
		return 0;

	GFX_Layout* internal = (GFX_Layout*)layout;
	if(!_gfx_layout_alloc_buffer(internal, index, GFX_CONT_AS_ARG))
		return 0;

	/* Set divisor */
	GFX_Buffer* set = gfx_vector_at(&internal->buffers, index);
	set->divisor = divisor;

	/* Initialize the buffer divisor */
	if(GFX_REND_GET.intExt[GFX_INT_EXT_VERTEX_ATTRIB_BINDING])
		GFX_REND_GET.VertexArrayBindingDivisor(
			internal->vao,
			index,
			divisor
		);

	else _gfx_layout_init_buff_divisor(
		internal,
		set,
		GFX_CONT_AS_ARG
	);

	return 1;
}

/******************************************************/
int gfx_vertex_layout_set_index_buffer(

		GFXVertexLayout*  layout,
		const GFXBuffer*  buffer,
		size_t            offset)
{
	GLuint buff = 0;
	if(buffer) buff = _gfx_gl_buffer_get_handle(buffer);

	return _gfx_layout_set_index_buffer(layout, buff, offset);
}

/******************************************************/
int gfx_vertex_layout_set_shared_index_buffer(

		GFXVertexLayout*        layout,
		const GFXSharedBuffer*  buffer,
		size_t                  offset)
{
	GLuint buff = 0;
	if(buffer)
	{
		buff = _gfx_gl_shared_buffer_get_handle(buffer);
		offset += buffer->offset;
	}

	return _gfx_layout_set_index_buffer(layout, buff, offset);
}
