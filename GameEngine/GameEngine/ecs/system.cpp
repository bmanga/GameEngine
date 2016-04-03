#include "system.h"
#include "manager.h"

namespace ecs
{

System::System(Manager& manager) : manager(manager)
{

}

System::~System()
{

}

/*
	Updates all matching Entities.
*/
size_t System::updateEntities(float elapsed_time)
{
	size_t updated_entities = 0;

	begin();

	for (auto entity = matching_entities.begin(); entity != matching_entities.end(); ++entity)
	{
		updateEntity(elapsed_time, *entity);
		++updated_entities;
	}

	end();

	return updated_entities;
}

}