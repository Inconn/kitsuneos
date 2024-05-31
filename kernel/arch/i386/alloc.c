#include <kernel/buddy.h>
#include <kernel/alloc.h>

#include <kernel/dumb_alloc.h>

#include <stddef.h>

#include "paging.h"

extern uint32_t kernel_end;

static buddy_allocator_t allocator;

int init_kalloc() {
	dumb_alloc_init();

	//uint32_t metadata_size = ((uint32_t)&kernel_end % 4096) - 2048;
	uint32_t metadata_size = 32768 + 390144;
	uint32_t data_size = 0x80000 * 128;

	void* metadata_phys = dumb_allocate_physical(metadata_size, 1);
	void* metadata = _kalloc_paging_init(metadata_phys, metadata_size);
	if (metadata == NULL)
		return -2;

	void* data_phys = dumb_allocate_physical(data_size, 4096);

	int result = buddy_init_allocator(&allocator, metadata, metadata_size, data_phys, data_size);

	if (result == 0)
		_kalloc_set_initialized();

	return result;
}

void* _kalloc_physical(uint32_t size) {
	return buddy_allocate(&allocator, size);
}

void* kalloc(uint32_t size) {
	void* physical_address = buddy_allocate(&allocator, size);

	
}

void kfree(void* mem) {
	
}
