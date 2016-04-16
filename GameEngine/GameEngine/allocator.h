#pragma once

#include "types.h"
#include "Lemur.h"


class Allocator
{
protected:
	void* start;
	size_t size;

	size_t used_memory;
	size_t num_allocations;

public:
	Allocator(size_t size, void* start)
	{
		this->start = start;
		this->size = size;

		used_memory = 0;
		num_allocations = 0;
	}

	virtual ~Allocator()
	{
		ASSERT_CLERROR(num_allocations == 0 && used_memory == 0, CLASS_NAME, "Allocation count/used memory > 0 on object destruction!");

		start = nullptr;
		size = 0;
	}

	virtual void* allocate(size_t size, Lemur::u8 alignment = 4) = 0;

	virtual void deallocate(void* p) = 0;

	void* getStart() const
	{
		return start;
	}

	size_t getSize() const
	{
		return size;
	}

	size_t getUsedMemory() const
	{
		return used_memory;
	}

	size_t getNumAllocations() const
	{
		return num_allocations;
	}
};

namespace allocator
{
	// bruno removed underscore
	template<class T> T* allocateNew(Allocator& allocator)
	{
		return new (allocator.allocate(sizeof(T), alignof(T))) T;
	}

	template<class T> T* allocateNew(Allocator& allocator, const T& t)
	{
		return new (allocator.allocate(sizeof(T), alignof(T))) T(t);
	}

	template<class T> void deallocateDelete(Allocator& allocator, T& object)
	{
		object.~T();
		allocator.deallocate(&object);
	}

	template<class T> T* allocateArray(Allocator& allocator, size_t length)
	{
		ASSERT_CLERROR(length != 0, CLASS_NAME, "Cannot allocate an array of length 0!");

		Lemur::u8 header_size = sizeof(size_t) / sizeof(T);

		if (sizeof(size_t) % sizeof(T) > 0)
			header_size += 1;

		// Allocate extra space to store array length in the bytes before array
		T* p = ((T*)allocator.allocate(sizeof(T) * (length + header_size), __alignof(T))) + header_size;

		*(((size_t)p) - 1) = length;

		for (size_t i = 0; i < length; i++)
			new (&p[i]) T;

		return p;
	}

	template<class T> void deallocateArray(Allocator& allocator, T* array)
	{
		ASSERT_CLERROR(array != nullptr, CLASS_NAME, "Null pointer to array requiring deallocation!");

		size_t length = *(((size_t*)array) - 1);

		for (size_t i = 0; i < length; i++)
			array[i].~T();

		// Calculate how much extra memory was allocated to store the length
		// before the array.
		Lemur::u8 header_size = sizeof(size_t) / sizeof(T);

		if (sizeof(size_t) % sizeof(T) > 0)
			header_size += 1;

		allocator.deallocate(array - header_size);
	}
}

namespace pointer_math
{
	/*
	Aligns the memory address by the alignment bytes:

	Masks off the log2(n) significant bits from the address to get the first
	n-byte aligned address previously.
	To get the next aligned address, alignment-1 is added to the address, which
	is then itself masked.
	*/
	inline void* alignForward(void* address, Lemur::u8 alignment)
	{
		return (void*)((reinterpret_cast<uintptr_t>(address) + static_cast<uintptr_t>(alignment - 1)) & static_cast<uintptr_t>(~(alignment - 1)));
	}

	/*
	Calculates how many bytes the address needs to be aligned by.
	*/
	inline Lemur::u8 alignForwardAdjustment(const void* address, Lemur::u8 alignment)
	{
		Lemur::u8 adjustment = alignment - (reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(alignment - 1));

		if (adjustment == alignment)
			return 0;	// Already aligned

		return adjustment;
	}

	/*
	Calculates how many bytes the address needs to be aligned by when included
	with a header.

	Some allocators need to store a header before each allocation so they can
	used the adjustment space to reduce the memory overhead caused by the
	headers.
	*/
	inline Lemur::u8 alignForwardAdjustmentWithHeader(const void* address, Lemur::u8 alignment, Lemur::u8 header_size)
	{
		Lemur::u8 adjustment = alignForwardAdjustment(address, alignment);

		Lemur::u8 needed_space = header_size;

		if (adjustment < needed_space)
		{
			needed_space -= adjustment;

			// Increase adjustment to fit header
			adjustment += alignment * (needed_space / alignment);

			if (needed_space % alignment > 0)
				adjustment += alignment;
		}

		return adjustment;
	}
}