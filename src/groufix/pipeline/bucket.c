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
	GFXVector      batches; /* Batch shortcuts, stores GFXBatchUnit* */
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
static void _gfx_bucket_radix_sort(GFXBatchState bit, struct GFX_Internal_Batch* first, struct GFX_Internal_Batch* last, struct GFX_Internal_Bucket* bucket)
{
	/* Nothing to sort */
	if(first == last || !bit) return;

	/* Loop over all entries */
	struct GFX_Internal_Batch* mid = last; /* last to iterate to */
	struct GFX_Internal_Batch* cur;
	struct GFX_Internal_Batch* nex = NULL;

	for(cur = first; cur != mid; cur = nex)
	{
		/* Next entry (before cur changes position) */
		nex = (struct GFX_Internal_Batch*)((GFXList*)cur)->next;

		/* If 1, put in 1 bucket */
		if(cur->state & bit)
		{
			/* Reset first */
			if(cur == first) first = nex;

			/* Move to end */
			gfx_list_splice_after((GFXList*)cur, (GFXList*)last);
			last = cur;
		}
	}

	/* Sort both 0 and 1 buckets */
	if(mid->state)
	{
		if(mid != first) _gfx_bucket_radix_sort(bit >> 1, first, (struct GFX_Internal_Batch*)((GFXList*)mid)->previous, bucket);
		if(mid != last) _gfx_bucket_radix_sort(bit >> 1, mid, last, bucket);
	}
	else
	{
		if(mid != first) _gfx_bucket_radix_sort(bit >> 1, first, mid, bucket);
		if(mid != last) _gfx_bucket_radix_sort(bit >> 1, (struct GFX_Internal_Batch*)((GFXList*)mid)->next, last, bucket);
	}

	/* Set first and last */
	bucket->first = (GFXBatchUnit*)first;
	bucket->last = (GFXBatchUnit*)last;
}

/******************************************************/
GFXBucket* gfx_bucket_create(unsigned char bits, GFXBatchProcessFunc process)
{
	/* Allocate bucket */
	struct GFX_Internal_Bucket* bucket = calloc(1, sizeof(struct GFX_Internal_Bucket));
	if(!bucket) return NULL;

	gfx_vector_init(&bucket->batches, sizeof(GFXBatchUnit*));
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

	/* Sort if needed */
	if(internal->sort)
	{
		_gfx_bucket_radix_sort(internal->maxBit,
			(struct GFX_Internal_Batch*)internal->first,
			(struct GFX_Internal_Batch*)internal->last,
			internal
		);
		internal->sort = 0;
	}
}
