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
static void _gfx_bucket_swap(struct GFX_Internal_Batch* A, struct GFX_Internal_Batch* B)
{
	/* Cannot swap pointers as neighbours would have invalid pointers */
	/* Assumes the same bucket */

	void* dataTemp = A->unit.data;
	A->unit.data = B->unit.data;
	B->unit.data = dataTemp;

	GFXBatchState stateTemp = A->state;
	A->state = B->state;
	B->state = stateTemp;
}

/******************************************************/
static void _gfx_bucket_radix_sort(GFXBatchState bit, struct GFX_Internal_Batch* first, struct GFX_Internal_Batch* last, struct GFX_Internal_Bucket* bucket)
{
	/* Nothing to sort */
	if(first == last || !bit)
	{
		/* Insert as batch */
		struct GFX_Batch_Pair pair = { (GFXBatchUnit*)first, (GFXBatchUnit*)last };
		gfx_vector_insert(&bucket->batches, &pair, bucket->batches.end);
	}
	else
	{
		/* Loop over all entries */
		struct GFX_Internal_Batch* mid = last; /* last to iterate to */
		struct GFX_Internal_Batch* cur = first;

		while(cur != mid)
		{
			/* If 1, put in 1 bucket */
			if(cur->state & bit)
			{
				/* Shift mid back */
				_gfx_bucket_swap(cur, mid);
				mid = (struct GFX_Internal_Batch*)((GFXList*)mid)->previous;
			}

			/* Shift cur forwards */
			else cur = (struct GFX_Internal_Batch*)((GFXList*)cur)->next;
		}

		/* Sort both 0 and 1 buckets */
		if(mid->state & bit)
		{
			if(mid != first) _gfx_bucket_radix_sort(
				bit >> 1, first, (struct GFX_Internal_Batch*)((GFXList*)mid)->previous, bucket);
			_gfx_bucket_radix_sort(
				bit >> 1, mid, last, bucket);
		}
		else
		{
			_gfx_bucket_radix_sort(
				bit >> 1, first, mid, bucket);
			if(mid != last) _gfx_bucket_radix_sort(
				bit >> 1, (struct GFX_Internal_Batch*)((GFXList*)mid)->next, last, bucket);
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
	int preprocess = internal->sort;

	if(preprocess)
	{
		gfx_vector_clear(&internal->batches);

		_gfx_bucket_radix_sort(internal->maxBit,
			(struct GFX_Internal_Batch*)internal->first,
			(struct GFX_Internal_Batch*)internal->last,
			internal
		);
		internal->sort = 0;

		preprocess = bucket->preprocess ? 1 : 0;
	}

	/* Process */
	GFXVectorIterator it;
	for(it = internal->batches.begin; it != internal->batches.end; it = gfx_vector_next(&internal->batches, it))
	{
		struct GFX_Batch_Pair* pair = it;
		GFXBatchState state = ((struct GFX_Internal_Batch*)pair->first)->state;

		if(preprocess) bucket->preprocess(state, pair->first, pair->last);
		bucket->process(state, pair->first, pair->last);
	}
}
