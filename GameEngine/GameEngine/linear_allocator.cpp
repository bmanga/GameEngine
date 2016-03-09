#include "linear_allocator.h"

LinearAllocator::LinearAllocator(size_t size, void* start) :
	Allocator(size, start), current_pos(start)
{
	ASSERT_CLERROR(size > 0, CLASS_NAME, "Cannot initialize with size <= 0!");
}

LinearAllocator::~LinearAllocator()
{
	current_pos = nullptr;
}

void* LinearAllocator::allocate(size_t size, Lemur::u8 alignment)
{
	ASSERT_CLERROR(size != 0, CLASS_NAME, "Cannot allocate with a size of 0!");

	Lemur::u8 adjustment = pointer_math::alignForwardAdjustment(current_pos, alignment);

	if (used_memory + adjustment + size > this->size)
		return nullptr;

	uintptr_t aligned_address = (uintptr_t)current_pos + adjustment;

	current_pos = (void*)(aligned_address + size);

	used_memory += size + adjustment;
	num_allocations++;

	return (void*)aligned_address;
}

void LinearAllocator::deallocate(void* p)
{
	ASSERT_CLERROR(false, CLASS_NAME, "Use clear() instead!");
}

void LinearAllocator::clear()
{
	num_allocations = 0;
	used_memory = 0;

	current_pos = start;
}