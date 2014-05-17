/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Groufix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Groufix.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "groufix/core/pipeline/internal.h"
#include "groufix/core/memory/internal.h"
#include "groufix/core/shading/internal.h"
#include "groufix/containers/vector.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Internal bucket flags */
#define GFX_INT_BUCKET_PROCESS_UNITS  0x01
#define GFX_INT_BUCKET_SORT           0x02

/* Internal unit state and action (for processing) */
#define GFX_INT_UNIT_VISIBLE     ((~(UINT32_MAX >> 1)) >> 0)
#define GFX_INT_UNIT_ERASE       ((~(UINT32_MAX >> 1)) >> 1)
#define GFX_INT_UNIT_MANUAL_MSB  ((~(UINT32_MAX >> 1)) >> 2)
#define GFX_INT_UNIT_MANUAL      (~((~(GFX_INT_UNIT_MANUAL_MSB << 1)) + 1))

/******************************************************/
/* Internal comparison function */
typedef int (*GFX_BucketComp)(

		const void* u1,
		const void* u2);


/* Internal bucket */
struct GFX_Bucket
{
	/* Super class */
	GFXBucket bucket;

	/* Hidden data */
	unsigned char      flags;
	GFX_BucketComp     compare;  /* Quicksort compare */

	GFXVector          sources;  /* Stores GFX_Source */
	GFXVector          refs;     /* References to units (units[refs[ID - 1] - 1] = unit, 0 is empty) */
	GFXVector          units;    /* Stores GFX_Unit */
	GFXVectorIterator  visible;  /* Everything after is not visible */
};

/* Internal source */
struct GFX_Source
{
	GFXVertexLayout*  layout;
	GFXVertexSource   source;
};

/* Internal batch unit */
struct GFX_Unit
{
	/* Sorting and ID */
	GFXBatchState    state;   /* Combination of unit state, action and manual state */
	size_t           ref;     /* Reference of the unit units[refs[ref] - 1] = this (const, equal to ID - 1) */
	size_t           program; /* ID of the program */
	size_t           layout;  /* ID of the layout */

	/* Drawing */
	GFXPropertyMap*  map;
	size_t           copy;   /* Copy of the property map to use */

	size_t           src;    /* Source of the bucket to use (ID - 1) */
	size_t           inst;   /* Number of instances */
	unsigned int     base;   /* Base instance for instanced vertex attributes */
	GFX_DrawType     type;
};

/******************************************************/
static int _gfx_bucket_qsort_program(

		const void* u1,
		const void* u2)
{
	struct GFX_Unit* unit1 = (struct GFX_Unit*)u1;
	struct GFX_Unit* unit2 = (struct GFX_Unit*)u2;

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
	struct GFX_Unit* unit1 = (struct GFX_Unit*)u1;
	struct GFX_Unit* unit2 = (struct GFX_Unit*)u2;

	return
		(unit1->layout < unit2->layout) ? -1 :
		(unit1->layout > unit2->layout) ? 1 :
		0;
}

/******************************************************/
static int _gfx_bucket_qsort_all(

		const void* u1,
		const void* u2)
{
	struct GFX_Unit* unit1 = (struct GFX_Unit*)u1;
	struct GFX_Unit* unit2 = (struct GFX_Unit*)u2;

	return
		(unit1->program < unit2->program) ? -1 :
		(unit1->program > unit2->program) ? 1 :
		(unit1->layout < unit2->layout) ? -1 :
		(unit1->layout > unit2->layout) ? 1 :
		0;
}

/******************************************************/
static size_t _gfx_bucket_insert_ref(

		struct GFX_Bucket*  bucket,
		size_t              unitIndex)
{
	++unitIndex;

	/* Search for an empty reference */
	GFXVectorIterator it;
	for(
		it = bucket->refs.begin;
		it != bucket->refs.end;
		it = gfx_vector_next(&bucket->refs, it))
	{
		if(!(*(size_t*)it))
		{
			*(size_t*)it = unitIndex;
			break;
		}
	}

	/* Insert a new one */
	if(it == bucket->refs.end) it = gfx_vector_insert(
		&bucket->refs,
		&unitIndex,
		bucket->refs.end
	);

	/* Return index + 1, 0 would be a failure */
	return (it == bucket->refs.end) ? 0 : gfx_vector_get_index(&bucket->refs, it) + 1;
}

/******************************************************/
static inline struct GFX_Unit* _gfx_bucket_ref_get(

		struct GFX_Bucket*  bucket,
		size_t              id)
{
	return gfx_vector_at(
		&bucket->units,
		*(size_t*)gfx_vector_at(&bucket->refs, id - 1) - 1
	);
}

