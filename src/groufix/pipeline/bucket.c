/**
 * Groufix  :  Graphics Engine produced by Ckef Worx
 * www      :  http://www.ejb.ckef-worx.com
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "groufix/containers/vector.h"
#include "groufix/pipeline/internal.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Internal bucket */
struct GFX_Internal_Bucket
{
	/* Super class */
	GFXBucket bucket;

	/* Hidden data */
	unsigned char  bit;   /* Index of the max bit to sort by */
	unsigned char  sort;  /* Non zero if a sort is required */
	GFXBatchUnit*  first; /* Begin of units */
	GFXBatchUnit*  last;  /* End of units */

	/* All invisible units */
	GFXBatchUnit*  invisible;
};

/* Internal batch unit */
struct GFX_Internal_Unit
{
	/* Super class */
	GFXBatchUnit unit;

	/* Hidden data */
	GFXBatchState     state;
	GFXBucket*        bucket;

	/* Drawing data */
	GFXVertexLayout*  layout;
	GLuint            program;

	GFXBatchMode      mode;
	unsigned char     start;
	unsigned char     num;
	size_t            inst;
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
			if(((struct GFX_Internal_Unit*)cur)->state & bit)
			{
				*first = (*first == cur) ? next : *first;
				gfx_list_splice_after(cur, *last);
				*last = cur;
			}
			cur = next;
		}

		/* Implicitely sort the last unit */
		int nonZero = ((struct GFX_Internal_Unit*)mid)->state & bit;
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
static inline GFXBatchState _gfx_bucket_add_manual_state(const struct GFX_Internal_Bucket* bucket, GFXBatchState state, GFXBatchState original)
{
	/* Apply black magic to shift mask and state */
	unsigned char shifts = bucket->bit - (bucket->bucket.bits - 1);
	GFXBatchState mask = (1 << shifts) - 1;
	state <<= shifts;

	/* Sitch the masks together */
	return (state & ~mask) | (original & mask);
}

/******************************************************/
static inline GFXBatchState _gfx_bucket_get_manual_state(const struct GFX_Internal_Bucket* bucket, GFXBatchState state)
{
	/* Apply reverse black magic to unshift the state */
	state >>= bucket->bit - (bucket->bucket.bits - 1);

	/* Mask it out */
	return state & ((1 << bucket->bucket.bits) - 1);
}

/******************************************************/
static GFXBatchState _gfx_bucket_compute_state(const struct GFX_Internal_Bucket* bucket, GFXBatchState state, size_t layout, size_t program)
{
	/* First sort on layout, then program */
	GFXBatchState orig;
	if(bucket->bucket.flags & GFX_BUCKET_SORT_VERTEX_LAYOUT)
	{
		/* Also, pretend it always sorts on program */
		orig = layout;
		orig |= (GFXBatchState)program << gfx_hardware_get_max_id_width();
	}
	else orig = program;

	/* It will get masked away if not anyway */
	return _gfx_bucket_add_manual_state(bucket, state, orig);
}

/******************************************************/
GFXBucket* _gfx_bucket_create(unsigned char bits, GFXBucketFlags flags)
{
	/* Allocate bucket */
	struct GFX_Internal_Bucket* bucket = calloc(1, sizeof(struct GFX_Internal_Bucket));
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

	return (GFXBucket*)bucket;
}

/******************************************************/
void _gfx_bucket_free(GFXBucket* bucket)
{
	if(bucket)
	{
		struct GFX_Internal_Bucket* internal = (struct GFX_Internal_Bucket*)bucket;

		gfx_list_free(internal->first);
		gfx_list_free(internal->invisible);
		free(bucket);
	}
}

/******************************************************/
void _gfx_bucket_process(GFXBucket* bucket, const GFX_Extensions* ext)
{
	struct GFX_Internal_Bucket* internal = (struct GFX_Internal_Bucket*)bucket;
	if(!internal->first) return;

	/* Check if sort is needed */
	if(internal->sort)
	{
		_gfx_bucket_radix_sort(1 << internal->bit, &internal->first, &internal->last);
		internal->sort = 0;
	}

	/* Process */
	GFXBatchUnit* curr;
	for(curr = internal->first; curr; curr = curr->next)
	{
		struct GFX_Internal_Unit* unit = (struct GFX_Internal_Unit*)curr;

		/* Bind shader program & draw */
		_gfx_program_use(unit->program, ext);

		switch(unit->mode)
		{
			case GFX_BATCH_DIRECT :
				_gfx_vertex_layout_draw(
					unit->layout,
					unit->start,
					unit->num
				);
				break;

			case GFX_BATCH_INDEXED :
				_gfx_vertex_layout_draw_indexed(
					unit->layout,
					unit->start,
					unit->num
				);
				break;

			case GFX_BATCH_DIRECT_INSTANCED :
				_gfx_vertex_layout_draw_instanced(
					unit->layout,
					unit->start,
					unit->num,
					unit->inst
				);
				break;

			case GFX_BATCH_INDEXED_INSTANCED :
				_gfx_vertex_layout_draw_indexed_instanced(
					unit->layout,
					unit->start,
					unit->num,
					unit->inst
				);
				break;
		}
	}
}

