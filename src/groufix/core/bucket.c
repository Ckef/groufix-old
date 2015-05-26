/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/internal.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


/* Internal bucket flags */
#define GFX_INT_BUCKET_PROCESS_UNITS  0x01
#define GFX_INT_BUCKET_SORT           0x02

/* Internal unit state and action (for processing) */
#define GFX_INT_UNIT_VISIBLE     (1 << (GFX_UNIT_STATE_MAX_BITS +1))
#define GFX_INT_UNIT_ERASE       (1 << (GFX_UNIT_STATE_MAX_BITS +0))
#define GFX_INT_UNIT_MANUAL_MSB  (1 << (GFX_UNIT_STATE_MAX_BITS -1))
#define GFX_INT_UNIT_MANUAL      (~((~(GFX_INT_UNIT_MANUAL_MSB << 1)) + 1))

/* Internal draw function types */
#define GFX_INT_DRAW                             0
#define GFX_INT_DRAW_INSTANCED                   1
#define GFX_INT_DRAW_INSTANCED_BASE              2
#define GFX_INT_DRAW_VERTEX_BASE                 3
#define GFX_INT_DRAW_INSTANCED_VERTEX_BASE       4
#define GFX_INT_DRAW_INSTANCED_BASE_VERTEX_BASE  5

#define GFX_INT_DRAW_COUNT                       6


/******************************************************/
/* Internal draw function */
typedef void (*GFX_DrawFunc)(

		const GFXVertexSource*,
		size_t,
		unsigned int,
		unsigned int,
		unsigned int,
		GFX_WIND_ARG);


/* Internal bucket */
typedef struct GFX_Bucket
{
	/* Super class */
	GFXBucket bucket;

	/* Hidden data */
	unsigned char      flags;
	GFXVectorIterator  visible;      /* Everything after is not visible in units */

	GFXVector          refs;         /* Stores GFX_Ref */
	GFXVector          sources;      /* Stores GFX_Source */
	GFXVector          units;        /* Stores GFX_Unit */

	GFXDeque           emptyRefs;    /* Stores GFXBucketUnit, empty reference IDs */
	GFXDeque           emptySources; /* Stores GFXBucketSource, empty source IDs */

} GFX_Bucket;


/* Internal reference of a unit */
typedef struct GFX_Ref
{
	unsigned int           unit; /* units[unit - 1] = unit, 0 when empty */
	unsigned char          type; /* Drawing type */

	GFXBucketSource        src;  /* Source of the bucket to use (ID - 1), sources[src] = source */
	const GFXPropertyMap*  map;
	unsigned int           copy; /* Copy of the property map to use */

	size_t                 instances;
	unsigned int           instanceBase;
	unsigned int           vertexBase;
	unsigned int           indexBase;

} GFX_Ref;


/* Internal source */
typedef struct GFX_Source
{
	GFXVertexLayout*  layout;  /* NULL when empty */
	unsigned char     index;   /* Source index at the layout */
	GFXVertexSource   source;  /* If indexed, source.indexed will be GFX_INT_DRAW_COUNT, 0 otherwise */

} GFX_Source;


/* Internal render unit (actually sorted on) */
typedef struct GFX_Unit
{
	GFXBucketUnit  ref;      /* Reference, units[refs[ref] - 1] = this (const, equal to ID - 1) */
	GFXUnitState   state;    /* Combination of unit state, action and manual state */
	GLuint         program;  /* Program or program map to sort on */
	GLuint         vao;      /* layout to sort on */

} GFX_Unit;


/******************************************************/
static void _gfx_draw(

		const GFXVertexSource*  src,
		size_t                  inst,
		unsigned int            instBase,
		unsigned int            vertBase,
		unsigned int            indBase,
		GFX_WIND_ARG)
{
	GFX_REND_GET.DrawArrays(
		src->primitive,
		src->first + vertBase,
		src->count
	);
}

