#pragma once

#include "entity.h"
#include "component.h"

#include <unordered_map>
#include <memory>

namespace ecs
{

/*
	Abstract base class for all templated ComponentStores.
*/
class IComponentStore
{
public:
	// TODO: Remove pointer and move ComponentStore into the manager
	typedef std::unique_ptr<IComponentStore> ptr;
};

/*
	A ComponentStore to keep all the data of a certain type of Component for all concerned Entities.
*/
template<typename C>
class ComponentStore : public IComponentStore
{
	static_assert(std::is_base_of<Component, C>::value, "C must be derived from the Component struct");
	static_assert(C::type != _invalid_component_type, "C must define a valid non-zero type");

private:
	std::unordered_map<Entity, C> store;
	static constexpr ComponentType type = C::type;

public:
	//ComponentStore()
	//~ComponentStore();

	/*
		Add (move) a Component (of the same type as ComponentStore) associated to an Entity.
	*/
	inline bool add(const Entity entity, C&& component)
	{
		return store.insert(std::make_pair(entity, std::move(component))).second;
	}

	/*
		Remove (destroy) the specified Component associated to an Entity.
	*/
	inline bool remove(Entity entity)
	{
		return (0 < store.erase(entity));
	}

	/*
		Test if the store contains a Component for the specified Entity.
	*/
	inline bool has(Entity entity) const
	{
		return (store.end() != store.find(entity));
	}

	/*
		Get access to the Component associated with the specified Entity.
	*/
	inline C& get(Entity entity)
	{
		return store.at(entity);
	}

	/*
		Extract (move out) the Component associated with the specified Entity.
	*/
	inline C extract(Entity entity)
	{
		C component = std::move(store.at(entity));
		store.erase(entity);
		return component;
	}

	/*
		Get access to the underlying Component map.
	*/
	inline const std::unordered_map<Entity, C>& getComponents()
	{
		return store;
	}
};

}