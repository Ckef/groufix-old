
#include <groufix.h>
#include <stdio.h>

#define NUM_BITS 16

/* Print either all units or only batches */
#define PRINT_ALL 1

void process(GFXBatchState state, GFXBatchUnit* first, GFXBatchUnit* last)
{
#if PRINT_ALL == 1
	while((GFXList*)first != last->node.next)
	{
		printf("%04X ", (unsigned int)gfx_bucket_get_state(first));
		first = (GFXBatchUnit*)first->node.next;
	}
#else
	printf("%04X ", (unsigned int)state);
#endif
}

int main()
{
	/* This performs the bucket's radix sort and prints, for testing purposes */

	GFXBucket* bucket = gfx_bucket_create(NUM_BITS, process);

	gfx_bucket_insert(bucket, NULL, 0x0010);
	gfx_bucket_insert(bucket, NULL, 0x0001);
	gfx_bucket_insert(bucket, NULL, 0x1000);
	gfx_bucket_insert(bucket, NULL, 0x0110);
	gfx_bucket_insert(bucket, NULL, 0x1001);
	gfx_bucket_insert(bucket, NULL, 0x0000);
	gfx_bucket_insert(bucket, NULL, 0x1111);
	gfx_bucket_insert(bucket, NULL, 0x0110);
	gfx_bucket_insert(bucket, NULL, 0x0001);

	gfx_bucket_process(bucket);

	gfx_bucket_free(bucket);

	puts("");

	return 0;
}
