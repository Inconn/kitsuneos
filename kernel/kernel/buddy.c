// FIXME: the algorithm used here sucks ass i think

#include <kernel/buddy.h>

#include <stdbool.h>
#include <string.h>

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

	uint32_t page_count = size >> BUDDY_MIN_ORDER;

	uint32_t metadata_size = sizeof(buddy_block_metadata_t) * page_count;
	
	struct double_linked_list* freelists_block = (struct double_linked_list*)((uint32_t)bookkeeping_block + metadata_size + (sizeof(struct double_linked_list*) * (BUDDY_MAX_ORDER - BUDDY_MIN_ORDER)));

	struct double_linked_list* freelists[BUDDY_MAX_ORDER - BUDDY_MIN_ORDER];
	uint32_t size_summation = 0;
	for (uint32_t i = 0; i < (BUDDY_MAX_ORDER - BUDDY_MIN_ORDER); i++) {
		freelists[i] = (struct double_linked_list*)((uint32_t)freelists_block + size_summation);

		uint32_t max_list_count = (page_count >> i);
		size_summation += max_list_count * sizeof(struct double_linked_list);
	}

	// not enough memory allocated to keep track of our data
	if ((metadata_size + size_summation) > bookkeeping_size)
		return -1;

	allocator->physical_page_metadata = bookkeeping_block;
	allocator->memory_block = memory_block;
	allocator->freelists = bookkeeping_block + metadata_size;
	allocator->size = size;

	memcpy(allocator->freelists, freelists, (sizeof(struct double_linked_list*) * (BUDDY_MAX_ORDER - BUDDY_MIN_ORDER + 1)));

	for (uint32_t i = 0; i < (BUDDY_MAX_ORDER - BUDDY_MIN_ORDER); i++) {
		allocator->freelists[i][0].prev = NULL;
		allocator->freelists[i][0].next = NULL;
	}

	for (uint32_t i = 0; i < page_count; i++) {
		allocator->physical_page_metadata[i].is_free = true;

		for (uint32_t j = 0; j < (BUDDY_MAX_ORDER - BUDDY_MIN_ORDER); j++) {
			if ((i % (j + 1)) == 0) {
				allocator->physical_page_metadata[i].order = (BUDDY_MIN_ORDER + j);
				if (j == (BUDDY_MAX_ORDER - BUDDY_MIN_ORDER)) {
					uint32_t index = i / (j + 1);

					struct double_linked_list* current_list = &allocator->freelists[BUDDY_MAX_ORDER - BUDDY_MIN_ORDER - 1][index];

					current_list->prev = (struct double_linked_list*)0xDEADBEEF;
					current_list->next = NULL;
					current_list->data = allocator->memory_block + ((1 << BUDDY_MAX_ORDER) * index);
					if (index != 0) {
						struct double_linked_list* last_list = &allocator->freelists[BUDDY_MAX_ORDER - BUDDY_MIN_ORDER - 1][index - 1];

						current_list->prev = last_list;
						last_list->next = current_list;
					}
				}
			}
		}
	}
	/*for (uint32_t i = 0; i < (allocator->page_count >> (BUDDY_MAX_ORDER - BUDDY_MIN_ORDER - 1)); i++) {
	}*/

	return 0;
}

void* buddy_allocate(buddy_allocator_t* allocator, uint32_t size) {
	// doesn't make sense to ask for an allocation of size 0
	// also means we can use __builtin_clz without undefined behavior
	if (size == 0)
		return NULL;

	uint32_t required_order = 30 - __builtin_clz(size);

	// too big to give out as one allocation
	if (required_order > BUDDY_MAX_ORDER)
		return NULL;

	// if they're requesting memory too small for us to give out, just give the smallest we can.
	if (required_order < BUDDY_MIN_ORDER)
		required_order = BUDDY_MIN_ORDER;

	// freelist item that corresponds to the free block we're going to use for the allocation
	struct double_linked_list* list = NULL;

	uint32_t order = required_order - BUDDY_MIN_ORDER;
	uint32_t index = 0;

	// find out if there's any memory available that can fulfill this allocation request.
	for (; order <= (BUDDY_MAX_ORDER - BUDDY_MIN_ORDER); order++) {
		struct double_linked_list* list_to_check = &allocator->freelists[order][0];
		if (list_to_check->prev != NULL || list_to_check->next != NULL)
			break;
	}

	// if this happens, there's no more memory available that can fulfill this allocation request.
	if (order > (BUDDY_MAX_ORDER - BUDDY_MIN_ORDER))
		return NULL;

	// loop over the freelist until we get to the last one.
	for (; list == NULL; index++) {
		struct double_linked_list* list_to_check = &allocator->freelists[order][index];
	
		if (list_to_check->next == NULL)
			list = list_to_check;
	}

	// shouldn't be possible for this to happen, but we'll check anyways
	if (list == NULL)
		return NULL;
	
	// i set the prev field to 0xDEADBEEF if the block is the first list in the linked list and it exists
	// but obviously that's not actually a pointer to a list, so we have to filter that out
	if (list->prev != (struct double_linked_list*)0xDEADBEEF)
		list->prev->next = list->next;

	list->prev = NULL;

	while (order < required_order) {
		order--;
		index <<= 2;

		struct double_linked_list* first_list = &allocator->freelists[order][index];
		first_list->data = allocator->memory_block + ((1 << order) * index);
		first_list->prev = NULL;
		first_list->next = NULL;
		if (index == 0) {
			first_list->prev = (struct double_linked_list*)0xDEADBEEF;
		} else {
			allocator->freelists[order][0].next = first_list;
		}

		list = &allocator->freelists[order][index + 1];

		index++;
	}

	list->data = allocator->memory_block + ((1 << order) * index);
	return list->data;
}

