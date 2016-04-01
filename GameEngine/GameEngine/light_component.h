#pragma once

#include "ecs\component.h"
#include "ecs\component_type.h"

#include "math.h"

struct LightComponent : public ecs::Component
{
	static const ecs::ComponentType type = 1338;

	glm::vec3 position;	// Should be a PositionComponent?
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};