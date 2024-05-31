#include <kernel/buddy2.h>

int buddy_init_allocator(buddy_allocator_t* allocator, void* bookkeeping_block, uint32_t bookkeeping_size, void* memory_block, uint32_t size) {
	// makes no sense to have a size of zero
	if (size == 0)
		return -1;

	// size is not even with max order
	if (size % (1 << BUDDY_MAX_ORDER) != 0)
		return -1;

	// memory block is not aligned properly
	if ((uint32_t)memory_block % (1 << BUDDY_MIN_ORDER) != 0)
		return -1;

	uint32_t page_count = (size >> BUDDY_MIN_ORDER);

	uint32_t* bitmaps[BUDDY_MAX_ORDER - BUDDY_MIN_ORDER];
	bitmaps[0] = (uint32_t*)bookkeeping_block;
	for (int i = 1; i < (BUDDY_MAX_ORDER - BUDDY_MIN_ORDER - 1); i++) {
		bitmaps[i] = bitmaps[i - 1] + (page_count >> i);
	}
	bitmaps[BUDDY_MAX_ORDER - BUDDY_MIN_ORDER - 1] = bitmaps[BUDDY_MAX_ORDER - BUDDY_MIN_ORDER - 2] + (page_count >> (BUDDY_MAX_ORDER - BUDDY_MIN_ORDER - 1));


}
