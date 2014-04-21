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

#include "groufix/core/errors.h"
#include "groufix/core/pipeline/internal.h"
#include "groufix/core/memory/internal.h"
#include "groufix/core/shading/internal.h"
#include "groufix/containers/vector.h"

#include <stdlib.h>
#include <string.h>

/* Internal bucket flags */
#define GFX_INT_BUCKET_PROCESS_UNITS  0x01
#define GFX_INT_BUCKET_SORT           0x02

/* Internal unit action (for processing) */
#define GFX_INT_UNIT_VISIBLE          0x01
#define GFX_INT_UNIT_ERASE            0x02

/******************************************************/
/* Internal bucket */
struct GFX_Bucket
{
	/* Super class */
	GFXBucket bucket;

	/* Hidden data */
	unsigned char      bit;      /* Index of the max bit to sort by */
	unsigned char      keyWidth; /* bit width of program/layout keys */
	unsigned char      flags;

	GFXVector          sources;  /* Stores GFX_Source */
	GFXVector          refs;     /* References to units (units[refs[ID - 1] - 1] = unit, 0 is empty) */
	GFXVector          units;    /* Stores GFX_Unit */
	GFXVectorIterator  visible;  /* Everything after is not visible */

	/* Keys mappings (arr[key - 1] = hardware id) */
	GFXVector          programKeys;
	GFXVector          layoutKeys;
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
	GFXBatchState    state;  /* Combination of vertex layout key, program key and manual state */
	unsigned char    action;
	size_t           ref;    /* Reference of the unit units[refs[ref] - 1] = this (const, equal to ID - 1) */

	GFXPropertyMap*  map;
	size_t           copy;   /* Copy of the property map to use */

	size_t           src;    /* Source of the bucket to use (ID - 1) */
	size_t           inst;   /* Number of instances */
	unsigned int     base;   /* Base instance for instanced vertex attributes */
	GFX_DrawType     type;
};

/******************************************************/
static size_t _gfx_bucket_map_key(

		struct GFX_Bucket*  bucket,
		GFXVector*          vec,
		size_t              id)
{
	/* Already existing key */
	GFXVectorIterator it;
	for(it = vec->begin; it != vec->end; it = gfx_vector_next(vec, it))
	{
		if(*(size_t*)it == id) return gfx_vector_get_index(vec, it) + 1;
	}

	/* Append new mapping */
	size_t key = gfx_vector_get_size(vec) + 1;

	/* Awmg, too big! */
	if(key > ((size_t)1 << bucket->keyWidth) - 1)
	{
		gfx_errors_push(
			GFX_ERROR_OVERFLOW,
			"Too many units have been inserted into a bucket, key overflow happened, you crazy person."
		);
		return 0;
	}

	if(gfx_vector_insert(vec, &id, vec->end) == vec->end) return 0;

	return key;
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
		struct GFX_Unit*    unit)
{
	GFXVectorIterator it = gfx_vector_at(&bucket->refs, unit->ref);
	*(size_t*)it = 0;

	/* Erase trailing zeros */
	if(unit->ref + 1 >= gfx_vector_get_size(&bucket->refs))
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
static void _gfx_bucket_swap_units(

		struct GFX_Bucket*  bucket,
		struct GFX_Unit*    unit1,
		struct GFX_Unit*    unit2)
{
	struct GFX_Unit temp = *unit1;
	*unit1 = *unit2;
	*unit2 = temp;

	/* Correct references */
	*(size_t*)gfx_vector_at(&bucket->refs, unit1->ref) =
		gfx_vector_get_index(&bucket->units, unit1) + 1;
	*(size_t*)gfx_vector_at(&bucket->refs, unit2->ref) =
		gfx_vector_get_index(&bucket->units, unit2) + 1;
}

/******************************************************/
static void _gfx_bucket_erase_unit(

		struct GFX_Bucket*  bucket,
		struct GFX_Unit*    unit)
{
	bucket->flags |= GFX_INT_BUCKET_PROCESS_UNITS;

	if(unit->action & GFX_INT_UNIT_VISIBLE)
		bucket->flags |= GFX_INT_BUCKET_SORT;

	unit->action = GFX_INT_UNIT_ERASE;
}

/******************************************************/
static void _gfx_bucket_process_units(

		struct GFX_Bucket* bucket)
{
	/* Iterate and filter visible */
	GFXVectorIterator end = bucket->units.end;
	GFXVectorIterator it = bucket->units.begin;
	size_t visible = 0;