/******************************************************/
static void _gfx_draw_instanced(

		const GFXVertexSource*  src,
		size_t                  inst,
		unsigned int            instBase,
		unsigned int            vertBase,
		unsigned int            indBase,
		GFX_WIND_ARG)
{
	GFX_REND_GET.DrawArraysInstanced(
		src->primitive,
		src->first + vertBase,
		src->count,
		inst
	);
}

/******************************************************/
static void _gfx_draw_instanced_base(

		const GFXVertexSource*  src,
		size_t                  inst,
		unsigned int            instBase,
		unsigned int            vertBase,
		unsigned int            indBase,
		GFX_WIND_ARG)
{
	GFX_REND_GET.DrawArraysInstancedBaseInstance(
		src->primitive,
		src->first + vertBase,
		src->count,
		inst,
		instBase
	);
}

/******************************************************/
static void _gfx_draw_indexed(

		const GFXVertexSource*  src,
		size_t                  inst,
		unsigned int            instBase,
		unsigned int            vertBase,
		unsigned int            indBase,
		GFX_WIND_ARG)
{
	GFX_REND_GET.DrawElements(
		src->primitive,
		src->count,
		src->indexType,
		(GLvoid*)(src->first + indBase)
	);
}

/******************************************************/
static void _gfx_draw_indexed_instanced(

		const GFXVertexSource*  src,
		size_t                  inst,
		unsigned int            instBase,
		unsigned int            vertBase,
		unsigned int            indBase,
		GFX_WIND_ARG)
{
	GFX_REND_GET.DrawElementsInstanced(
		src->primitive,
		src->count,
		src->indexType,
		(GLvoid*)(src->first + indBase),
		inst
	);
}

/******************************************************/
static void _gfx_draw_indexed_instanced_base(

		const GFXVertexSource*  src,
		size_t                  inst,
		unsigned int            instBase,
		unsigned int            vertBase,
		unsigned int            indBase,
		GFX_WIND_ARG)
{
	GFX_REND_GET.DrawElementsInstancedBaseInstance(
		src->primitive,
		src->count,
		src->indexType,
		(GLvoid*)(src->first + indBase),
		inst,
		instBase
	);
}

/******************************************************/
static void _gfx_draw_indexed_vertex_base(

		const GFXVertexSource*  src,
		size_t                  inst,
		unsigned int            instBase,
		unsigned int            vertBase,
		unsigned int            indBase,
		GFX_WIND_ARG)
{
	GFX_REND_GET.DrawElementsBaseVertex(
		src->primitive,
		src->count,
		src->indexType,
		(GLvoid*)(src->first + indBase),
		vertBase
	);
}

/******************************************************/
static void _gfx_draw_indexed_instanced_vertex_base(

		const GFXVertexSource*  src,
		size_t                  inst,
		unsigned int            instBase,
		unsigned int            vertBase,
		unsigned int            indBase,
		GFX_WIND_ARG)
{
	GFX_REND_GET.DrawElementsInstancedBaseVertex(
		src->primitive,
		src->count,
		src->indexType,
		(GLvoid*)(src->first + indBase),
		inst,
		vertBase
	);
}

/******************************************************/
static void _gfx_draw_indexed_instanced_base_vertex_base(

		const GFXVertexSource*  src,
		size_t                  inst,
		unsigned int            instBase,
		unsigned int            vertBase,
		unsigned int            indBase,
		GFX_WIND_ARG)
{
	GFX_REND_GET.DrawElementsInstancedBaseVertexBaseInstance(
		src->primitive,
		src->count,
		src->indexType,
		(GLvoid*)(src->first + indBase),
		inst,
		vertBase,
		instBase
	);
}

