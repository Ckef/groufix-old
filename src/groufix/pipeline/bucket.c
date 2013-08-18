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

#include "groufix/pipeline.h"
#include "groufix/containers/vector.h"

#include <stdlib.h>

/******************************************************/
/* Batch pair */
struct GFX_Batch_Pair
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
	GFXBatchState  maxBit;  /* Single bit */
	unsigned char  sort;

	GFXBatchUnit*  first;   /* Begin of units */
	GFXBatchUnit*  last;    /* End of units */
	GFXVector      batches; /* Batch shortcuts, stores GFX_Batch_Pair */
};

/* Internal batch unit */
struct GFX_Internal_Batch
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
		struct GFX_Batch_Pair pair = { *first, *last };
		gfx_vector_insert(&bucket->batches, &pair, bucket->batches.end);

		/* Preprocess */
		if(bucket->bucket.preprocess) bucket->bucket.preprocess(
			((struct GFX_Internal_Batch*)*first)->state, pair.first, pair.last
		);
	}
	else
	{
		/* Loop over all entries */
		GFXBatchUnit* mid = *last; /* last to iterate to */
		GFXBatchUnit* cur = *first;

		while(cur != mid)
		{
			GFXBatchUnit* next = (GFXBatchUnit*)cur->node.next;

			/* If 1, put in 1 bucket */
			if(((struct GFX_Internal_Batch*)cur)->state & bit)
			{
				*first = (*first == cur) ? next : *first;
				gfx_list_splice_after((GFXList*)cur, (GFXList*)*last);
				*last = cur;
			}
			cur = next;
		}

		/* Sort both 0 and 1 buckets */
		int nonZero = ((struct GFX_Internal_Batch*)mid)->state & bit;
		bit >>= 1;

		if(nonZero)
		{
			if(mid != *first) _gfx_bucket_radix_sort(bit, first, (GFXBatchUnit**)&mid->node.previous, bucket);
			_gfx_bucket_radix_sort(bit, &mid, last, bucket);
		}
		else
		{
			if(mid != *last)
			{
				/* Force it to first sort the 0s bucket, so batches are in order */
				mid = (GFXBatchUnit*)mid->node.next;

				_gfx_bucket_radix_sort(bit, first, (GFXBatchUnit**)&mid->node.previous, bucket);
				_gfx_bucket_radix_sort(bit, &mid, last, bucket);
			}
			else _gfx_bucket_radix_sort(bit, first, &mid, bucket);
		}
	}
}

/******************************************************/
GFXBucket* gfx_bucket_create(unsigned char bits, GFXBatchProcessFunc process)
{
	if(!bits) return NULL;

	/* Allocate bucket */
	struct GFX_Internal_Bucket* bucket = calloc(1, sizeof(struct GFX_Internal_Bucket));
	if(!bucket) return NULL;

	gfx_vector_init(&bucket->batches, sizeof(struct GFX_Batch_Pair));
	bucket->bucket.process = process;

	/* Calculate max bit */
	bucket->maxBit = 1;
	while(--bits) bucket->maxBit <<= 1;

	return (GFXBucket*)bucket;
}

/******************************************************/
void gfx_bucket_free(GFXBucket* bucket)
{
	if(bucket)
	{
		struct GFX_Internal_Bucket* internal = (struct GFX_Internal_Bucket*)bucket;

		gfx_list_free((GFXList*)internal->first);
		gfx_vector_clear(&internal->batches);

		free(bucket);
	}
}

/******************************************************/
GFXBatchUnit* gfx_bucket_insert(GFXBucket* bucket, void* data, GFXBatchState state)
{
	struct GFX_Internal_Bucket* internal = (struct GFX_Internal_Bucket*)bucket;
	struct GFX_Internal_Batch* unit;

	/* Check if any units exist */
	if(!internal->first)
	{
		/* No beginning yet, create it and also set last */
		unit = (struct GFX_Internal_Batch*)gfx_list_create(sizeof(struct GFX_Internal_Batch));
		internal->last = (GFXBatchUnit*)unit;
	}
	else
	{
		/* Insert before beginning */
		unit = (struct GFX_Internal_Batch*)gfx_list_insert_before(
			(GFXList*)internal->first,
			sizeof(struct GFX_Internal_Batch)
		);
	}
	if(!unit) return NULL;

	/* Set first */
	internal->first = (GFXBatchUnit*)unit;

	/* Init unit */
	unit->unit.data = data;
	unit->state = state;
	unit->bucket = bucket;

	/* Force a re-sort */
	internal->sort = 1;

	return (GFXBatchUnit*)unit;
}

/******************************************************/
GFXBatchState gfx_bucket_get_state(GFXBatchUnit* unit)
{
	return ((struct GFX_Internal_Batch*)unit)->state;
}

/******************************************************/
void gfx_bucket_set_state(GFXBatchUnit* unit, GFXBatchState state)
{
	struct GFX_Internal_Batch* internal = (struct GFX_Internal_Batch*)unit;

	/* Force a re-sort */
	internal->state = state;
	((struct GFX_Internal_Bucket*)internal->bucket)->sort = 1;
}

/******************************************************/
void gfx_bucket_erase(GFXBatchUnit* unit)
{
	struct GFX_Internal_Batch* internal = (struct GFX_Internal_Batch*)unit;
	struct GFX_Internal_Bucket* bucket = (struct GFX_Internal_Bucket*)internal->bucket;

	/* Erase it */
	GFXBatchUnit* new = (GFXBatchUnit*)gfx_list_erase((GFXList*)unit);

	/* Replace first or last if necessary */
	if(bucket->first == unit) bucket->first = new;
	if(bucket->last == unit) bucket->last = new;
}

/******************************************************/
void gfx_bucket_process(GFXBucket* bucket)
{
	struct GFX_Internal_Bucket* internal = (struct GFX_Internal_Bucket*)bucket;

	/* Nothing to process */
	if(!internal->first) return;

	/* Sort if needed */
	if(internal->sort)
	{
		gfx_vector_clear(&internal->batches);
		_gfx_bucket_radix_sort(internal->maxBit, &internal->first, &internal->last, internal);

		internal->sort = 0;
	}

	/* Process */
	GFXVectorIterator it;
	for(it = internal->batches.begin; it != internal->batches.end; it = gfx_vector_next(&internal->batches, it))
	{
		struct GFX_Batch_Pair* pair = it;
		GFXBatchState state = ((struct GFX_Internal_Batch*)pair->first)->state;

		bucket->process(state, pair->first, pair->last);
	}
}
