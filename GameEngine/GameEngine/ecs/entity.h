#pragma once

namespace ecs
{

/*
	An Entity represents an object, but does not contain any data on its own, nor any logic.
	It is defined as an aggregation of Components, processed and updated by associated Systems.
*/
typedef unsigned int Entity;

/*
	Entity IDs are strictly positive integers.
*/
static const Entity _invalid_entity = 0;

}