	while(it != end)
	{
		/* Swap out if not visible */
		if(!(GFX_INT_UNIT_VISIBLE & ((struct GFX_Unit*)it)->action))
		{
			end = gfx_vector_previous(&bucket->units, end);
			_gfx_bucket_swap_units(bucket, it, end);
		}
		else
		{
			it = gfx_vector_next(&bucket->units, it);
			++visible;
		}
	}

	/* Iterate again and filter the ones to be erased */
	end = bucket->units.end;
	size_t erased = 0;

	while(it != end)
	{
		/* Swap out if it should be erased */
		if(GFX_INT_UNIT_ERASE & ((struct GFX_Unit*)it)->action)
		{
			end = gfx_vector_previous(&bucket->units, end);
			_gfx_bucket_swap_units(bucket, it, end);

			/* Erase precautions */
			_gfx_bucket_erase_ref(bucket, end);
			++erased;
		}
		else it = gfx_vector_next(&bucket->units, it);
	}

	/* Erase the ones to be erased :D */
	gfx_vector_erase_range(&bucket->units, erased, it);
	bucket->visible = gfx_vector_advance(
		&bucket->units,
		bucket->units.begin,
		visible
	);
}

/******************************************************/
static void _gfx_bucket_radix_sort(

		struct GFX_Bucket*  bucket,
		GFXBatchState       bit,
		GFXVectorIterator   begin,
		GFXVectorIterator   end)
{
	/* Skip on range of 1 or no bit */
	if(GFX_PTR_DIFF(begin, end) > sizeof(struct GFX_Unit) && bit)
	{
		GFXVectorIterator mid = end;
		GFXVectorIterator it = begin;

		while(it != mid)
		{
			/* If 1, put in 1 bucket */
			if(((struct GFX_Unit*)it)->state & bit)
			{
				mid = gfx_vector_previous(&bucket->units, mid);
				_gfx_bucket_swap_units(bucket, it, mid);
			}
			else it = gfx_vector_next(&bucket->units, it);
		}

		/* Sort both buckets */
		bit >>= 1;
		_gfx_bucket_radix_sort(bucket, bit, begin, mid);
		_gfx_bucket_radix_sort(bucket, bit, mid, end);
	}
}

/******************************************************/
static inline GFXBatchState _gfx_bucket_add_manual_state(

		const struct GFX_Bucket*  bucket,
		GFXBatchState             state,
		GFXBatchState             original)
{
	/* Apply black magic to shift mask and state */
	unsigned char shifts = bucket->bit - (bucket->bucket.bits - 1);
	GFXBatchState mask = (1 << shifts) - 1;
	state <<= shifts;

	/* Stitch the masks together */
	return (state & ~mask) | (original & mask);
}

/******************************************************/
static inline GFXBatchState _gfx_bucket_get_manual_state(

		const struct GFX_Bucket*  bucket,
		GFXBatchState             state)
{
	/* Apply reverse black magic to unshift the state */
	state >>= bucket->bit - (bucket->bucket.bits - 1);

	/* Mask it out */
	return state & ((1 << bucket->bucket.bits) - 1);
}

/******************************************************/
static GFXBatchState _gfx_bucket_create_state(

		const struct GFX_Bucket*  bucket,
		GFXBatchState             manual,
		size_t                    layout,
		size_t                    program)
{
	/* Create state containing vertex layout/program key */
	GFXBatchState state = 0;
	size_t shifts = 0;

	if(bucket->bucket.flags & GFX_BUCKET_SORT_VERTEX_LAYOUT)
	{
		state |= layout;
		shifts = bucket->keyWidth;
	}
	if(bucket->bucket.flags & GFX_BUCKET_SORT_PROGRAM)
	{
		state |= (GFXBatchState)program << shifts;
	}

	/* Add manual state to it */
	return _gfx_bucket_add_manual_state(bucket, manual, state);
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
	gfx_vector_init(&bucket->programKeys, sizeof(size_t));
	gfx_vector_init(&bucket->layoutKeys, sizeof(size_t));

	bucket->visible = bucket->units.end;

	/* Apply sorting flags & bits */
	bucket->bucket.flags = flags;
	gfx_bucket_set_key_width((GFXBucket*)bucket, GFX_BUCKET_KEY_WIDTH_DEFAULT, bits);

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
		gfx_vector_clear(&internal->programKeys);
		gfx_vector_clear(&internal->layoutKeys);

		free(bucket);
	}
}