/******************************************************/
static void _gfx_bucket_erase_ref(

		struct GFX_Bucket*  bucket,
		size_t              index)
{
	GFXVectorIterator it = gfx_vector_at(&bucket->refs, index);
	*(size_t*)it = 0;

	/* Erase trailing zeros */
	if(index + 1 >= gfx_vector_get_size(&bucket->refs))
	{
		size_t del = 1;
		while(it != bucket->refs.begin)
		{
			GFXVectorIterator prev = gfx_vector_previous(&bucket->refs, it);
			if(*(size_t*)prev) break;

			it = prev;
			++del;
		}

		gfx_vector_erase_range(&bucket->refs, del, it);
	}
}

/******************************************************/
static inline void _gfx_bucket_swap_units(

		struct GFX_Bucket*  bucket,
		struct GFX_Unit*    unit1,
		struct GFX_Unit*    unit2)
{
	struct GFX_Unit temp = *unit1;
	*unit1 = *unit2;
	*unit2 = temp;
}

/******************************************************/
static void _gfx_bucket_erase_unit(

		struct GFX_Bucket*  bucket,
		struct GFX_Unit*    unit)
{
	bucket->flags |= GFX_INT_BUCKET_PROCESS_UNITS;

	if(unit->state & GFX_INT_UNIT_VISIBLE)
		bucket->flags |= GFX_INT_BUCKET_SORT;

	unit->state |= GFX_INT_UNIT_ERASE;
}

/******************************************************/
static void _gfx_bucket_process_units(

		struct GFX_Bucket* bucket)
{
	/* Iterate and filter the ones to be erased */
	GFXVectorIterator end = bucket->units.end;
	GFXVectorIterator it = bucket->units.begin;
	size_t num = 0;

