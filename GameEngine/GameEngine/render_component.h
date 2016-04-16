#pragma once

#include <memory>

#include "texture.h"
#include "shader_program.h"

//namespace Lemur
//{
//class Mesh;
//class Texture;
//}
struct CRenderable
{
	int rot_x, rot_y, rot_z;	// Make component
	ShaderProgram* program;
	std::shared_ptr<Lemur::Texture> texture;
	std::shared_ptr<Lemur::Mesh> mesh;
};