#pragma once

#include "ecs\component.h"
#include "ecs\component_type.h"

#include "math.h"

struct PositionComponent : public ecs::Component
{
	static constexpr ecs::ComponentType type = 1339;

	float x;
	float y;
	float z;
};