	while(it != end)
	{
		/* Swap out if it should be erased */
		if(GFX_INT_UNIT_ERASE & ((struct GFX_Unit*)it)->state)
		{
			end = gfx_vector_previous(&bucket->units, end);
			_gfx_bucket_swap_units(bucket, it, end);

			/* Erase precautions */
			_gfx_bucket_erase_ref(
				bucket,
				((struct GFX_Unit*)it)->ref
			);
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
		if(!(GFX_INT_UNIT_VISIBLE & ((struct GFX_Unit*)it)->state))
		{
			end = gfx_vector_previous(&bucket->units, end);
			_gfx_bucket_swap_units(bucket, it, end);
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

		struct GFX_Bucket*  bucket,
		GFXBatchState       bit,
		size_t              start,
		size_t              num)
{
	/* Skip on range of 1 or no bit */
	if(num > 1 && bit)
	{
		/* Radix sort based on state :) */
		/* Start, mid, end */
		size_t st = start;
		size_t mi = start + num;
		size_t en = mi;

		while(st < mi)
		{
			struct GFX_Unit* unit = (struct GFX_Unit*)gfx_vector_at(
				&bucket->units,
				st);

			/* If 1, put in 1 bucket */
			if(unit->state & bit)
			{
				_gfx_bucket_swap_units(bucket, unit, gfx_vector_at(
					&bucket->units,
					--mi));
			}
			else ++st;
		}

		/* Sort both buckets */
		bit >>= 1;
		_gfx_bucket_sort_units(bucket, bit, start, mi - start);
		_gfx_bucket_sort_units(bucket, bit, mi, en - mi);
	}

	else if(num)
	{
		/* Quicksort equal states based on bucket flags */
		if(bucket->compare) qsort(
			gfx_vector_at(&bucket->units, start),
			num,
			sizeof(struct GFX_Unit),
			bucket->compare
		);
	}
}

/******************************************************/
static void _gfx_bucket_fix_units(

		struct GFX_Bucket* bucket)
{
	GFXVectorIterator it;
	for(
		it = bucket->units.begin;
		it != bucket->units.end;
		it = gfx_vector_next(&bucket->units, it))
	{
		*(size_t*)gfx_vector_at(&bucket->refs, ((struct GFX_Unit*)it)->ref) =
			gfx_vector_get_index(&bucket->units, it) + 1;
	}
}

/******************************************************/
GFXBucket* _gfx_bucket_create(

		unsigned char   bits,
		GFXBucketFlags  flags)
{
	/* Allocate bucket */
	struct GFX_Bucket* bucket = calloc(1, sizeof(struct GFX_Bucket));
	if(!bucket) return NULL;

	gfx_vector_init(&bucket->sources, sizeof(struct GFX_Source));
	gfx_vector_init(&bucket->refs, sizeof(size_t));
	gfx_vector_init(&bucket->units, sizeof(struct GFX_Unit));

	bucket->visible      = bucket->units.end;
	bucket->bucket.flags = flags;
	bucket->bucket.bits  = bits > GFX_BATCH_STATE_MAX_BITS ?
		GFX_BATCH_STATE_MAX_BITS : bits;

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
		struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;

		gfx_vector_clear(&internal->sources);
		gfx_vector_clear(&internal->refs);
		gfx_vector_clear(&internal->units);

		free(bucket);
	}
}

/******************************************************/
static void _gfx_bucket_preprocess(

		struct GFX_Bucket* bucket)
{
	/* Process all units */
	if(bucket->flags & GFX_INT_BUCKET_PROCESS_UNITS)
		_gfx_bucket_process_units(bucket);

	/* Sort all units */
	if(bucket->flags & GFX_INT_BUCKET_SORT)
		_gfx_bucket_sort_units(
			bucket,
			(GFXBatchState)1 << (bucket->bucket.bits - 1),
			0,
			gfx_vector_get_index(&bucket->units, bucket->visible)
		);

	/* Fix all unit references */
	if(bucket->flags)
		_gfx_bucket_fix_units(bucket);

	bucket->flags = 0;
}

/******************************************************/
void _gfx_bucket_process(

		GFXBucket*       bucket,
		GFXPipeState*    state,
		GFX_Extensions*  ext)
{
	struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;

	/* Preprocess, set states and process */
	_gfx_bucket_preprocess(internal);
	_gfx_states_set(state, ext);

	struct GFX_Unit* unit;
	for(
		unit = internal->units.begin;
		unit != internal->visible;
		unit = gfx_vector_next(&internal->units, unit))
	{
		struct GFX_Source* src = gfx_vector_at(&internal->sources, unit->src);

		/* Bind shader program & draw */
		_gfx_property_map_use(unit->map, unit->copy, ext);

		_gfx_vertex_layout_draw(
			src->layout,
			src->source,
			unit->inst,
			unit->base,
			unit->type
		);
	}
}

/******************************************************/
void gfx_bucket_set_bits(

		GFXBucket*     bucket,
		unsigned char  bits)
{
	/* Clamp */
	bits = bits > GFX_BATCH_STATE_MAX_BITS ?
		GFX_BATCH_STATE_MAX_BITS : bits;

	/* Make sure to resort */
	if(bucket->bits != bits)
		((struct GFX_Bucket*)bucket)->flags |= GFX_INT_BUCKET_SORT;

	bucket->bits = bits;
}

/******************************************************/
size_t gfx_bucket_add_source(

		GFXBucket*        bucket,
		GFXVertexLayout*  layout)
{
	/* Derp */
	if(!layout) return 0;

	struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;

	/* Create source */
	struct GFX_Source src;

	memset(&src, 0, sizeof(struct GFX_Source));
	src.layout = layout;

	/* Find disabled source to replace */
	GFXVectorIterator it;
	for(
		it = internal->sources.begin;
		it != internal->sources.end;
		it = gfx_vector_next(&internal->sources, it))
	{
		struct GFX_Source* source = it;
		if(!source->layout)
		{
			*source = src;
			break;
		}
	}

	/* Insert as a new source */
	if(it == internal->sources.end) it = gfx_vector_insert(
		&internal->sources,
		&src,
		internal->sources.end
	);

	/* Return index + 1 */
	return (it == internal->sources.end) ? 0 : gfx_vector_get_index(&internal->sources, it) + 1;
}

/******************************************************/
int gfx_bucket_set_source(

		GFXBucket*       bucket,
		size_t           src,
		GFXVertexSource  values)
{
	--src;

	/* Validate index */
	struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;
	size_t cnt = gfx_vector_get_size(&internal->sources);

	if(src >= cnt) return 0;

	struct GFX_Source* source = gfx_vector_at(&internal->sources, src);

	/* Check draw call boundaries */
	if(values.startDraw + values.numDraw > source->layout->drawCalls) return 0;
	source->source = values;

	return 1;
}

/******************************************************/
void gfx_bucket_remove_source(

		GFXBucket*  bucket,
		size_t      src)
{
	--src;

	/* Validate index */
	struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;
	size_t cnt = gfx_vector_get_size(&internal->sources);

	if(src < cnt)
	{
		/* Disable source */
		struct GFX_Source* source = gfx_vector_at(&internal->sources, src);
		source->layout = NULL;

		/* Erase any unit using the source */
		struct GFX_Unit* unit;
		for(
			unit = internal->units.begin;
			unit != internal->units.end;
			unit = gfx_vector_next(&internal->units, unit))
		{
			if(unit->src == src) _gfx_bucket_erase_unit(internal, unit);
		}

		/* Erase trailing disabled sources */
		if(src + 1 >= cnt)
		{
			size_t del = 1;
			while(source != internal->sources.begin)
			{
				struct GFX_Source* prev = gfx_vector_previous(&internal->sources, source);
				if(prev->layout) break;

				source = prev;
				++del;
			}

			gfx_vector_erase_range(&internal->sources, del, source);
		}
	}
}

/******************************************************/
static inline void _gfx_bucket_set_draw_type(

		struct GFX_Unit* unit)
{
	unit->type =
		(unit->base != 0) ?
			GFX_INT_DRAW_INSTANCED_BASE :
		(unit->inst != 1) ?
			GFX_INT_DRAW_INSTANCED :
			GFX_INT_DRAW;
}

/******************************************************/
size_t gfx_bucket_insert(

		GFXBucket*       bucket,
		size_t           src,
		GFXPropertyMap*  map,
		GFXBatchState    state,
		int              visible)
{
	--src;

	/* Validate index & source */
	struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;
	size_t cnt = gfx_vector_get_size(&internal->sources);

	if(!map || src >= cnt) return 0;

	struct GFX_Source* source = gfx_vector_at(&internal->sources, src);
	if(!source->layout) return 0;

	/* Initialize the new unit */
	struct GFX_Unit unit;

	unit.state    = state & GFX_INT_UNIT_MANUAL;
	unit.state   |= visible ? GFX_INT_UNIT_VISIBLE : 0;
	unit.program  = map->program->id;
	unit.layout   = source->layout->id;

	unit.map  = map;
	unit.copy = 0;
	unit.src  = src;
	unit.inst = 1;
	unit.base = 0;

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
size_t gfx_bucket_get_copy(

		GFXBucket*  bucket,
		size_t      unit)
{
	return _gfx_bucket_ref_get((struct GFX_Bucket*)bucket, unit)->copy;
}

/******************************************************/
size_t gfx_bucket_get_instances(

		GFXBucket*  bucket,
		size_t      unit)
{
	return _gfx_bucket_ref_get((struct GFX_Bucket*)bucket, unit)->inst;
}

/******************************************************/
unsigned int gfx_bucket_get_instance_base(

		GFXBucket*  bucket,
		size_t      unit)
{
	return _gfx_bucket_ref_get((struct GFX_Bucket*)bucket, unit)->base;
}

/******************************************************/
GFXBatchState gfx_bucket_get_state(

		GFXBucket*  bucket,
		size_t      unit)
{
	struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;
	return _gfx_bucket_ref_get(internal, unit)->state & GFX_INT_UNIT_MANUAL;
}

/******************************************************/
int gfx_bucket_is_visible(

		GFXBucket*  bucket,
		size_t      unit)
{
	return _gfx_bucket_ref_get((struct GFX_Bucket*)bucket, unit)->state & GFX_INT_UNIT_VISIBLE;
}

/******************************************************/
void gfx_bucket_set_copy(

		GFXBucket*  bucket,
		size_t      unit,
		size_t      copy)
{
	_gfx_bucket_ref_get((struct GFX_Bucket*)bucket, unit)->copy = copy;
}

/******************************************************/
void gfx_bucket_set_instances(

		GFXBucket*  bucket,
		size_t      unit,
		size_t      instances)
{
	struct GFX_Unit* un = _gfx_bucket_ref_get((struct GFX_Bucket*)bucket, unit);
	un->inst = instances;

	_gfx_bucket_set_draw_type(un);
}

/******************************************************/
void gfx_bucket_set_instance_base(

		GFXBucket*    bucket,
		size_t        unit,
		unsigned int  base)
{
	struct GFX_Unit* un = _gfx_bucket_ref_get((struct GFX_Bucket*)bucket, unit);
	un->base = base;

	_gfx_bucket_set_draw_type(un);
}

/******************************************************/
void gfx_bucket_set_state(

		GFXBucket*     bucket,
		size_t         unit,
		GFXBatchState  state)
{
	struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;
	struct GFX_Unit* un = _gfx_bucket_ref_get(internal, unit);

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

		GFXBucket*  bucket,
		size_t      unit,
		int         visible)
{
	struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;
	struct GFX_Unit* un = _gfx_bucket_ref_get(internal, unit);

	if(!(un->state & GFX_INT_UNIT_ERASE))
	{
		visible = visible ? 1 : 0;
		int cur = (un->state & GFX_INT_UNIT_VISIBLE) ? 1 : 0;

		if(visible != cur) internal->flags |= GFX_INT_BUCKET_PROCESS_UNITS | GFX_INT_BUCKET_SORT;

		if(visible) un->state |= GFX_INT_UNIT_VISIBLE;
		else un->state &= ~GFX_INT_UNIT_VISIBLE;
	}
}

/******************************************************/
void gfx_bucket_erase(

		GFXBucket*  bucket,
		size_t      unit)
{
	struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;
	struct GFX_Unit* un = _gfx_bucket_ref_get(internal, unit);

	_gfx_bucket_erase_unit(internal, un);
}
