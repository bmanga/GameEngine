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

		for (auto ct : entity_components)
		{
			bool register_entity = true;

			// Check if entity component is in system-required components
			const bool is_in_required = system_required_components.find(ct) != system_required_components.end();
			if (!is_in_required) break;

			auto and_relationships = (*system)->getAndRelationships();
			auto or_relationships = (*system)->getOrRelationships();

			// Loop through all AND relationship sets
			for (auto and_set : and_relationships)
			{
				// Check if entity component is in relationship set
				const bool is_in_and = and_set.find(ct) != and_set.end();
				if (is_in_and)
				{
					// Make sure that entity has all components in AND relationship
					// NOTE: Not sure if this does what I want it to...
					if (!std::includes(entity_components.begin(), entity_components.end(), and_set.begin(), and_set.end()))
					{
						register_entity = false;
					}
					//// Make sure that entity has all components in AND relationship
					//for (std::set<ComponentType>::iterator i = and_set.begin(); i != and_set.end(); i++)
					//{
					//	// If entity doesn't have all components in the AND set, do not register the entity
					//	if (entity_components.find(*i) == entity_components.end())
					//		register_entity = false;
					//}
				}
			}

			// Loop through all OR relationship sets
			for (auto or_set : or_relationships)
			{
				// Check if entity component is in relationship set
				const bool is_in_or = or_set.find(ct) != or_set.end();
				if (!is_in_or) register_entity = false;
			}

			// Register the entity in the system if it meets all AND/OR relationships
			if (register_entity)
			{
				(*system)->registerEntity(entity);
				++associated_systems;
			}
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