#include "paging.h"

#include <stdbool.h>
#include <string.h>

#include <kernel/dumb_alloc.h>
#include <kernel/alloc.h>
#include "alloc.h"

extern uint32_t _kernel_page_directory;
extern uint32_t _boot_page_table1;

page_directory_entry_t* kernel_page_directory;
page_table_entry_t* base_kernel_page_table;

static uint32_t page_tables_size;
static struct page_table_entry_metadata* page_tables;

static bool kalloc_init = false;

void _kalloc_set_initialized() {
	kalloc_init = true;
}

void _paging_invalidate_tlb() {
	asm volatile(
			"pushl %ecx;"
			"movl %cr3, %ecx;"
			"movl %ecx, %cr3;"
			"popl %ecx;"
		    );
}

page_table_entry_t* _get_virtual_page_table_ptr(uint16_t directory_table_index) {

}

page_table_entry_t* _map_page_table_ptr(void* page_table_physical) {
	page_table_entry_t* high_page_table = &base_kernel_page_table[1];

	//void* page_table_physical = (void*)((uint32_t)kernel_page_directory[directory_index].address << 12);

	high_page_table->address = ((uint32_t)page_table_physical >> 12);

	// FIXME: we don't have to invalidate the whole tlb cache
	_paging_invalidate_tlb();

	page_table_entry_t* page_table = (page_table_entry_t*)0xC0001000;

	return page_table;
}

int _map_physical_to_virtual(void* physical_ptr, uint16_t directory_index, uint16_t page_index) {
	if (((uint32_t)physical_ptr % 4096) != 0)
		return -1; // ptr is not aligned to a 4KiB page frame

	page_table_entry_t* page_table = NULL;
	void* page_table_physical = NULL;

	if (kernel_page_directory[directory_index].present == 1) {
		page_table_physical = (void*)((uint32_t)kernel_page_directory[directory_index].address << 12);

		page_table = _map_page_table_ptr(page_table_physical);
		if (page_table[page_index].present == 1)
			return -1; // there's already memory mapped here
	} else {
		if (kalloc_init) {
			page_table_physical = _kalloc_physical(4096);
		} else {
			page_table_physical = dumb_allocate_physical(4096, 4096);
		}
		page_table = _map_page_table_ptr(page_table_physical);

	}
	
	memset(page_table, 0, sizeof(page_table_entry_t));

	page_table->address = ((uint32_t)physical_ptr >> 12);
	page_table->present = 1;
	page_table->read_write = 1;

	kernel_page_directory[directory_index].address = ((uint32_t)page_table_physical >> 12);
	kernel_page_directory[directory_index].present = 1;
	kernel_page_directory[directory_index].read_write = 1;

	_paging_invalidate_tlb();
}

void arch_paging_init() {
	kernel_page_directory =  (page_directory_entry_t*)&_kernel_page_directory;
	base_kernel_page_table = (page_table_entry_t*)&_boot_page_table1;
}

void* arch_map_page(uint16_t starting_dir_entry, void* physical_address) {
/*	for (uint32_t i = 0; i < page_tables_size; i++) {
		page_table_entry_t* page_table = _map_page_table_ptr(i);

		bool found = false;
		uint32_t j = page_tables[i].lowest_index;
		for (; j < 1024; i++) {
			if (page_table[j].present == 0) {
				found = true;
				break;
			}
		}

		if (found) {
			page_table_entry_t* page_table_entry = &page_table[j];
			page_table_entry->address = (uint32_t)physical_address;
			page_table_entry->present = 1;
			page_table_entry->read_write = 1;

			// FIXME: we don't have to invalidate the whole tlb cache
			_paging_invalidate_tlb();

			void* linear_address = (page_table_entry_t*)((page_tables[j].page_directory_index << 22) | (j << 12));

			return linear_address;
		}
	}*/
}

int find_consecutive_pages(uint32_t required_page_count, uint16_t directory_index_start, uint16_t page_index_start, uint16_t* directory_index_out, uint16_t* page_index_out) {
	uint32_t consecutive_pages_found = 0;
	uint16_t directory_index = directory_index_start;
	uint16_t page_index = page_index_start;
	
	page_table_entry_t* page_table = NULL;
	while (consecutive_pages_found < required_page_count) {
		if (kernel_page_directory[directory_index].present == 0) {
			if (consecutive_pages_found == 0) {
				*directory_index_out = directory_index;
				*page_index_out = page_index;
			}
			consecutive_pages_found += 1024;
			if ((directory_index + 1) == 1024)
				break;
			page_index = 0;
			directory_index++;
		} else {
			void* page_table_physical = (void*)(kernel_page_directory[directory_index].address << 12);
			page_table_entry_t* high_page_table = &base_kernel_page_table[1];

			high_page_table->address = ((uint32_t)page_table_physical >> 12);
			high_page_table->present = 1;
			high_page_table->read_write = 1;

			_paging_invalidate_tlb();
		
			page_table = (page_table_entry_t*)((768 << 22) | (1 << 12));
			if (page_table[page_index].present == 0) {
				if (consecutive_pages_found == 0) {
					*directory_index_out = directory_index;
					*page_index_out = page_index;
				}
				consecutive_pages_found++;
				if ((page_index + 1) == 1024) {
					if ((directory_index + 1) == 1024)
						break;
					page_index = 0;
					directory_index++;
				} else {
					page_index++;
				}
			}
		}
	}
	if (consecutive_pages_found < required_page_count)
		return -1;

	return 0;
}