/******************************************************/
void _gfx_bucket_process(

		GFXBucket*       bucket,
		GFX_State*       state,
		GFX_Extensions*  ext)
{
	struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;

	/* Handle the flags */
	if(internal->flags & GFX_INT_BUCKET_PROCESS_UNITS)
		_gfx_bucket_process_units(internal);

	if(internal->flags & GFX_INT_BUCKET_SORT)
		_gfx_bucket_radix_sort(
			internal,
			(GFXBatchState)1 << internal->bit,
			internal->units.begin,
			internal->visible
		);

	internal->flags = 0;

	/* Set states and process */
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
void gfx_bucket_set_key_width(

		GFXBucket*     bucket,
		unsigned char  width,
		unsigned char  bits)
{
	struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;

	/* Check maps */
	if(internal->programKeys.begin == internal->programKeys.end &&
		internal->layoutKeys.begin == internal->layoutKeys.end)
	{
		/* Change width, maximum is the state / 2 (or sizeof(state) * 8 / 2) */
		unsigned char max = sizeof(GFXBatchState) << 2;
		internal->keyWidth = width > max ? max : width;

		unsigned char intern = 0;
		unsigned char manual = max << 1;

		intern += (bucket->flags & GFX_BUCKET_SORT_PROGRAM) ? internal->keyWidth : 0;
		intern += (bucket->flags & GFX_BUCKET_SORT_VERTEX_LAYOUT) ? internal->keyWidth : 0;
		manual -= intern;

		/* Subtract one to be able to use it as index */
		bucket->bits = (bits > manual) ? manual : bits;
		internal->bit = intern + bucket->bits - 1;
	}
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

	/* Map vertex layout and program key */
	size_t layKey = _gfx_bucket_map_key(
		internal,
		&internal->layoutKeys,
		source->layout->id);

	size_t proKey = _gfx_bucket_map_key(
		internal,
		&internal->programKeys,
		map->program->id);

	if(!layKey || !proKey) return 0;

	/* Insert the new unit */
	struct GFX_Unit unit;

	unit.state  = _gfx_bucket_create_state(internal, state, layKey, proKey);
	unit.action = visible ? GFX_INT_UNIT_VISIBLE : 0;
	unit.map    = map;
	unit.copy   = 0;
	unit.src    = src;
	unit.inst   = 1;
	unit.base   = 0;

	GFXVectorIterator it = gfx_vector_insert(
		&internal->units,
		&unit,
		internal->units.end
	);
	if(it == internal->units.end) return 0;

	/* Force to process */
	internal->flags |= GFX_INT_BUCKET_PROCESS_UNITS;

	/* Insert a reference for it */
	size_t id = _gfx_bucket_insert_ref(
		internal,
		gfx_vector_get_index(&internal->units, it)
	);

	if(!id)
	{
		/* Erase it */
		gfx_vector_erase(&internal->units, it);
		return 0;
	}

	/* Set reference and draw type and force to sort if necessary */
	((struct GFX_Unit*)it)->ref = id - 1;
	_gfx_bucket_set_draw_type(it);

	if(visible) internal->flags |= GFX_INT_BUCKET_SORT;

	return id;
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
	return _gfx_bucket_get_manual_state(internal, _gfx_bucket_ref_get(internal, unit)->state);
}

/******************************************************/
int gfx_bucket_is_visible(

		GFXBucket*  bucket,
		size_t      unit)
{
	return _gfx_bucket_ref_get((struct GFX_Bucket*)bucket, unit)->action & GFX_INT_UNIT_VISIBLE;
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

	if(!(un->action & GFX_INT_UNIT_ERASE))
	{
		/* Detect equal states */
		state = _gfx_bucket_add_manual_state(internal, state, un->state);
		if(un->state != state && (un->action & GFX_INT_UNIT_VISIBLE))
		{
			/* Force a re-sort if visible */
			internal->flags |= GFX_INT_BUCKET_SORT;
		}
		un->state = state;
	}
}

/******************************************************/
void gfx_bucket_set_visible(

		GFXBucket*  bucket,
		size_t      unit,
		int         visible)
{
	struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;
	struct GFX_Unit* un = _gfx_bucket_ref_get(internal, unit);

	if(!(un->action & GFX_INT_UNIT_ERASE))
	{
		visible = visible ? 1 : 0;
		int cur = (un->action & GFX_INT_UNIT_VISIBLE) ? 1 : 0;

		if(visible != cur) internal->flags |= GFX_INT_BUCKET_PROCESS_UNITS | GFX_INT_BUCKET_SORT;

		if(visible) un->action |= GFX_INT_UNIT_VISIBLE;
		else un->action &= ~GFX_INT_UNIT_VISIBLE;
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
