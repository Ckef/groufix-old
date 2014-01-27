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

#include "groufix/containers/vector.h"
#include "groufix/memory/internal.h"
#include "groufix/pipeline/internal.h"
#include "groufix/shading/internal.h"

#include <stdlib.h>
#include <string.h>

/* Internal bucket flags */
#define GFX_INT_BUCKET_SORT   0x01
#define GFX_INT_BUCKET_BATCH  0x02

/******************************************************/
/* Internal bucket */
struct GFX_Bucket
{
	/* Super class */
	GFXBucket bucket;

	/* Hidden data */
	unsigned char  bit;       /* Index of the max bit to sort by */
	unsigned char  flags;

	GFXBatchUnit*  first;     /* Begin of units */
	GFXBatchUnit*  last;      /* End of units */
	GFXBatchUnit*  invisible; /* All invisible units */

	/* Drawing data */
	GFXVector      sources;
	GFXVector      batches;
};

/* Internal source */
struct GFX_Source
{
	GFXPropertyMap*   map;
	GFXVertexLayout*  layout;

	GFXBatchMode      mode;
	unsigned char     start;
	unsigned char     num;
};

/* Internal batch */
struct GFX_Batch
{
	size_t src;  /* Source of the bucket to use (ID - 1) */
	size_t inst; /* Number of instances */
};

/* Internal batch unit */
struct GFX_Unit
{
	/* Super class */
	GFXBatchUnit unit;

	/* Hidden data */
	GFXBatchState  state;
	GFXBucket*     bucket;
	size_t         src;  /* Source of the bucket to use (ID - 1) */
	size_t         inst; /* Number of instances */
};

/******************************************************/
static void _gfx_bucket_radix_sort(GFXBatchState bit, GFXBatchUnit** first, GFXBatchUnit** last)
{
	/* Nothing to sort */
	if(*first != *last && bit)
	{
		/* Loop over all entries */
		GFXBatchUnit* mid = *last; /* last to iterate to */
		GFXBatchUnit* cur = *first;

		while(cur != mid)
		{
			GFXBatchUnit* next = cur->next;

			/* If 1, put in 1 bucket */
			if(((struct GFX_Unit*)cur)->state & bit)
			{
				*first = (*first == cur) ? next : *first;
				gfx_list_splice_after(cur, *last);
				*last = cur;
			}
			cur = next;
		}

		/* Implicitly sort the last unit */
		int nonZero = ((struct GFX_Unit*)mid)->state & bit;
		bit >>= 1;

		if(mid != (nonZero ? *first : *last))
		{
			/* Force it to first sort the 0s bucket, so batches are in order */
			mid = nonZero ? mid : mid->next;

			_gfx_bucket_radix_sort(bit, first, &mid->previous);
			_gfx_bucket_radix_sort(bit, &mid, last);
		}
		else _gfx_bucket_radix_sort(bit, first, last);
	}
}

/******************************************************/
static void _gfx_bucket_create_batches(struct GFX_Bucket* bucket)
{
	/* Empty all */
	gfx_vector_clear(&bucket->batches);

	/* Iterate over all units */
	GFXBatchUnit* beg = bucket->first;
	GFXBatchUnit* cur = bucket->first;

	struct GFX_Batch batch;
	batch.inst = 0;

	while(cur)
	{
		/* Create new batch and reset */
		if(((struct GFX_Unit*)cur)->src != ((struct GFX_Unit*)beg)->src)
		{
			batch.src = ((struct GFX_Unit*)beg)->src;
			gfx_vector_insert(&bucket->batches, &batch, bucket->batches.end);

			batch.inst = 0;
		}

		/* Increase instances and continue */
		batch.inst += ((struct GFX_Unit*)cur)->inst;
		cur = cur->next;
	}

	/* Insert last batch */
	if(batch.inst)
	{
		batch.src = ((struct GFX_Unit*)beg)->src;
		gfx_vector_insert(&bucket->batches, &batch, bucket->batches.end);
	}
}

