#pragma once

#include <set>

namespace ecs
{

/*
	A ComponentType is a positive ID referencing a type of Component.
*/
typedef unsigned int ComponentType;

/*
	ComponentTypes are strictly positive IDs.
*/
static const ComponentType _invalid_component_type = 0;

/*
	Sorted List (set) of ComponentType (of an Entity, or required by a System).
*/
typedef std::set<ComponentType> ComponentTypeSet;

}