/******************************************************/
static void _gfx_bucket_invoke(

		const GFX_Ref*     ref,
		const GFX_Source*  source,
		const GFX_Unit*    unit,
		GFX_WIND_ARG)
{
	/* Bind VAO & Tessellation vertices */
	_gfx_vertex_layout_bind(
		unit->vao,
		GFX_WIND_AS_ARG);

	_gfx_states_set_patch_vertices(
		source->source.patchSize,
		GFX_WIND_AS_ARG);

	/* Bind shader program */
	_gfx_property_map_use(
		ref->map,
		ref->copy,
		ref->instanceBase,
		GFX_WIND_AS_ARG);

	/* Jump table & invoke draw call */
	static const GFX_DrawFunc jump[] =
	{
		_gfx_draw,
		_gfx_draw_instanced,
		_gfx_draw_instanced_base,
		_gfx_draw,
		_gfx_draw_instanced,
		_gfx_draw_instanced_base,
		_gfx_draw_indexed,
		_gfx_draw_indexed_instanced,
		_gfx_draw_indexed_instanced_base,
		_gfx_draw_indexed_vertex_base,
		_gfx_draw_indexed_instanced_vertex_base,
		_gfx_draw_indexed_instanced_base_vertex_base
	};

	jump[ref->type + source->source.indexed](
		&source->source,
		ref->instances,
		ref->instanceBase,
		ref->vertexBase,
		ref->indexBase,
		GFX_WIND_AS_ARG
	);
}

/******************************************************/
static void _gfx_bucket_set_draw_type(

		GFX_Ref* ref)
{
	GFX_WIND_INIT_UNSAFE;

	unsigned int inst = 0;
	unsigned int vert = 0;

	/* Check the extension for bases */
	if(!GFX_WIND_EQ(NULL))
	{
		inst = ref->instanceBase &&
			GFX_WIND_GET.ext[GFX_EXT_INSTANCED_BASE_ATTRIBUTES];
		vert = ref->vertexBase &&
			GFX_WIND_GET.ext[GFX_EXT_VERTEX_BASE_INDICES];
	}

	ref->type = inst ?

		/* Instance base */
		(vert ?
		GFX_INT_DRAW_INSTANCED_BASE_VERTEX_BASE :
		GFX_INT_DRAW_INSTANCED_BASE) :

		ref->instances != 1 ?

		/* Instanced */
 		(vert ?
		GFX_INT_DRAW_INSTANCED_VERTEX_BASE :
		GFX_INT_DRAW_INSTANCED) :

		/* Regular */
		(vert ?
		GFX_INT_DRAW_VERTEX_BASE :
		GFX_INT_DRAW);
}

/******************************************************/
static GFX_Ref* _gfx_bucket_insert_ref(

		GFX_Bucket*   bucket,
		unsigned int  unitIndex)
{
	GFX_Ref* it;

	if(bucket->emptyRefs.begin != bucket->emptyRefs.end)
	{
		/* Replace an empty reference */
		GFXBucketUnit unit = *(GFXBucketUnit*)bucket->emptyRefs.begin;
		gfx_deque_pop_begin(&bucket->emptyRefs);

		it = gfx_vector_at(&bucket->refs, unit - 1);
	}
	else
	{
		/* Check new index for overflow */
		size_t size = gfx_vector_get_size(&bucket->refs);
		GFXBucketUnit unit = size + 1;

		if(unit < size)
		{
			/* Overflow error */
			gfx_errors_push(
				GFX_ERROR_OVERFLOW,
				"Overflow occurred during unit insertion at a bucket."
			);
			return NULL;
		}

		/* Insert a new reference at the end */
		it = gfx_vector_insert(&bucket->refs, NULL, bucket->refs.end);
		if(it == bucket->refs.end) return NULL;
	}

	it->unit = unitIndex + 1;
	return it;
}

/******************************************************/
static void _gfx_bucket_erase_ref(

		GFX_Bucket*   bucket,
		unsigned int  index)
{
	size_t size = gfx_vector_get_size(&bucket->refs);
	GFXBucketUnit unit = index + 1;

	if(unit < size)
	{
		/* Save ID and mark as empty */
		gfx_deque_push_end(&bucket->emptyRefs, &unit);
		((GFX_Ref*)gfx_vector_at(&bucket->refs, index))->unit = 0;
	}
	else
	{
		/* Remove last element */
		gfx_vector_erase_at(&bucket->refs, index);
		--size;
	}

	/* Clear both deque and vector */
	if(!size || size == gfx_deque_get_size(&bucket->emptyRefs))
	{
		gfx_vector_clear(&bucket->refs);
		gfx_deque_clear(&bucket->emptyRefs);
	}
}