/******************************************************/
void _gfx_bucket_process(GFXBucket* bucket, GFXPipeState state, GFX_Extensions* ext)
{
	struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;

	/* Handle the flags */
	if(internal->flags & GFX_INT_BUCKET_SORT)
		_gfx_bucket_radix_sort(1 << internal->bit, &internal->first, &internal->last);

	if(internal->flags & GFX_INT_BUCKET_BATCH)
		_gfx_bucket_create_batches(internal);

	internal->flags = 0;

	/* Set states and process */
	_gfx_states_set(state, ext);

	struct GFX_Batch* batch;
	for(batch = internal->batches.begin; batch != internal->batches.end; batch = gfx_vector_next(&internal->batches, batch))
	{
		struct GFX_Source* src = (struct GFX_Source*)gfx_vector_at(&internal->sources, batch->src);

		/* Bind shader program & draw */
		_gfx_property_map_use(src->map, ext);

		switch(src->mode)
		{
			case GFX_BATCH_DIRECT :
				_gfx_vertex_layout_draw(
					src->layout,
					src->start,
					src->num
				);
				break;

			case GFX_BATCH_INDEXED :
				_gfx_vertex_layout_draw_indexed(
					src->layout,
					src->start,
					src->num
				);
				break;

			case GFX_BATCH_DIRECT_INSTANCED :
				_gfx_vertex_layout_draw_instanced(
					src->layout,
					src->start,
					src->num,
					batch->inst
				);
				break;

			case GFX_BATCH_INDEXED_INSTANCED :
				_gfx_vertex_layout_draw_indexed_instanced(
					src->layout,
					src->start,
					src->num,
					batch->inst
				);
				break;
		}
	}
}

/******************************************************/
static GFXBatchState _gfx_bucket_get_state(const struct GFX_Bucket* bucket, size_t layout, size_t program)
{
	GFXBatchState state = 0;

	/* First sort on program, then layout */
	GFXBatchState shifts = 0;
	if(bucket->bucket.flags & GFX_BUCKET_SORT_VERTEX_LAYOUT)
	{
		state |= layout;
		shifts = gfx_hardware_get_max_id_width();
	}
	if(bucket->bucket.flags & GFX_BUCKET_SORT_PROGRAM)
	{
		state |= (GFXBatchState)program << shifts;
	}

	return state;
}

/******************************************************/
static inline GFXBatchState _gfx_bucket_add_manual_state(const struct GFX_Bucket* bucket, GFXBatchState state, GFXBatchState original)
{
	/* Apply black magic to shift mask and state */
	unsigned char shifts = bucket->bit - (bucket->bucket.bits - 1);
	GFXBatchState mask = (1 << shifts) - 1;
	state <<= shifts;

	/* Stitch the masks together */
	return (state & ~mask) | (original & mask);
}

/******************************************************/
static inline GFXBatchState _gfx_bucket_get_manual_state(const struct GFX_Bucket* bucket, GFXBatchState state)
{
	/* Apply reverse black magic to unshift the state */
	state >>= bucket->bit - (bucket->bucket.bits - 1);

	/* Mask it out */
	return state & ((1 << bucket->bucket.bits) - 1);
}

/******************************************************/
GFXBucket* _gfx_bucket_create(unsigned char bits, GFXBucketFlags flags)
{
	/* Allocate bucket */
	struct GFX_Bucket* bucket = calloc(1, sizeof(struct GFX_Bucket));
	if(!bucket) return NULL;

	/* Apply sorting flags & bits */
	unsigned char idWidth = gfx_hardware_get_max_id_width();
	unsigned char intern = 0;
	unsigned char manual = sizeof(GFXBatchState) << 3;

	intern += (flags & GFX_BUCKET_SORT_PROGRAM) ? idWidth : 0;
	intern += (flags & GFX_BUCKET_SORT_VERTEX_LAYOUT) ? idWidth : 0;
	manual -= intern;

	/* Subtract one to be able to use it as index */
	bucket->bucket.flags = flags;
	bucket->bucket.bits = (bits > manual) ? manual : bits;
	bucket->bit = intern + bucket->bucket.bits - 1;

	gfx_vector_init(&bucket->sources, sizeof(struct GFX_Source));
	gfx_vector_init(&bucket->batches, sizeof(struct GFX_Batch));

	return (GFXBucket*)bucket;
}

/******************************************************/
void _gfx_bucket_free(GFXBucket* bucket)
{
	if(bucket)
	{
		struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;

		gfx_vector_clear(&internal->sources);
		gfx_vector_clear(&internal->batches);

		gfx_list_free(internal->first);
		gfx_list_free(internal->invisible);

		free(bucket);
	}
}

/******************************************************/
size_t gfx_bucket_add_source(GFXBucket* bucket, GFXPropertyMap* map, GFXVertexLayout* layout)
{
	/* Derp */
	if(!map || !layout) return 0;

	struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;

	/* Create source */
	struct GFX_Source src;
	src.map    = map;
	src.layout = layout;
	src.start  = 0;
	src.num    = 0;

	if(gfx_vector_insert(&internal->sources, &src, internal->sources.end) == internal->sources.end)
		return 0;

	/* Return its index + 1 */
	return gfx_vector_get_size(&internal->sources);
}

