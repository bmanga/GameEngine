#pragma once

#include "component_type.h"

template <size_t N>
struct ConstVal
{
	enum {value = N};
};

namespace ecs
{

/*
	A Component is a data structure that maintains a sub-state of an Entity.
	The state of any Entity can be described by a few standard Components.

	Every Component class must be dervied from this struct and define its own positive ComponentType.
*/
struct Component
{
	// Default invalid component type
	static constexpr ComponentType type = _invalid_component_type;

};

}