/******************************************************/
static inline void _gfx_bucket_swap_units(

		GFX_Unit* unit1,
		GFX_Unit* unit2)
{
	GFX_Unit temp = *unit1;
	*unit1 = *unit2;
	*unit2 = temp;
}

/******************************************************/
static void _gfx_bucket_erase_unit(

		GFX_Bucket*  bucket,
		GFX_Unit*    unit)
{
	bucket->flags |= GFX_INT_BUCKET_PROCESS_UNITS;

	if(unit->state & GFX_INT_UNIT_VISIBLE)
		bucket->flags |= GFX_INT_BUCKET_SORT;

	unit->state |= GFX_INT_UNIT_ERASE;
}

/******************************************************/
static void _gfx_bucket_process_units(

		GFX_Bucket* bucket)
{
	/* Iterate and filter the ones to be erased */
	GFX_Unit* end = bucket->units.end;
	GFX_Unit* it = bucket->units.begin;
	unsigned int num = 0;

	while(it != end)
	{
		/* Swap out if it should be erased */
		if(GFX_INT_UNIT_ERASE & it->state)
		{
			/* Erase precautions */
			_gfx_bucket_erase_ref(bucket, it->ref);

			end = gfx_vector_previous(&bucket->units, end);
			_gfx_bucket_swap_units(it, end);

			++num;
		}
		else it = gfx_vector_next(&bucket->units, it);
	}

	/* Erase the ones to be erased :D */
	gfx_vector_erase_range(&bucket->units, num, it);

	/* Iterate again and filter visible */
	end = bucket->units.end;
	it = bucket->units.begin;
	num = 0;

	while(it != end)
	{
		/* Swap out if not visible */
		if(!(GFX_INT_UNIT_VISIBLE & it->state))
		{
			end = gfx_vector_previous(&bucket->units, end);
			_gfx_bucket_swap_units(it, end);
		}
		else
		{
			it = gfx_vector_next(&bucket->units, it);
			++num;
		}
	}

	/* Get visible iterator */
	bucket->visible = gfx_vector_advance(
		&bucket->units,
		bucket->units.begin,
		num
	);
}

/******************************************************/
static int _gfx_bucket_qsort(

		const void* u1,
		const void* u2)
{
	const GFX_Unit* unit1 = (const GFX_Unit*)u1;
	const GFX_Unit* unit2 = (const GFX_Unit*)u2;

	return
		(unit1->program < unit2->program) ? -1 :
		(unit1->program > unit2->program) ? 1 :
		(unit1->vao < unit2->vao) ? -1 :
		(unit1->vao > unit2->vao) ? 1 :
		0;
}

/******************************************************/
static void _gfx_bucket_sort_units(

		GFX_Bucket*   bucket,
		GFXUnitState  bit,
		unsigned int  start,
		unsigned int  num)
{
	if(num <= 1) return;

	/* Quicksort equal states based on bucket flags */
	if(!bit) qsort(
		gfx_vector_at(&bucket->units, start),
		num,
		sizeof(GFX_Unit),
		_gfx_bucket_qsort);

	else
	{
		/* Radix sort based on state :) */
		/* Start, mid, end */
		unsigned int st = start;
		unsigned int mi = start + num;
		unsigned int en = mi;

		while(st < mi)
		{
			GFX_Unit* unit = gfx_vector_at(&bucket->units, st);

			/* If 1, put in 1 bucket */
			if(unit->state & bit)
			{
				_gfx_bucket_swap_units(
					unit,
					gfx_vector_at(&bucket->units, --mi)
				);
			}
			else ++st;
		}

		/* Sort both buckets */
		bit >>= 1;
		_gfx_bucket_sort_units(bucket, bit, start, mi - start);
		_gfx_bucket_sort_units(bucket, bit, mi, en - mi);
	}
}