void* arch_map_physical(void* physical_ptr, uint32_t required_page_count) {
	if (((uint32_t)physical_ptr % 4096) != 0)
		return NULL; // physical pointer is not aligned to a 4 KiB page
	
	void* current_ptr = physical_ptr;
	uint16_t directory_index;
	uint16_t page_index;

	if (find_consecutive_pages(required_page_count, 768, 2, &directory_index, &page_index) != 0)
		return NULL;

	for (uint32_t page_count = 0; page_count < required_page_count; page_count++) {
		_map_physical_to_virtual(current_ptr, directory_index, page_index);
		if ((page_index + 1) >= 1024) {
			directory_index++;
			page_index = 0;
		} else {
			page_index++;
		}
	}
}

void* arch_map_pages(uint16_t starting_dir_entry, void** physical_addresses, uint32_t page_count) {
	for (uint32_t i = 0; i < page_count; i++) {
		
	}
}


void _unmap_page_entry(uint16_t directory_index, uint16_t page_index) {
	if (kernel_page_directory[directory_index].present == 0)
		return;

	void* page_table_physical = (void*)(kernel_page_directory[directory_index].address << 12);
	page_table_entry_t* high_page_table = &base_kernel_page_table[1];
			
	high_page_table->address = ((uint32_t)page_table_physical >> 12);

	_paging_invalidate_tlb();
		
	page_table_entry_t* page_table = (page_table_entry_t*)0xC0001000;

	if (page_table[page_index].present == 0)
		return;

	memset(&page_table[page_index], 0, sizeof(page_table_entry_t));
}

void* _kalloc_paging_init(void* metadata_physical_ptr, uint32_t metadata_size) {
	uint32_t offset = ((uint32_t)metadata_physical_ptr % 4096);
	void* aligned_ptr = (void*)((uint32_t)metadata_physical_ptr - ((uint32_t)metadata_physical_ptr % 4096));
	uint32_t required_page_entry_count = ((uint32_t)metadata_size + 4095) / 4096;

	uint32_t page_index = 0;
	uint32_t index = 0;

	bool was_present = false;

	uint32_t consecutive_page_entries_found = 0;
	uint16_t page_directory_index = 768;
	uint16_t page_table_index = 2;

	uint16_t found_directory_index = 0;
	uint16_t found_page_index = 0;
	if (find_consecutive_pages(required_page_entry_count, 768, 2, &found_directory_index, &found_page_index) != 0)
		return NULL;

	uint16_t current_directory_index = found_directory_index;
	uint16_t current_page_index = found_page_index;
	page_table_entry_t* current_page_table;
	void* current_physical = aligned_ptr;
	uint32_t pages_mapped = 0;
	while (pages_mapped < required_page_entry_count) {
		void* page_table_physical;
		if (kernel_page_directory[page_directory_index].present == 0) {
			page_table_physical = dumb_allocate_physical(4096, 4096);
			page_table_entry_t* high_page_table = &base_kernel_page_table[1];

			high_page_table->address = ((uint32_t)page_table_physical >> 12);

			_paging_invalidate_tlb();
		
			current_page_table = (page_table_entry_t*)0xC0001000;

			memset(current_page_table, 0, 4096);

			kernel_page_directory[current_directory_index].address = ((uint32_t)page_table_physical >> 12);
			kernel_page_directory[current_directory_index].present = 1;
			kernel_page_directory[current_directory_index].read_write = 1;

			_paging_invalidate_tlb();
		} else if (current_page_table == NULL) {
			page_table_physical = (void*)(kernel_page_directory[page_directory_index].address << 12);
			page_table_entry_t* high_page_table = &base_kernel_page_table[1];
			
			high_page_table->address = ((uint32_t)page_table_physical >> 12);

			_paging_invalidate_tlb();
		
			current_page_table = (page_table_entry_t*)0xC0001000;
		}

		current_page_table[current_page_index].address = ((uint32_t)current_physical >> 12);
		current_page_table[current_page_index].present = 1;
		current_page_table[current_page_index].read_write = 1;

		if (current_page_index >= 1024) {
			current_page_table = NULL;
			current_page_index = 0;
			current_directory_index++;
		}

		current_physical = (void*)((uint32_t)current_physical + 4096);
		pages_mapped++;
		current_page_index++;
	}

	_paging_invalidate_tlb();

	return (void*)((found_directory_index << 22) | (found_page_index << 12) | (offset));
}
