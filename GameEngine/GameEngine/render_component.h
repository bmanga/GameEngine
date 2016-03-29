#pragma once

#include "ecs\component.h"

#include "texture.h"
#include "Mesh_v2.h"
#include "shader_program.h"

struct RenderComponent : public ecs::Component
{
	static constexpr ecs::ComponentType type = 1337;

	int x, y, z;				// Make component
	int rot_x, rot_y, rot_z;	// Make component
	ShaderProgram* program;
	Texture* texture;
	todo::Mesh* mesh;
};