/******************************************************/
static void _gfx_bucket_fix_units(

		GFX_Bucket* bucket)
{
	GFX_Unit* it;
	size_t ind = 0;

	for(
		it = bucket->units.begin;
		it != bucket->units.end;
		it = gfx_vector_next(&bucket->units, it))
	{
		GFX_Ref* ref = gfx_vector_at(&bucket->refs, it->ref);
		ref->unit = ++ind;
	}
}

/******************************************************/
static void _gfx_bucket_preprocess(

		GFX_Bucket* bucket)
{
	/* Process all units */
	if(bucket->flags & GFX_INT_BUCKET_PROCESS_UNITS)
		_gfx_bucket_process_units(bucket);

	/* Sort all units */
	if(bucket->flags & GFX_INT_BUCKET_SORT)
		_gfx_bucket_sort_units(
			bucket,
			(GFXUnitState)1 << (bucket->bucket.bits - 1),
			0,
			gfx_vector_get_index(&bucket->units, bucket->visible)
		);

	/* Fix all unit references */
	if(bucket->flags)
		_gfx_bucket_fix_units(bucket);

	bucket->flags = 0;
}

/******************************************************/
GFXBucket* _gfx_bucket_create(

		unsigned char bits)
{
	/* Allocate bucket */
	GFX_Bucket* bucket = calloc(1, sizeof(GFX_Bucket));
	if(!bucket)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Bucket could not be allocated."
		);
		return NULL;
	}

	gfx_vector_init(&bucket->refs, sizeof(GFX_Ref));
	gfx_vector_init(&bucket->sources, sizeof(GFX_Source));
	gfx_vector_init(&bucket->units, sizeof(GFX_Unit));

	gfx_deque_init(&bucket->emptyRefs, sizeof(GFXBucketUnit));
	gfx_deque_init(&bucket->emptySources, sizeof(GFXBucketSource));

	bucket->visible =
		bucket->units.begin;
	bucket->bucket.bits =
		bits > GFX_UNIT_STATE_MAX_BITS ? GFX_UNIT_STATE_MAX_BITS : bits;

	return (GFXBucket*)bucket;
}

/******************************************************/
void _gfx_bucket_free(

		GFXBucket* bucket)
{
	if(bucket)
	{
		GFX_Bucket* internal = (GFX_Bucket*)bucket;

		/* Unblock all layouts */
		GFX_Source* src;
		for(
			src = internal->sources.begin;
			src != internal->sources.end;
			src = gfx_vector_next(&internal->sources, src))
		{
			if(src->layout) _gfx_vertex_layout_unblock(
				src->layout,
				src->index
			);
		}

		/* Free all the things */
		gfx_vector_clear(&internal->refs);
		gfx_vector_clear(&internal->sources);
		gfx_vector_clear(&internal->units);

		gfx_deque_clear(&internal->emptyRefs);
		gfx_deque_clear(&internal->emptySources);

		free(bucket);
	}
}

/******************************************************/
void _gfx_bucket_process(

		GFXBucket*           bucket,
		const GFXPipeState*  state,
		GFX_WIND_ARG)
{
	GFX_Bucket* internal = (GFX_Bucket*)bucket;

	/* Preprocess, set states and invoke units */
	_gfx_bucket_preprocess(internal);
	_gfx_states_set(state, GFX_WIND_AS_ARG);

	GFX_Unit* unit;
	for(
		unit = internal->units.begin;
		unit != internal->visible;
		unit = gfx_vector_next(&internal->units, unit))
	{
		GFX_Ref* ref = gfx_vector_at(
			&internal->refs,
			unit->ref);

		GFX_Source* src = gfx_vector_at(
			&internal->sources,
			ref->src);

		_gfx_bucket_invoke(
			ref,
			src,
			unit,
			GFX_WIND_AS_ARG);
	}
}

/******************************************************/
void gfx_bucket_set_bits(

		GFXBucket*     bucket,
		unsigned char  bits)
{
	/* Clamp */
	bits = bits > GFX_UNIT_STATE_MAX_BITS ?
		GFX_UNIT_STATE_MAX_BITS : bits;

	/* Make sure to resort */
	if(bucket->bits != bits)
		((GFX_Bucket*)bucket)->flags |= GFX_INT_BUCKET_SORT;

	bucket->bits = bits;
}