/******************************************************/
void gfx_bucket_set_draw_calls(GFXBucket* bucket, size_t src, GFXBatchMode mode, unsigned char start, unsigned char num)
{
	struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;
	struct GFX_Source* source = (struct GFX_Source*)gfx_vector_at(&internal->sources, src - 1);

	source->mode = mode;
	source->start = start;
	source->num = num;
}

/******************************************************/
GFXBatchUnit* gfx_bucket_insert(GFXBucket* bucket, size_t src, GFXBatchState state, int visible)
{
	struct GFX_Bucket* internal = (struct GFX_Bucket*)bucket;
	struct GFX_Source* source = (struct GFX_Source*)gfx_vector_at(&internal->sources, src - 1);

	/* Create unit */
	struct GFX_Unit* unit = (struct GFX_Unit*)gfx_list_create(sizeof(struct GFX_Unit));
	if(!unit) return NULL;

	unit->state = _gfx_bucket_get_state(internal, source->layout->id, source->map->program->id);
	unit->state = _gfx_bucket_add_manual_state(internal, state, unit->state);
	unit->bucket = bucket;

	unit->src = src - 1;
	unit->inst = 1;

	/* Insert unit */
	gfx_bucket_set_visible((GFXBatchUnit*)unit, visible);

	return (GFXBatchUnit*)unit;
}

/******************************************************/
size_t gfx_bucket_get_instances(GFXBatchUnit* unit)
{
	return ((struct GFX_Unit*)unit)->inst;
}

/******************************************************/
GFXBatchState gfx_bucket_get_state(GFXBatchUnit* unit)
{
	struct GFX_Unit* internal = (struct GFX_Unit*)unit;
	struct GFX_Bucket* bucket = (struct GFX_Bucket*)internal->bucket;

	return _gfx_bucket_get_manual_state(bucket, internal->state);
}

/******************************************************/
void gfx_bucket_set_instances(GFXBatchUnit* unit, size_t instances)
{
	struct GFX_Unit* internal = (struct GFX_Unit*)unit;
	struct GFX_Bucket* bucket = (struct GFX_Bucket*)internal->bucket;

	internal->inst = instances > 0 ? instances : 1;
	bucket->flags |= GFX_INT_BUCKET_BATCH;
}

/******************************************************/
void gfx_bucket_set_state(GFXBatchUnit* unit, GFXBatchState state)
{
	struct GFX_Unit* internal = (struct GFX_Unit*)unit;
	struct GFX_Bucket* bucket = (struct GFX_Bucket*)internal->bucket;

	/* Detect equal states */
	state = _gfx_bucket_add_manual_state(bucket, state, internal->state);
	if(internal->state != state)
	{
		/* Force a re-sort */
		internal->state = state;
		bucket->flags |= GFX_INT_BUCKET_SORT | GFX_INT_BUCKET_BATCH;
	}
}

/******************************************************/
void gfx_bucket_set_visible(GFXBatchUnit* unit, int visible)
{
	struct GFX_Unit* internal = (struct GFX_Unit*)unit;
	struct GFX_Bucket* bucket = (struct GFX_Bucket*)internal->bucket;

	/* First remove it from any list */
	if(bucket->invisible == unit) bucket->invisible = unit->next;
	if(bucket->first == unit) bucket->first = unit->next;
	if(bucket->last == unit) bucket->last = unit->previous;

	gfx_list_unsplice(unit, unit);

	if(visible)
	{
		/* Insert into batch list */
		if(!bucket->last) bucket->last = unit;
		else gfx_list_splice_before(unit, bucket->first);

		bucket->first = unit;

		/* Force a re-sort */
		bucket->flags |= GFX_INT_BUCKET_SORT;
	}
	else
	{
		/* Insert into invisible list */
		if(!bucket->invisible) bucket->invisible = unit;
		else gfx_list_splice_after(unit, bucket->invisible);
	}

	bucket->flags |= GFX_INT_BUCKET_BATCH;
}

/******************************************************/
void gfx_bucket_erase(GFXBatchUnit* unit)
{
	struct GFX_Unit* internal = (struct GFX_Unit*)unit;
	struct GFX_Bucket* bucket = (struct GFX_Bucket*)internal->bucket;

	/* Erase it */
	GFXBatchUnit* new = gfx_list_erase(unit);

	/* Replace first or last if necessary */
	if(bucket->invisible == unit) bucket->invisible = new;
	if(bucket->first == unit) bucket->first = new;
	if(bucket->last == unit) bucket->last = new;

	bucket->flags |= GFX_INT_BUCKET_BATCH;
}
