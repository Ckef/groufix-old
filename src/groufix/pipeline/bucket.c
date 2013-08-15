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
/* Internal batch unit */
struct GFX_Internal_Batch
{
	/* Super class */
	GFXBatchUnit unit;

	/* Hidden data */
	GFXBatchState  state;
	GFXBucket*     bucket;
};

/* Internal bucket */
struct GFX_Internal_Bucket
{
	/* Super class */
	GFXBucket bucket;

	/* Hidden data */
	unsigned char  bits;
	unsigned char  sort;

	GFXBatchUnit*  begin;   /* Begin of units */
	GFXBatchUnit*  end;     /* End of units */
	GFXVector      batches; /* Batch shortcuts, stores GFXBatchUnit* */
};

/******************************************************/
GFXBucket* gfx_bucket_create(unsigned char bits, GFXBatchProcessFunc process)
{
	/* Allocate bucket */
	struct GFX_Internal_Bucket* bucket = calloc(1, sizeof(struct GFX_Internal_Bucket));
	if(!bucket) return NULL;

	/* Try to allocate the batch vector */
	gfx_vector_init(&bucket->batches, sizeof(GFXBatchUnit*));
	bucket->bucket.process = process;
	bucket->bits = bits;

	return (GFXBucket*)bucket;
}

/******************************************************/
void gfx_bucket_free(GFXBucket* bucket)
{
	if(bucket)
	{
		struct GFX_Internal_Bucket* internal = (struct GFX_Internal_Bucket*)bucket;

		gfx_list_free((GFXList*)internal->begin);
		gfx_vector_clear(&internal->batches);

		free(bucket);
	}
}
