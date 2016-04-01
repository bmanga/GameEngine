#pragma once

#include <memory>
#include "ecs\component.h"

#include "texture.h"

#include "shader_program.h"

namespace Lemur {
class Mesh;
class Texture;
}

struct RenderComponent : public ecs::Component
{
	static constexpr ecs::ComponentType type = 1337;

	int x, y, z;				// Make component
	int rot_x, rot_y, rot_z;	// Make component
	ShaderProgram* program;
	std::shared_ptr<Lemur::Texture> texture;
	std::shared_ptr<Lemur::Mesh> mesh;
};