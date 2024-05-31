#ifndef KERNEL_ALLOC_H
#define KERNEL_ALLOC_H

#include <stdint.h>

int init_kalloc();

void* kalloc(uint32_t size);

void kfree(void* mem);

#endif
