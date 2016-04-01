#pragma once
#include <cstdint>

template <uint64_t N>
struct ComponentTypeid
{
private:
	using ID = N;
};
