#pragma once

#include "allocator.h"

/*
	Simplest and fastest type of allocator.
	Pointers to the start of the allocator, to the first free address and the
	total size of the allocator are maintained.

	Allocations:
	New allocations simply move the pointer to the first free address forward.

	Deallocations:
	Individual deallocations cannot be made in linear allocators. Used clear()
	instead to completely clear the memory used by the allocator.
*/
class LinearAllocator : Allocator
{
private:
	LinearAllocator(const LinearAllocator&);	// Prevent copies; might cause errors
	LinearAllocator& operator=(const LinearAllocator&);

	void* current_pos;

public:
	LinearAllocator(size_t size, void* start);
	~LinearAllocator();

	void* allocate(size_t size, Lemur::u8 alignment) override;

	void deallocate(void* p) override;

	void clear();
};