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
struct GFX_Unit_Pair
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
	unsigned char  sort;

	GFXBatchUnit*  first;   /* Begin of units */
	GFXBatchUnit*  last;    /* End of units */
	GFXVector      batches; /* Batch shortcuts, stores GFX_Unit_Pair */
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
/* Reconstructs the batch vector before erasing a unit */
static void _gfx_bucket_fix_batch(struct GFX_Internal_Bucket* bucket, size_t start, size_t end, struct GFX_Internal_Unit* erase)
{
	/* Nothing found */
	if(start < end)
	{
		/* Binary search for the state */
		size_t mid = start + ((end - start) >> 1);
		struct GFX_Unit_Pair* pair = gfx_vector_at(&bucket->batches, mid);

		GFXBatchState compare = ((struct GFX_Internal_Unit*)pair->first)->state;

		if(erase->state < compare)
			_gfx_bucket_fix_batch(bucket, start, mid, erase);

		else if(erase->state > compare)
			_gfx_bucket_fix_batch(bucket, mid, end, erase);

		/* Erase or repair batch */
		else if(pair->first == pair->last)
			gfx_vector_erase(&bucket->batches, (GFXVectorIterator)pair);

		else if((GFXBatchUnit*)erase == pair->first)
			pair->first = (GFXBatchUnit*)pair->first->next;

		else if((GFXBatchUnit*)erase == pair->last)
			pair->last = (GFXBatchUnit*)pair->last->previous;
	}
}

/******************************************************/
/* Sorts the linked list and constructs the batch vector */
static void _gfx_bucket_radix_sort(GFXBatchState bit, GFXBatchUnit** first, GFXBatchUnit** last, struct GFX_Internal_Bucket* bucket)
{
	/* Nothing to sort */
	if(*first == *last || !bit)
	{
		/* Insert as batch */
		struct GFX_Unit_Pair pair = { *first, *last };
		gfx_vector_insert(&bucket->batches, &pair, bucket->batches.end);

		/* Preprocess */
		if(bucket->bucket.preprocess) bucket->bucket.preprocess(
			((struct GFX_Internal_Unit*)*first)->state, pair.first, pair.last
		);
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
GFXBucket* gfx_bucket_create(unsigned char bits, GFXBatchProcessFunc process, GFXBatchProcessFunc preprocess)
{
	/* Allocate bucket */
	struct GFX_Internal_Bucket* bucket = calloc(1, sizeof(struct GFX_Internal_Bucket));
	if(!bucket) return NULL;

	gfx_vector_init(&bucket->batches, sizeof(struct GFX_Unit_Pair));

	/* Init bucket */
	bucket->bit = (bits ? bits : sizeof(GFXBatchState) * 8) - 1;
	bucket->bucket.process = process;
	bucket->bucket.preprocess = preprocess;

	return (GFXBucket*)bucket;
}

/******************************************************/
void gfx_bucket_free(GFXBucket* bucket)
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
GFXBatchUnit* gfx_bucket_insert(GFXBucket* bucket, GFXBatchState state, size_t dataSize)
{
	struct GFX_Internal_Bucket* internal = (struct GFX_Internal_Bucket*)bucket;
	struct GFX_Internal_Unit* unit;

	size_t size = sizeof(struct GFX_Internal_Unit) + dataSize;

	/* Check if any units exist */
	if(!internal->first)
	{
		/* No beginning yet, create it and also set last */
		unit = (struct GFX_Internal_Unit*)gfx_list_create(size);
		internal->last = (GFXBatchUnit*)unit;
	}
	else
	{
		/* Insert before beginning */
		unit = (struct GFX_Internal_Unit*)gfx_list_insert_before(internal->first, size);
	}
	if(!unit) return NULL;

	/* Set first */
	internal->first = (GFXBatchUnit*)unit;

	/* Init unit */
	unit->state = state;
	unit->bucket = bucket;

	/* Force a re-sort */
	internal->sort = 1;

	return (GFXBatchUnit*)unit;
}

/******************************************************/
GFXBatchState gfx_bucket_get_state(GFXBatchUnit* unit)
{
	return ((struct GFX_Internal_Unit*)unit)->state;
}

/******************************************************/
void gfx_bucket_set_state(GFXBatchUnit* unit, GFXBatchState state)
{
	struct GFX_Internal_Unit* internal = (struct GFX_Internal_Unit*)unit;

	/* Force a re-sort */
	internal->state = state;
	((struct GFX_Internal_Bucket*)internal->bucket)->sort = 1;
}

/******************************************************/
void* gfx_bucket_get_data(GFXBatchUnit* unit)
{
	return (void*)(((struct GFX_Internal_Unit*)unit) + 1);
}

/******************************************************/
void gfx_bucket_erase(GFXBatchUnit* unit)
{
	struct GFX_Internal_Unit* internal = (struct GFX_Internal_Unit*)unit;
	struct GFX_Internal_Bucket* bucket = (struct GFX_Internal_Bucket*)internal->bucket;

	/* First fix batches */
	_gfx_bucket_fix_batch(bucket, 0, gfx_vector_get_size(&bucket->batches), internal);

	/* Erase it */
	GFXBatchUnit* new = gfx_list_erase(unit);

	/* Replace first or last if necessary */
	if(bucket->first == unit) bucket->first = new;
	if(bucket->last == unit) bucket->last = new;
}

/******************************************************/
void gfx_bucket_preprocess(GFXBucket* bucket)
{
	struct GFX_Internal_Bucket* internal = (struct GFX_Internal_Bucket*)bucket;

	/* Check if sort is needed */
	if(internal->first && internal->sort)
	{
		gfx_vector_clear(&internal->batches);
		_gfx_bucket_radix_sort(1 << internal->bit, &internal->first, &internal->last, internal);

		internal->sort = 0;
	}
}

/******************************************************/
void gfx_bucket_process(GFXBucket* bucket)
{
	struct GFX_Internal_Bucket* internal = (struct GFX_Internal_Bucket*)bucket;

	/* Process */
	GFXVectorIterator it;
	for(it = internal->batches.begin; it != internal->batches.end; it = gfx_vector_next(&internal->batches, it))
	{
		struct GFX_Unit_Pair* pair = it;

		bucket->process(
			((struct GFX_Internal_Unit*)pair->first)->state,
			pair->first, pair->last
		);
	}
}
