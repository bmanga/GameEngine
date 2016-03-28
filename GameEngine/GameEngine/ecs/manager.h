#pragma once

#include "entity.h"
#include "component.h"
#include "component_type.h"
#include "component_store.h"
#include "system.h"

#include <map>
#include <unordered_map>
#include <set>
#include <vector>
#include <memory>
#include <cassert>
#include <limits>
#include <stdexcept>

namespace ecs
{

/*
	Manages associations of Entity, Component and System.
*/
class Manager
{
private:

	Entity last_entity;
	std::unordered_map<Entity, ComponentTypeSet> entities;
	std::map<ComponentType, IComponentStore::ptr> component_stores;
	std::vector<System::ptr> systems;

public:

	Manager();
	virtual ~Manager();

	/*
		Creates a ComponentStore for a certain type of Component.
	*/
	template<typename C>
	inline bool createComponentStore()
	{
		static_assert(std::is_base_of<Component, C>::value, "C must be derived from the Component struct");
		static_assert(C::type != _invalid_component_type, "C must define a valid non-zero type");
		return component_store;
	}

	/*
		Gets (access to) the ComponentStore of a certain type of Component.
	*/
	template<typename C>
	inline ComponentStore<C>& getComponentStore()
	{
		static_assert(std::is_base_of<Component, C>::value, "C must be derived from the Component struct");
		static_assert(C::type != _invalid_component_type, "C must define a valid non-zero type");
		auto component_store = component_stores.find(C::type);
		if (component_stores.end() == component_store)
		{
			throw std::runtime_error("The ComponentStore does not exist");
		}
		return reinterpret_cast<ComponentStore<C>&>(*(component_store->second));
	}

	/*
		Adds a System.

		Requires a shared pointer (instead of a unique_ptr) to a System to be able to handle multiple entries
		into the vector of managed Systems (for multi-execution of the same System).
	*/
	void addSystem(const System::ptr& system_ptr);

	/*
		Creates a new Entity - simply allocates a new ID.
	*/
	inline Entity createEntity()
	{
		assert(last_entity < std::numeric_limits<Entity>::max());
		entities.insert(std::make_pair((last_entity + 1), ComponentTypeSet()));
		return (++last_entity);
	}

	/*
		Adds (moves) a Component (of the same type as the ComponentStore) associated to an Entity.
	*/
	template<typename C>
	inline bool addComponent(const Entity entity, C&& component)
	{
		static_assert(std::is_base_of<Component, C>::value, "C must be derived from the Component struct");
		static_assert(C::type != _invalid_component_type, "C must define a valid non-zero type");
		auto entity = entities.find(entity);
		if (entities.end() == entity)
		{
			throw std::runtime_error("The Entity does not exist");
		}
		(*entity).second.insert(C::type);
		return getComponentStore<C>().add(entity, std::move(component));
	}

	/*
		Registers an Entity to all matching Systems.
	*/
	size_t registerEntity(const Entity entity);

	/*
		Unregisters an Entity from all matching Systems.
	*/
	size_t unregisterEntity(const Entity entity);

	/*
		Updates all Entities of all Systems.
	*/
	size_t updateEntities(float elapsed_time);
};

}