int buddy_free(buddy_allocator_t* allocator, void* memory_to_free) {
	if (memory_to_free < allocator->memory_block || memory_to_free > (allocator->memory_block + allocator->size))
		// this isn't memory that we manage, we can't do anything with this.
		return -1;

	if ((uint32_t)memory_to_free % (1 << BUDDY_MIN_ORDER) != 0)
		// memory isn't aligned properly so we can't free it properly
		return -1;

	uint32_t page_index = ((allocator->memory_block - memory_to_free) >> BUDDY_MIN_ORDER);
	if (allocator->physical_page_metadata[page_index].is_free == true)
		// this memory has already been freed.
		return -1;

	uint32_t order = allocator->physical_page_metadata[page_index].order;
	uint32_t index = (page_index >> (order - BUDDY_MIN_ORDER));

	struct double_linked_list* order_freelist = allocator->freelists[order];
	struct double_linked_list* memory_llist = &order_freelist[index];

	memory_llist->data = memory_to_free;

	if (memory_llist != order_freelist) {
		if (order_freelist[0].next != NULL) {
			if (order_freelist[0].next > memory_llist) {
				memory_llist->prev = NULL;
				if (order_freelist[0].prev == (struct double_linked_list*)0xDEADBEEF)
					memory_llist->prev = &order_freelist[0];
				memory_llist->next = order_freelist[0].next;

				order_freelist[0].next->prev = memory_llist;
				order_freelist[0].next = memory_llist;
			} else {
				struct double_linked_list* llist = order_freelist[0].next;
				bool all_lower = false;

				while (llist < memory_llist && !all_lower) {
					llist = llist->next;
					all_lower = (llist->next == NULL);
				}

				if (all_lower) {
					llist->next = memory_llist;
					memory_llist->next = NULL;
				} else {
					memory_llist->prev = llist->prev;
					memory_llist->next = llist;

					llist->prev->next = memory_llist;
					llist->prev = memory_llist;
				}
			}
		} else {
			order_freelist[0].next = memory_llist;

		}
	} else {
		memory_llist->prev = (struct double_linked_list*)0xDEADBEEF;
	}

	while (order < BUDDY_MAX_ORDER) {
		page_index -= (page_index % (order << 1));
		if (!(allocator->physical_page_metadata[page_index].is_free && allocator->physical_page_metadata[page_index + 1].is_free && (allocator->physical_page_metadata[page_index].order == order) && (allocator->physical_page_metadata[page_index + 1].order == order)))
			break;

		index = (page_index >> (order - BUDDY_MIN_ORDER));
		struct double_linked_list* first_list = &allocator->freelists[order][index];
		struct double_linked_list* second_list = &allocator->freelists[order][index + 1];

		if (first_list == allocator->freelists[order]) {
			second_list->next->prev = NULL;
			first_list->next = second_list->next;
		}
		else {
			if (first_list->prev != NULL)
				first_list->prev->next = second_list->next;
			if (second_list->next != NULL)
				second_list->next->prev = first_list->prev;
		}

		struct double_linked_list* list = &allocator->freelists[order][index];

		if (list == allocator->freelists[order]) {
			list->prev = (struct double_linked_list*)0xDEADBEEF;
		} else if (allocator->freelists[order]->next == NULL) {
			allocator->freelists[order]->next = list;
			if (allocator->freelists[order]->prev == (struct double_linked_list*)0xDEADBEEF)
				list->prev = allocator->freelists[order];
		} else if (allocator->freelists[order]->next > list) {
			list->next = allocator->freelists[order]->next;
			allocator->freelists[order]->next = list;
			if (allocator->freelists[order]->prev == (struct double_linked_list*)0xDEADBEEF)
				list->prev = allocator->freelists[order];
		} else {
			struct double_linked_list* llist = order_freelist[0].next;
			bool all_lower = false;

			while (llist < list && !all_lower) {
				llist = llist->next;
				all_lower = (llist->next == NULL);
			}

			if (all_lower) {
				llist->next = list;
				list->next = NULL;
			} else {
				list->prev = llist->prev;
				list->next = llist;

				llist->prev->next = list;
				llist->prev = list;
			}
		}

		order--;
	}

	return 0;
}