/******************************************************/
GFXBatchUnit* gfx_bucket_insert(GFXBucket* bucket, GFXBatchState state, GFXVertexLayout* layout, GFXProgram* program, unsigned char visible)
{
	struct GFX_Internal_Bucket* internal = (struct GFX_Internal_Bucket*)bucket;

	/* Create unit */
	struct GFX_Internal_Unit* unit = (struct GFX_Internal_Unit*)gfx_list_create(sizeof(struct GFX_Internal_Unit));
	if(!unit) return NULL;

	unit->state   = _gfx_bucket_compute_state(internal, state, layout->id, program->id);
	unit->bucket  = bucket;
	unit->layout  = layout;
	unit->program = _gfx_program_get_handle(program);

	/* Insert unit */
	gfx_bucket_set_visible((GFXBatchUnit*)unit, visible);

	return (GFXBatchUnit*)unit;
}

/******************************************************/
void gfx_bucket_set_mode(GFXBatchUnit* unit, GFXBatchMode mode, unsigned char start, unsigned char num, size_t inst)
{
	struct GFX_Internal_Unit* internal = (struct GFX_Internal_Unit*)unit;

	/* Disable instancing when only 1 instance */
	if(inst < 2)
	{
		switch(mode)
		{
			case GFX_BATCH_DIRECT :
			case GFX_BATCH_DIRECT_INSTANCED :
				mode = GFX_BATCH_DIRECT;
				break;

			case GFX_BATCH_INDEXED :
			case GFX_BATCH_INDEXED_INSTANCED :
				mode = GFX_BATCH_INDEXED;
				break;
		}
		inst = 1;
	}

	/* Set mode */
	internal->mode  = mode;
	internal->start = start;
	internal->num   = num;
	internal->inst  = inst;
}

/******************************************************/
GFXBatchState gfx_bucket_get_state(const GFXBatchUnit* unit)
{
	const struct GFX_Internal_Unit* internal = (const struct GFX_Internal_Unit*)unit;
	const struct GFX_Internal_Bucket* bucket = (const struct GFX_Internal_Bucket*)internal->bucket;

	return _gfx_bucket_get_manual_state(bucket, internal->state);
}

/******************************************************/
void gfx_bucket_set_state(GFXBatchUnit* unit, GFXBatchState state)
{
	struct GFX_Internal_Unit* internal = (struct GFX_Internal_Unit*)unit;
	struct GFX_Internal_Bucket* bucket = (struct GFX_Internal_Bucket*)internal->bucket;

	/* Detect equal states */
	state = _gfx_bucket_add_manual_state(bucket, state, internal->state);
	if(internal->state != state)
	{
		/* Force a re-sort */
		internal->state = state;
		bucket->sort = 1;
	}
}

/******************************************************/
void gfx_bucket_set_visible(GFXBatchUnit* unit, unsigned char visible)
{
	struct GFX_Internal_Unit* internal = (struct GFX_Internal_Unit*)unit;
	struct GFX_Internal_Bucket* bucket = (struct GFX_Internal_Bucket*)internal->bucket;

	/* First remove it from any list */
	if(bucket->invisible == unit) bucket->invisible = unit->next;
	if(bucket->first == unit) bucket->first = unit->next;
	if(bucket->last == unit) bucket->last = unit->previous;

	gfx_list_unsplice(unit, unit);

	/* Insert into batch list */
	if(visible)
	{
		if(!bucket->last) bucket->last = unit;
		else gfx_list_splice_before(unit, bucket->first);

		bucket->first = unit;

		/* Force a re-sort */
		bucket->sort = 1;
	}

	/* Insert into invisible list */
	else
	{
		if(!bucket->invisible) bucket->invisible = unit;
		else gfx_list_splice_after(unit, bucket->invisible);
	}
}

/******************************************************/
void gfx_bucket_erase(GFXBatchUnit* unit)
{
	struct GFX_Internal_Unit* internal = (struct GFX_Internal_Unit*)unit;
	struct GFX_Internal_Bucket* bucket = (struct GFX_Internal_Bucket*)internal->bucket;

	/* Erase it */
	GFXBatchUnit* new = gfx_list_erase(unit);

	/* Replace first or last if necessary */
	if(bucket->first == unit) bucket->first = new;
	if(bucket->last == unit) bucket->last = new;

	/* Force a re-sort */
	bucket->sort = 1;
}
