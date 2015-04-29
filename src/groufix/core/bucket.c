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

/******************************************************/
/* Internal comparison function */
typedef int (*GFX_BucketComp)(

		const void* u1,
		const void* u2);


/* Internal bucket */
typedef struct GFX_Bucket
{
	/* Super class */
	GFXBucket bucket;

	/* Hidden data */
	unsigned char      flags;
	GFX_BucketComp     compare;      /* Quicksort compare */

	GFXVector          sources;      /* Stores GFX_Source */
	GFXDeque           emptySources; /* Stores GFXBucketSource, free source IDs */

	GFXVector          refs;         /* Stores unsigned int (units[refs[ID - 1] - 1] = unit, 0 when empty) */
	GFXDeque           emptyRefs;    /* Stores GFXBucketUnit, free (empty) reference IDs */
	GFXVector          units;        /* Stores GFX_Unit */
	GFXVectorIterator  visible;      /* Everything after is not visible */

} GFX_Bucket;


/* Internal source */
typedef struct GFX_Source
{
	const GFXVertexLayout*  layout; /* NULL when empty */
	GFXVertexSource         source;

} GFX_Source;


/* Internal render unit */
typedef struct GFX_Unit
{
	/* Sorting */
	GFXUnitState           state;    /* Combination of unit state, action and manual state */
	GFXBucketUnit          ref;      /* Reference of the unit units[refs[ref] - 1] = this (const, equal to ID - 1) */
	GLuint                 program;  /* Program or program map to sort on */
	GLuint                 vao;      /* layout to sort on */

	/* Drawing */
	const GFXPropertyMap*  map;
	unsigned int           copy;     /* Copy of the property map to use */
	GFXBucketSource        src;      /* Source of the bucket to use (ID - 1) */

	size_t                 inst;     /* Number of instances */
	unsigned int           instBase; /* Instance base */
	int                    vertBase; /* Vertex base */
	GFX_DrawType           type;

} GFX_Unit;


/******************************************************/
static int _gfx_bucket_qsort_program(

		const void* u1,
		const void* u2)
{
	const GFX_Unit* unit1 = (const GFX_Unit*)u1;
	const GFX_Unit* unit2 = (const GFX_Unit*)u2;

	return
		(unit1->program < unit2->program) ? -1 :
		(unit1->program > unit2->program) ? 1 :
		0;
}

/******************************************************/
static int _gfx_bucket_qsort_layout(

		const void* u1,
		const void* u2)
{
	const GFX_Unit* unit1 = (const GFX_Unit*)u1;
	const GFX_Unit* unit2 = (const GFX_Unit*)u2;

	return
		(unit1->vao < unit2->vao) ? -1 :
		(unit1->vao > unit2->vao) ? 1 :
		0;
}

