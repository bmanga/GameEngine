#pragma once

#include "component_type.h"
#include "entity.h"

#include <set>
#include <memory>

namespace ecs
{

class Manager;

/*
	A System manages any Entity which has all the required Components.

	A System is a program running logic and updating data on any Entity holding a certain set of Components.
	Systems run repeatedly on all corresponding Entities.

	This is a base class that must be subclassed.
*/
class System
{
private:
	/*
		Lists the types of all the Components required by the System.
	*/
	ComponentTypeSet required_components;

	/*
		Lists all the matching Entities having required Components for the System.
	*/
	std::set<Entity> matching_entities;

protected:

	/*
		Specifies the Components that are required by the System.
	*/
	inline void setRequiredComponents(ComponentTypeSet&& required_components)
	{
		this->required_components = std::move(required_components);
	}

	/*
		Reference to the Manager; needed to access Entity Components.
	*/
	Manager& manager;

public:

	typedef std::shared_ptr<System> ptr;

	explicit System(Manager& manager);

	virtual ~System();

	/*
		Gets the types of all the Components that are required by the System.
	*/
	inline const ComponentTypeSet& getRequiredComponents() const
	{
		return required_components;
	}

	/*
		Registers a matching Entity which has all the required Components.
	*/
	inline bool registerEntity(Entity entity)
	{
		return matching_entities.insert(entity).second;
	}

	/*
		Unregisters an Entity from the System.
	*/
	inline size_t unregisterEntity(Entity entity)
	{
		return matching_entities.erase(entity);
	}

	/*
		Checks if this System has registered the specified Entity.
	*/
	inline bool hasEntity(Entity entity) const
	{
		return (matching_entities.end() != matching_entities.find(entity));
	}

	/*
		Update function for all Entities in the System.
	*/
	size_t updateEntities(float elapsed_time);

	/*
		Update function for a specific Entity within the System.
	*/
	virtual void updateEntity(float elapsed_time, Entity entity) = 0;
};

}