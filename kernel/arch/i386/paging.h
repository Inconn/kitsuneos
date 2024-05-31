#ifndef ARCH_I386_PAGING_H
#define ARCH_I386_PAGING_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	unsigned int present		: 1;
	unsigned int read_write		: 1;
	unsigned int user		: 1;
	unsigned int writethrough	: 1;
	unsigned int cache_disable	: 1;
	unsigned int accessed		: 1;
	unsigned int _ignored2		: 1;
	unsigned int page_size		: 1;
	unsigned int _ignored		: 4;
	unsigned int address		: 20;
} __attribute__((packed)) page_directory_entry_t;

struct page_table_flags {
	unsigned int read_write		: 1;
	unsigned int user		: 1;
	unsigned int writethrough	: 1;
	unsigned int cache_disable	: 1;
	unsigned int accessed		: 1;
	unsigned int dirty		: 1;
	unsigned int PAT		: 1;
	unsigned int global		: 1;
} __attribute__((packed));

typedef struct {
	unsigned int read_write		: 1;
	unsigned int user		: 1;
	unsigned int writethrough	: 1;
	unsigned int cache_disable	: 1;
	unsigned int accessed		: 1;
	unsigned int dirty		: 1;
	unsigned int PAT		: 1;
	unsigned int global		: 1;
} __attribute__((packed)) page_table_flags_t;

typedef struct {
	unsigned int present		: 1;
	unsigned int read_write		: 1;
	unsigned int user		: 1;
	unsigned int writethrough	: 1;
	unsigned int cache_disable	: 1;
	unsigned int accessed		: 1;
	unsigned int dirty		: 1;
	unsigned int PAT		: 1;
	unsigned int global		: 1;
	unsigned int _ignored		: 3;
	unsigned int address		: 20;
} __attribute__((packed)) page_table_entry_t;

struct page_table_entry_metadata {
	page_table_entry_t* page_table; //physical address
	bool available_entries;
	uint16_t lowest_index;
	uint16_t page_directory_index;
};

void arch_paging_init();

void* arch_map_physical(void* physical_address, uint32_t size);

void* arch_map_page(uint16_t starting_dir_entry, void* physical_address);

void* arch_map_pages(uint16_t starting_dir_entry, void** physical_addresses, uint32_t page_count);

void _kalloc_set_initialized();

void* _kalloc_paging_init(void* metadata_physical_ptr, uint32_t metadata_size);

#endif