/******************************************************/
static int _gfx_bucket_qsort_all(

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
static GFXBucketUnit _gfx_bucket_insert_ref(

		GFX_Bucket*   bucket,
		unsigned int  unitIndex)
{
	++unitIndex;
	GFXBucketUnit ref = 0;

	if(bucket->emptyRefs.begin != bucket->emptyRefs.end)
	{
		/* Replace an empty reference */
		ref = *(GFXBucketUnit*)bucket->emptyRefs.begin;
		gfx_deque_pop_begin(&bucket->emptyRefs);

		*(unsigned int*)gfx_vector_at(&bucket->refs, ref - 1) =
			unitIndex;
	}
	else
	{
		/* Get index + 1 as reference, check for overflow */
		size_t size = gfx_vector_get_size(&bucket->refs);
		ref = size + 1;

		if(ref < size)
		{
			/* Overflow error */
			gfx_errors_push(
				GFX_ERROR_OVERFLOW,
				"Overflow occurred during unit insertion at a bucket."
			);
			return 0;
		}

		/* Insert a new reference at the end */
		GFXVectorIterator it = gfx_vector_insert(
			&bucket->refs,
			&unitIndex,
			bucket->refs.end
		);

		if(it == bucket->refs.end) return 0;
	}

	return ref;
}

/******************************************************/
static inline GFX_Unit* _gfx_bucket_ref_get(

		const GFX_Bucket*  bucket,
		GFXBucketUnit      id)
{
	return gfx_vector_at(
		&bucket->units,
		*(unsigned int*)gfx_vector_at(&bucket->refs, id - 1) - 1
	);
}

/******************************************************/
static void _gfx_bucket_erase_ref(

		GFX_Bucket*    bucket,
		GFXBucketUnit  index)
{
	size_t size = gfx_vector_get_size(&bucket->refs);
	++index;

	if(index < size)
	{
		/* Save ID and mark as empty */
		gfx_deque_push_end(&bucket->emptyRefs, &index);
		*(unsigned int*)gfx_vector_at(&bucket->refs, index - 1) = 0;
	}
	else
	{
		/* Remove last element */
		gfx_vector_erase_at(&bucket->refs, index - 1);
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
	GFXVectorIterator end = bucket->units.end;
	GFXVectorIterator it = bucket->units.begin;
	unsigned int num = 0;

	while(it != end)
	{
		/* Swap out if it should be erased */
		if(GFX_INT_UNIT_ERASE & ((GFX_Unit*)it)->state)
		{
			/* Erase precautions */
			_gfx_bucket_erase_ref(
				bucket,
				((GFX_Unit*)it)->ref);

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
		if(!(GFX_INT_UNIT_VISIBLE & ((GFX_Unit*)it)->state))
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
static void _gfx_bucket_sort_units(

		GFX_Bucket*   bucket,
		GFXUnitState  bit,
		unsigned int  start,
		unsigned int  num)
{
	if(num <= 1) return;

	if(!bit)
	{
		/* Quicksort equal states based on bucket flags */
		if(bucket->compare) qsort(
			gfx_vector_at(&bucket->units, start),
			num,
			sizeof(GFX_Unit),
			bucket->compare
		);

		return;
	}

	/* Radix sort based on state :) */
	/* Start, mid, end */
	unsigned int st = start;
	unsigned int mi = start + num;
	unsigned int en = mi;

	while(st < mi)
	{
		GFX_Unit* unit = (GFX_Unit*)gfx_vector_at(
			&bucket->units,
			st
		);

		/* If 1, put in 1 bucket */
		if(unit->state & bit)
		{
			_gfx_bucket_swap_units(unit, gfx_vector_at(
				&bucket->units,
				--mi)
			);
		}
		else ++st;
	}

	/* Sort both buckets */
	bit >>= 1;
	_gfx_bucket_sort_units(bucket, bit, start, mi - start);
	_gfx_bucket_sort_units(bucket, bit, mi, en - mi);
}

/******************************************************/
static void _gfx_bucket_fix_units(

		GFX_Bucket* bucket)
{
	GFXVectorIterator it;
	for(
		it = bucket->units.begin;
		it != bucket->units.end;
		it = gfx_vector_next(&bucket->units, it))
	{
		*(unsigned int*)gfx_vector_at(&bucket->refs, ((GFX_Unit*)it)->ref) =
			gfx_vector_get_index(&bucket->units, it) + 1;
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
static void _gfx_bucket_set_draw_type(

		GFX_Unit* unit)
{
	GFX_WIND_INIT_UNSAFE;

	unsigned int base = 0;

	/* Check the extension for base instancing */
	if(!GFX_WIND_EQ(NULL))
		base = GFX_WIND_GET.ext[GFX_EXT_INSTANCED_BASE_ATTRIBUTES];

	unit->type =
		(unit->instBase != 0 && base) ?
			GFX_INT_DRAW_INSTANCED_BASE :
		(unit->inst != 1) ?
			GFX_INT_DRAW_INSTANCED :
			GFX_INT_DRAW;
}

/******************************************************/
GFXBucket* _gfx_bucket_create(

		unsigned char   bits,
		GFXBucketFlags  flags)
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

	gfx_vector_init(&bucket->sources, sizeof(GFX_Source));
	gfx_deque_init(&bucket->emptySources, sizeof(GFXBucketSource));

	gfx_vector_init(&bucket->refs, sizeof(unsigned int));
	gfx_deque_init(&bucket->emptyRefs, sizeof(GFXBucketUnit));
	gfx_vector_init(&bucket->units, sizeof(GFX_Unit));

	bucket->visible
		= bucket->units.end;
	bucket->bucket.flags
		= flags;
	bucket->bucket.bits
		= bits > GFX_UNIT_STATE_MAX_BITS ? GFX_UNIT_STATE_MAX_BITS : bits;

	/* Get comparison function from flags */
	bucket->compare =
		((flags & GFX_BUCKET_SORT_ALL) == GFX_BUCKET_SORT_ALL) ?
			_gfx_bucket_qsort_all :
		(flags & GFX_BUCKET_SORT_PROGRAM) ?
			_gfx_bucket_qsort_program :
		(flags & GFX_BUCKET_SORT_VERTEX_LAYOUT) ?
			_gfx_bucket_qsort_layout :
			NULL;

	return (GFXBucket*)bucket;
}

/******************************************************/
void _gfx_bucket_free(

		GFXBucket* bucket)
{
	if(bucket)
	{
		GFX_Bucket* internal = (GFX_Bucket*)bucket;

		gfx_vector_clear(&internal->sources);
		gfx_deque_clear(&internal->emptySources);

		gfx_vector_clear(&internal->refs);
		gfx_deque_clear(&internal->emptyRefs);
		gfx_vector_clear(&internal->units);

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

	/* Preprocess, set states and process */
	_gfx_bucket_preprocess(internal);
	_gfx_states_set(state, GFX_WIND_AS_ARG);

	GFX_Unit* unit;
	for(
		unit = internal->units.begin;
		unit != internal->visible;
		unit = gfx_vector_next(&internal->units, unit))
	{
		GFX_Source* src = gfx_vector_at(
			&internal->sources,
			unit->src
		);

		/* Bind shader program & draw */
		_gfx_property_map_use(
			unit->map,
			unit->copy,
			unit->instBase,
			GFX_WIND_AS_ARG);

		_gfx_vertex_layout_draw(
			src->layout,
			src->source,
			unit->inst,
			unit->instBase,
			unit->vertBase,
			unit->type,
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

		GFXBucket*              bucket,
		const GFXVertexLayout*  layout)
{
	/* Derp */
	if(!layout) return 0;

	GFX_Bucket* internal = (GFX_Bucket*)bucket;

	/* Create source */
	GFX_Source src;
	GFXBucketSource id = 0;

	memset(&src, 0, sizeof(GFX_Source));
	src.layout = layout;

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
			return 0;
		}

		/* Insert a new source at the end */
		GFXVectorIterator it = gfx_vector_insert(
			&internal->sources,
			&src,
			internal->sources.end
		);

		if(it == internal->sources.end) return 0;
	}

	return id;
}

/******************************************************/
int gfx_bucket_set_source(

		GFXBucket*       bucket,
		GFXBucketSource  src,
		GFXVertexSource  values)
{
	--src;

	/* Validate index */
	GFX_Bucket* internal = (GFX_Bucket*)bucket;
	size_t cnt = gfx_vector_get_size(&internal->sources);

	if(src >= cnt)
		return 0;

	/* Check source and draw call index */
	GFX_Source* source = gfx_vector_at(&internal->sources, src);
	if(!source->layout || values.drawIndex >= source->layout->drawCalls)
		return 0;

	source->source = values;
	return 1;
}

/******************************************************/
void gfx_bucket_remove_source(

		GFXBucket*       bucket,
		GFXBucketSource  src)
{
	/* Derpsies */
	if(src)
	{
		GFX_Bucket* internal = (GFX_Bucket*)bucket;
		size_t size = gfx_vector_get_size(&internal->sources);

		if(src < size)
		{
			/* Save ID and mark as empty */
			gfx_deque_push_end(&internal->emptySources, &src);
			((GFX_Source*)gfx_vector_at(&internal->sources, src - 1))->layout = NULL;
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
		GFX_Unit* unit;
		--src;

		for(
			unit = internal->units.begin;
			unit != internal->units.end;
			unit = gfx_vector_next(&internal->units, unit))
		{
			if(unit->src == src) _gfx_bucket_erase_unit(internal, unit);
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
	size_t cnt = gfx_vector_get_size(&internal->sources);

	if(!map || src >= cnt) return 0;

	GFX_Source* source = gfx_vector_at(&internal->sources, src);
	if(!source->layout) return 0;

	/* Initialize the new unit */
	GFX_Unit unit;

	unit.state    = visible ? GFX_INT_UNIT_VISIBLE : 0;
	unit.program  = _gfx_program_map_get_handle(map->programMap);
	unit.vao      = _gfx_vertex_layout_get_handle(source->layout);

	unit.map      = map;
	unit.copy     = copy;
	unit.src      = src;
	unit.inst     = 1;
	unit.instBase = 0;
	unit.vertBase = 0;

	_gfx_bucket_set_draw_type(&unit);

	/* Insert a reference for it */
	unit.ref = _gfx_bucket_insert_ref(
		internal,
		gfx_vector_get_size(&internal->units)
	);

	if(!unit.ref) return 0;
	--unit.ref;

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
unsigned int gfx_bucket_get_copy(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit)
{
	return _gfx_bucket_ref_get((const GFX_Bucket*)bucket, unit)->copy;
}

/******************************************************/
size_t gfx_bucket_get_instances(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit)
{
	return _gfx_bucket_ref_get((const GFX_Bucket*)bucket, unit)->inst;
}

/******************************************************/
unsigned int gfx_bucket_get_instance_base(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit)
{
	return _gfx_bucket_ref_get((const GFX_Bucket*)bucket, unit)->instBase;
}

/******************************************************/
int gfx_bucket_get_vertex_base(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit)
{
	return _gfx_bucket_ref_get((const GFX_Bucket*)bucket, unit)->vertBase;
}

/******************************************************/
GFXUnitState gfx_bucket_get_state(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit)
{
	const GFX_Bucket* internal = (const GFX_Bucket*)bucket;
	return _gfx_bucket_ref_get(internal, unit)->state & GFX_INT_UNIT_MANUAL;
}

/******************************************************/
int gfx_bucket_is_visible(

		const GFXBucket*  bucket,
		GFXBucketUnit     unit)
{
	return _gfx_bucket_ref_get((const GFX_Bucket*)bucket, unit)->state
		& GFX_INT_UNIT_VISIBLE;
}

/******************************************************/
void gfx_bucket_set_copy(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		unsigned int   copy)
{
	_gfx_bucket_ref_get((GFX_Bucket*)bucket, unit)->copy = copy;
}

/******************************************************/
void gfx_bucket_set_instances(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		size_t         instances)
{
	GFX_Unit* un = _gfx_bucket_ref_get((GFX_Bucket*)bucket, unit);
	un->inst = instances;

	_gfx_bucket_set_draw_type(un);
}

/******************************************************/
void gfx_bucket_set_instance_base(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		unsigned int   base)
{
	GFX_Unit* un = _gfx_bucket_ref_get((GFX_Bucket*)bucket, unit);
	un->instBase = base;

	_gfx_bucket_set_draw_type(un);
}

/******************************************************/
void gfx_bucket_set_vertex_base(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		int            base)
{
	GFX_Unit* un = _gfx_bucket_ref_get((GFX_Bucket*)bucket, unit);
	un->vertBase = base;
}

/******************************************************/
void gfx_bucket_set_state(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		GFXUnitState   state)
{
	GFX_Bucket* internal = (GFX_Bucket*)bucket;
	GFX_Unit* un = _gfx_bucket_ref_get(internal, unit);

	/* Detect equal states */
	state = (state & GFX_INT_UNIT_MANUAL) | (un->state & ~GFX_INT_UNIT_MANUAL);
	if(un->state != state && (un->state & GFX_INT_UNIT_VISIBLE))
	{
		/* Force a re-sort if visible */
		internal->flags |= GFX_INT_BUCKET_SORT;
	}
	un->state = state;
}

/******************************************************/
void gfx_bucket_set_visible(

		GFXBucket*     bucket,
		GFXBucketUnit  unit,
		int            visible)
{
	GFX_Bucket* internal = (GFX_Bucket*)bucket;
	GFX_Unit* un = _gfx_bucket_ref_get(internal, unit);

	if(!(un->state & GFX_INT_UNIT_ERASE))
	{
		visible = visible ? 1 : 0;
		int cur = (un->state & GFX_INT_UNIT_VISIBLE) ? 1 : 0;

		if(visible != cur)
			internal->flags |= GFX_INT_BUCKET_PROCESS_UNITS | GFX_INT_BUCKET_SORT;

		if(visible)
			un->state |= GFX_INT_UNIT_VISIBLE;
		else
			un->state &= ~GFX_INT_UNIT_VISIBLE;
	}
}

/******************************************************/
void gfx_bucket_erase(

		GFXBucket*     bucket,
		GFXBucketUnit  unit)
{
	GFX_Bucket* internal = (GFX_Bucket*)bucket;
	GFX_Unit* un = _gfx_bucket_ref_get(internal, unit);

	_gfx_bucket_erase_unit(internal, un);
}
