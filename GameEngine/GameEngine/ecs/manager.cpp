#include "manager.h"

#include <algorithm>

namespace ecs
{

Manager::Manager() :
	last_entity(_invalid_entity),
	entities(),
	component_stores(),
	systems()
{

}

Manager::~Manager()
{

}

/*
	Adds a System.
*/
void Manager::addSystem(const System::ptr& system_ptr)
{
	// Checks that the required Components are specified
	if ((!system_ptr) || (system_ptr->getRequiredComponents().empty()))
	{
		Lemur::ConsoleLogger::Error("manager.cpp", "System should specify required Components");
	}

	// Simply copy the pointer (instead of moving it) to allow for multiple insertions of the same shared pointer
	systems.push_back(system_ptr);
}

/*
	Registers an Entity to all matching Systems.
*/
size_t Manager::registerEntity(const Entity entity)
{
	size_t associated_systems = 0;

	auto e = entities.find(entity);
	if (entities.end() == e)
	{
		Lemur::ConsoleLogger::Error("manager.cpp", "The Entity does not exist");
	}
	auto entity_components = (*e).second;

	// Cycles through all the Systems to check which ones are interested in the Entity
	for (auto system = systems.begin(); system != systems.end(); ++system)
	{
		auto system_required_components = (*system)->getRequiredComponents();
		if (std::includes(entity_components.begin(), entity_components.end(), system_required_components.begin(), system_required_components.end()))
		{
			// Registers the matching Entity
			(*system)->registerEntity(entity);
			++associated_systems;
		}
	}

	return associated_systems;
}

/*
	Unregisters an Entity from all matching Systems.
*/
size_t Manager::unregisterEntity(const Entity entity)
{
	size_t associated_systems = 0;

	auto e = entities.find(entity);
	if (entities.end() == e)
	{
		Lemur::ConsoleLogger::Error("manager.cpp", "The Entity does not exist");
	}
	auto entity_components = (*e).second;

	// Cycles through all the Systems to unregister the Entity
	for (auto system = systems.begin(); system != systems.end(); ++system)
	{
		// Try to unregister the matching Entity
		associated_systems += (*system)->unregisterEntity(entity);
	}
	
	return associated_systems;
}

/*
	Updates all Entities of all Systems.
*/
size_t Manager::updateEntities(float elapsed_time)
{
	size_t updated_entities = 0;

	for (auto system = systems.begin(); system != systems.end(); ++system)
	{
		updated_entities += (*system)->updateEntities(elapsed_time);
	}

	return updated_entities;
}

}