/******************************************************/
GFXBucketSource gfx_bucket_add_source(

		GFXBucket*        bucket,
		GFXVertexLayout*  layout,
		unsigned char     srcIndex,
		size_t            offset,
		size_t            count)
{
	/* Get source */
	GFX_Source src;
	src.layout = layout;
	src.index = srcIndex;

	if(!layout || !gfx_vertex_layout_get_source(
		layout,
		srcIndex,
		&src.source))
	{
		return 0;
	}

	/* Adjust source and bound check */
	if(offset + count > src.source.count)
		return 0;

	src.source.first += offset;
	src.source.count = count;

	/* Apply index buffer offset */
	if(src.source.indexed)
	{
		/* Get index buffer offset */
		if(!_gfx_vertex_layout_get_index_buffer(layout, &offset))
			return 0;

		GFXDataType type;
		type.unpacked = src.source.indexType;
		unsigned char size = _gfx_sizeof_data_type(type);

		/* Also check alignment of the buffer */
		if(offset % size)
			return 0;

		src.source.first = src.source.first * size + offset;
		src.source.indexed = GFX_INT_DRAW_COUNT;
	}

	/* Attempt to block the layout */
	if(!_gfx_vertex_layout_block(layout, srcIndex))
		return 0;

	/* Insert source */
	GFX_Bucket* internal = (GFX_Bucket*)bucket;
	GFXBucketSource id = 0;

	if(internal->emptySources.begin != internal->emptySources.end)
	{
		/* replace an empty ID */
		id = *(GFXBucketSource*)internal->emptySources.begin;
		gfx_deque_pop_begin(&internal->emptySources);

		*(GFX_Source*)gfx_vector_at(&internal->sources, id - 1) = src;
	}
	else
	{
		/* Get index + 1 as ID, check for overflow, yet again */
		size_t size = gfx_vector_get_size(&internal->sources);
		id = size + 1;

		if(id < size)
		{
			/* Overflow error */
			gfx_errors_push(
				GFX_ERROR_OVERFLOW,
				"Overflow occurred during source insertion at a bucket."
			);

			_gfx_vertex_layout_unblock(layout, srcIndex);
			return 0;
		}

		/* Insert a new source at the end */
		GFXVectorIterator it = gfx_vector_insert(
			&internal->sources,
			&src,
			internal->sources.end
		);

		if(it == internal->sources.end)
		{
			_gfx_vertex_layout_unblock(layout, srcIndex);
			return 0;
		}
	}

	return id;
}

/******************************************************/
void gfx_bucket_remove_source(

		GFXBucket*       bucket,
		GFXBucketSource  src)
{
	GFX_Bucket* internal = (GFX_Bucket*)bucket;
	GFX_Source* source = gfx_vector_at(&internal->sources, src - 1);

	/* Derpsies */
	size_t size = gfx_vector_get_size(&internal->sources);
	if(src && src <= size && source->layout)
	{
		/* Unblock the layout */
		_gfx_vertex_layout_unblock(source->layout, source->index);

		if(src < size)
		{
			/* Save ID and mark as empty */
			gfx_deque_push_end(&internal->emptySources, &src);
			source->layout = NULL;
		}

		else if(src == size)
		{
			/* Remove last element */
			gfx_vector_erase_at(&internal->sources, src - 1);
			--size;
		}

		/* Clear both deque and vector */
		if(!size || size == gfx_deque_get_size(&internal->emptySources))
		{
			gfx_vector_clear(&internal->sources);
			gfx_deque_clear(&internal->emptySources);
		}

		/* Erase any unit using the source */
		GFX_Ref* ref;
		--src;

		for(
			ref = internal->refs.begin;
			ref != internal->refs.end;
			ref = gfx_vector_next(&internal->refs, ref))
		{
			if(ref->src == src) _gfx_bucket_erase_unit(
				internal,
				gfx_vector_at(&internal->units, ref->unit - 1)
			);
		}
	}
}

