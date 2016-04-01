#pragma once

#include "ecs\component_type.h"

#include <unordered_map>
#include <math.h>

typedef unsigned int BitMask;

struct Aspect
{
	unsigned int component_type_bits;

	std::unordered_map<ecs::ComponentType, BitMask> id_to_mask_map;
	const unsigned int BASE = 2;
	unsigned int current_power = 0;

	template<typename CT, typename... CTs>
	void and(CT first, CTs... rest)
	{
		BitMask mask = BASE * current_power;

		id_to_mask_map.insert(std::make_pair(first, mask));

		// Set the bit at the current power
		component_type_bits |= mask;
		++current_power;

		//and(rest);
	}

	template<typename CT, typename... CTs>
	void or(CT first, CTs... rest)
	{
		/*
		1. KEEP TRACK OF EVERY SINGLE COMPONENT AND ITS TYPE
		2. CREATE A BIG ASS VALUE WITH LOADS OF MASK BITS FOR EACH COMPONENT TYPE
		3. WHEN CREATING SYSTEMS: USE AND/OR WITH THESE MASK BITS
		*/
	}

	BitMask getMask(ecs::ComponentType type)
	{
		std::unordered_map<ecs::ComponentType, BitMask>::const_iterator got = id_to_mask_map.find(type);
		if (got != id_to_mask_map.end())
		{
			return got->first;
		}
		return 0;
	}
};