
void initialize_buddy_allocator();
void* buddy_allocate(size_t size);
void buddy_allocator_free(void* data);