/******************************************************/
GFXBucketUnit gfx_bucket_insert(

		GFXBucket*             bucket,
		GFXBucketSource        src,
		const GFXPropertyMap*  map,
		unsigned int           copy,
		int                    visible)
{
	--src;

	/* Validate index & source */
	GFX_Bucket* internal = (GFX_Bucket*)bucket;
	GFX_Source* source = gfx_vector_at(&internal->sources, src);

	size_t cnt = gfx_vector_get_size(&internal->sources);
	if(!map || src >= cnt || !source->layout) return 0;

	/* Initialize the new unit */
	GFX_Unit unit;
	unit.state   = visible ? GFX_INT_UNIT_VISIBLE : 0;
	unit.program = _gfx_program_map_get_handle(map->programMap);
	unit.vao     = _gfx_vertex_layout_get_handle(source->layout);

	/* Insert a reference for it */
	GFX_Ref* ref = _gfx_bucket_insert_ref(
		internal,
		gfx_vector_get_size(&internal->units)
	);

	if(!ref) return 0;
	unit.ref = gfx_vector_get_index(&internal->refs, ref);

	/* Initialize the reference */
	ref->src          = src;
	ref->map          = map;
	ref->copy         = copy;
	ref->instances    = 1;
	ref->instanceBase = 0;
	ref->vertexBase   = 0;

	_gfx_bucket_set_draw_type(ref);

	/* Insert the unit */
	GFXVectorIterator it = gfx_vector_insert(
		&internal->units,
		&unit,
		internal->units.end
	);

	if(it == internal->units.end)
	{
		_gfx_bucket_erase_ref(internal, unit.ref);
		return 0;
	}

	/* Force to process */
	internal->flags |= GFX_INT_BUCKET_PROCESS_UNITS;
	if(visible) internal->flags |= GFX_INT_BUCKET_SORT;

	return unit.ref + 1;
}

/******************************************************/
void gfx_bucket_erase(

		GFXBucket*     bucket,
		GFXBucketUnit  unit)
{
	GFX_Ref* ref =
		gfx_vector_at(&((GFX_Bucket*)bucket)->refs, unit - 1);
	GFX_Unit* un =
		gfx_vector_at(&((GFX_Bucket*)bucket)->units, ref->unit - 1);

	_gfx_bucket_erase_unit((GFX_Bucket*)bucket, un);
}

/******************************************************/
unsigned int gfx_bucket_get_copy(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit)
{
	const GFX_Ref* ref =
		gfx_vector_at(&((const GFX_Bucket*)bucket)->refs, unit - 1);

	return ref->copy;
}

/******************************************************/
size_t gfx_bucket_get_instances(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit)
{
	const GFX_Ref* ref =
		gfx_vector_at(&((const GFX_Bucket*)bucket)->refs, unit - 1);

	return ref->instances;
}

/******************************************************/
unsigned int gfx_bucket_get_instance_base(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit)
{
	const GFX_Ref* ref =
		gfx_vector_at(&((const GFX_Bucket*)bucket)->refs, unit - 1);

	return ref->instanceBase;
}

/******************************************************/
unsigned int gfx_bucket_get_vertex_base(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit)
{
	const GFX_Ref* ref =
		gfx_vector_at(&((const GFX_Bucket*)bucket)->refs, unit - 1);

	return ref->vertexBase;
}

/******************************************************/
unsigned int gfx_bucket_get_index_base(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit)
{
	const GFX_Ref* ref =
		gfx_vector_at(&((const GFX_Bucket*)bucket)->refs, unit - 1);
	const GFX_Source* src =
		gfx_vector_at(&((const GFX_Bucket*)bucket)->sources, ref->src);

	GFXDataType type;
	type.unpacked = src->source.indexType;
	unsigned char size = _gfx_sizeof_data_type(type);

	return ref->indexBase / size;
}

