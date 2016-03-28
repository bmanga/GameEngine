#pragma once

#include "texture.h"
#include "Mesh.h"
#include "shader_program.h"

class RenderComponent
{
public:
	int x, y, z;				// Make component
	int rot_x, rot_y, rot_z;	// Make component?
	ShaderProgram* program;
	Texture* texture;
	Mesh* mesh;
};