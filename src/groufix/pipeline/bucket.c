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
#include "groufix/pipeline.h"
#include "groufix/window.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Batch pair */
struct GFX_Internal_Batch
{
	GFXBatchUnit* first;
	GFXBatchUnit* last;
};

/* Internal bucket */
struct GFX_Internal_Bucket
{
	/* Super class */
	GFXBucket bucket;

	/* Hidden data */
	unsigned char  bit;     /* Index of the max bit to sort by */
	GFXBatchUnit*  first;   /* Begin of units */
	GFXBatchUnit*  last;    /* End of units */
	GFXVector      batches; /* Batch shortcuts, stores GFX_Internal_Batch */
};

/* Internal batch unit */
struct GFX_Internal_Unit
{
	/* Super class */
	GFXBatchUnit unit;

	/* Hidden data */
	GFXBatchState  state;
	GFXBucket*     bucket;
};

/******************************************************/
static void _gfx_bucket_radix_sort(GFXBatchState bit, GFXBatchUnit** first, GFXBatchUnit** last, struct GFX_Internal_Bucket* bucket)
{
	/* Nothing to sort */
	if(*first == *last || !bit)
	{
		/* Insert as batch */
		struct GFX_Internal_Batch batch = { *first, *last };
		gfx_vector_insert(&bucket->batches, &batch, bucket->batches.end);
	}
	else
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

			_gfx_bucket_radix_sort(bit, first, &mid->previous, bucket);
			_gfx_bucket_radix_sort(bit, &mid, last, bucket);
		}
		else _gfx_bucket_radix_sort(bit, first, last, bucket);
	}
}

/******************************************************/
static GFXBatchState _gfx_bucket_add_manual_state(const struct GFX_Internal_Bucket* bucket, GFXBatchState state, GFXBatchState original)
{
	/* Get manual bit mask and factor in */
	GFXBatchState mask;
	if(!(bucket->bucket.flags & GFX_BUCKET_SORT_BITS_LAST))
	{
		/* Apply black magic to shift mask and state to the left */
		unsigned char shifts = bucket->bit - (bucket->bucket.bits - 1);
		mask = ~((1 << shifts) - 1);
		state <<= shifts;
	}
	else mask = (1 << bucket->bucket.bits) - 1;

	/* Sitch the masks together */
	return (state & mask) | (original & ~mask);
}

/******************************************************/
static GFXBatchState _gfx_bucket_get_manual_state(const struct GFX_Internal_Bucket* bucket, GFXBatchState state)
{
	/* Apply reverse black magic to unshift the state */
	if(!(bucket->bucket.flags & GFX_BUCKET_SORT_BITS_LAST))
		state >>= bucket->bit - (bucket->bucket.bits - 1);

	/* Mask it out */
	return state & ((1 << bucket->bucket.bits) - 1);
}

/******************************************************/
GFXBucket* _gfx_bucket_create(unsigned char bits, GFXBatchProcessFunc process, GFXBucketFlags flags)
{
	/* Allocate bucket */
	struct GFX_Internal_Bucket* bucket = calloc(1, sizeof(struct GFX_Internal_Bucket));
	if(!bucket) return NULL;

	/* Apply sorting flags & bits */
	unsigned char maxID = gfx_hardware_get_max_id_width();
	unsigned char intern = 0;
	unsigned char manual = sizeof(GFXBatchState) << 3;

	intern += (flags & GFX_BUCKET_SORT_PROGRAM) ? maxID : 0;
	intern += (flags & GFX_BUCKET_SORT_VERTEX_LAYOUT) ? maxID : 0;
	manual -= intern;

	/* Subtract one to be able to use it as index */
	bucket->bucket.bits = (bits > manual) ? manual : bits;
	bucket->bit = intern + bucket->bucket.bits - 1;

	/* Init bucket */
	gfx_vector_init(&bucket->batches, sizeof(struct GFX_Internal_Batch));
	bucket->bucket.process = process;
	bucket->bucket.flags = flags;

	return (GFXBucket*)bucket;
}

/******************************************************/
void _gfx_bucket_free(GFXBucket* bucket)
{
	if(bucket)
	{
		struct GFX_Internal_Bucket* internal = (struct GFX_Internal_Bucket*)bucket;

		gfx_list_free(internal->first);
		gfx_vector_clear(&internal->batches);

		free(bucket);
	}
}

/******************************************************/
void _gfx_bucket_process(GFXBucket* bucket)
{
	struct GFX_Internal_Bucket* internal = (struct GFX_Internal_Bucket*)bucket;

	/* Check if sort is needed */
	if((internal->batches.begin == internal->batches.end) && internal->first)
		_gfx_bucket_radix_sort(1 << internal->bit, &internal->first, &internal->last, internal);

	/* Process */
	GFXVectorIterator it;
	for(it = internal->batches.begin; it != internal->batches.end; it = gfx_vector_next(&internal->batches, it))
	{
		struct GFX_Internal_Batch* batch = it;
		bucket->process(((struct GFX_Internal_Unit*)batch->first)->state, batch->first, batch->last);
	}
}

/******************************************************/
GFXBatchUnit* gfx_bucket_insert(GFXBucket* bucket, GFXBatchState state, size_t dataSize)
{
	struct GFX_Internal_Bucket* internal = (struct GFX_Internal_Bucket*)bucket;
	size_t size = sizeof(struct GFX_Internal_Unit) + dataSize;

	/* Create unit */
	struct GFX_Internal_Unit* unit = (struct GFX_Internal_Unit*)gfx_list_create(size);
	if(!unit) return NULL;

	unit->state = _gfx_bucket_add_manual_state(internal, state, 0);
	unit->bucket = bucket;

	/* Insert unit */
	if(!internal->first)
	{
		internal->first = (GFXBatchUnit*)unit;
		internal->last = (GFXBatchUnit*)unit;
	}
	else
	{
		gfx_list_splice_before((GFXBatchUnit*)unit, internal->first);
		internal->first = (GFXBatchUnit*)unit;
	}

	/* Force a re-sort */
	gfx_vector_clear(&internal->batches);

	return (GFXBatchUnit*)unit;
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
		gfx_vector_clear(&bucket->batches);
	}
}

/******************************************************/
void* gfx_bucket_get_data(GFXBatchUnit* unit)
{
	return ((struct GFX_Internal_Unit*)unit) + 1;
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
	gfx_vector_clear(&bucket->batches);
}