/******************************************************/
GFXUnitState gfx_bucket_get_state(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit)
{
	const GFX_Ref* ref =
		gfx_vector_at(&((const GFX_Bucket*)bucket)->refs, unit - 1);
	const GFX_Unit* un =
		gfx_vector_at(&((const GFX_Bucket*)bucket)->units, ref->unit - 1);

	return un->state & GFX_INT_UNIT_MANUAL;
}

/******************************************************/
int gfx_bucket_is_visible(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit)
{
	const GFX_Ref* ref =
		gfx_vector_at(&((const GFX_Bucket*)bucket)->refs, unit - 1);
	const GFX_Unit* un =
		gfx_vector_at(&((const GFX_Bucket*)bucket)->units, ref->unit - 1);

	return un->state & GFX_INT_UNIT_VISIBLE;
}

/******************************************************/
void gfx_bucket_set_copy(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		unsigned int   copy)
{
	GFX_Ref* ref =
		gfx_vector_at(&((GFX_Bucket*)bucket)->refs, unit - 1);

	ref->copy = copy;
}

/******************************************************/
void gfx_bucket_set_instances(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		size_t         instances)
{
	GFX_Ref* ref =
		gfx_vector_at(&((GFX_Bucket*)bucket)->refs, unit - 1);

	ref->instances = instances;
	_gfx_bucket_set_draw_type(ref);
}

/******************************************************/
void gfx_bucket_set_instance_base(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		unsigned int   base)
{
	GFX_Ref* ref =
		gfx_vector_at(&((GFX_Bucket*)bucket)->refs, unit - 1);

	ref->instanceBase = base;
	_gfx_bucket_set_draw_type(ref);
}

/******************************************************/
void gfx_bucket_set_vertex_base(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		unsigned int   base)
{
	GFX_Ref* ref =
		gfx_vector_at(&((GFX_Bucket*)bucket)->refs, unit - 1);

	ref->vertexBase = base;
	_gfx_bucket_set_draw_type(ref);
}

/******************************************************/
void gfx_bucket_set_index_base(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		unsigned int   base)
{
	GFX_Ref* ref =
		gfx_vector_at(&((GFX_Bucket*)bucket)->refs, unit - 1);
	GFX_Source* src =
		gfx_vector_at(&((GFX_Bucket*)bucket)->sources, ref->src);

	GFXDataType type;
	type.unpacked = src->source.indexType;
	unsigned char size = _gfx_sizeof_data_type(type);

	ref->indexBase = base * size;
}

/******************************************************/
void gfx_bucket_set_state(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		GFXUnitState   state)
{
	GFX_Ref* ref =
		gfx_vector_at(&((GFX_Bucket*)bucket)->refs, unit - 1);
	GFX_Unit* un =
		gfx_vector_at(&((GFX_Bucket*)bucket)->units, ref->unit - 1);

	/* Detect equal states */
	/* Force a re-sort if visible */
	state = (state & GFX_INT_UNIT_MANUAL) | (un->state & ~GFX_INT_UNIT_MANUAL);

	if(un->state != state && (un->state & GFX_INT_UNIT_VISIBLE))
		((GFX_Bucket*)bucket)->flags |= GFX_INT_BUCKET_SORT;

	un->state = state;
}

/******************************************************/
void gfx_bucket_set_visible(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		int            visible)
{
	GFX_Ref* ref =
		gfx_vector_at(&((GFX_Bucket*)bucket)->refs, unit - 1);
	GFX_Unit* un =
		gfx_vector_at(&((GFX_Bucket*)bucket)->units, ref->unit - 1);

	if(!(un->state & GFX_INT_UNIT_ERASE))
	{
		visible = visible ? 1 : 0;
		int cur = un->state & GFX_INT_UNIT_VISIBLE ? 1 : 0;

		if(visible != cur) ((GFX_Bucket*)bucket)->flags |=
			GFX_INT_BUCKET_PROCESS_UNITS | GFX_INT_BUCKET_SORT;

		if(visible)
			un->state |= GFX_INT_UNIT_VISIBLE;
		else
			un->state &= ~GFX_INT_UNIT_VISIBLE;
	}
}
