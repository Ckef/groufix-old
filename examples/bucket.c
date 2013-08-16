
#include <groufix.h>
#include <stdio.h>

#define NUM_BITS 32
#define OUT_VARS 9

int main()
{
	/* This performs the bucket's radix sort and prints, for testing purposes */

	GFXBucket* bucket = gfx_bucket_create(NUM_BITS, NULL);

	gfx_bucket_insert(bucket, NULL, 0x0000);
	gfx_bucket_insert(bucket, NULL, 0x0001);
	gfx_bucket_insert(bucket, NULL, 0x1000);
	gfx_bucket_insert(bucket, NULL, 0x0110);
	gfx_bucket_insert(bucket, NULL, 0x1001);
	gfx_bucket_insert(bucket, NULL, 0x0010);
	gfx_bucket_insert(bucket, NULL, 0x1111);
	gfx_bucket_insert(bucket, NULL, 0x0110);
	gfx_bucket_insert(bucket, NULL, 0x0001);

	GFXBatchUnit* unit = gfx_bucket_process(bucket);

	while(unit)
	{
		printf(" %04X", (unsigned int)gfx_bucket_get_state(unit));
		unit = (GFXBatchUnit*)unit->node.next;
	}
	puts("");

	return 0;
}
