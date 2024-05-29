#ifndef ARCH_I386_PAGING_H
#define ARCH_I386_PAGING_H

#include <stdint.h>

typedef struct {
	unsigned int address		: 20;
	unsigned int _ignored		: 4;
	unsigned int page_size		: 1;
	unsigned int _ignored2		: 1;
	unsigned int accessed		: 1;
	unsigned int cache_disable	: 1;
	unsigned int writethrough	: 1;
	unsigned int user		: 1;
	unsigned int read_write		: 1;
	unsigned int present		: 1;
} __attribute__((packed)) page_directory_entry_t;

typedef struct {
	unsigned int address		: 20;
	unsigned int _ignored		: 3;
	unsigned int global		: 1;
	unsigned int PAT		: 1;
	unsigned int dirty		: 1;
	unsigned int accessed		: 1;
	unsigned int cache_disable	: 1;
	unsigned int writethrough	: 1;
	unsigned int user		: 1;
	unsigned int read_write		: 1;
	unsigned int present		: 1;
} __attribute__((packed)) page_table_entry_t;

void arch_paging_init();

#endif
