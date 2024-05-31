#ifndef KERNEL_DUMB_ALLOC_H
#define KERNEL_DUMB_ALLOC_H

#include <stdint.h>

void dumb_alloc_init();

void* dumb_allocate_physical(uint32_t size, uint32_t alignment);

#endif
