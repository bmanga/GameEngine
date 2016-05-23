#pragma once

#include <memory>

#include "Texture.h"
#include "ShaderProgram.h"
#include "Material.h"

struct CRenderable
{
	int rot_x, rot_y, rot_z;	// Make component (CTransform?)
	std::shared_ptr<Lemur::Material> material;
	std::shared_ptr<Lemur::Mesh> mesh;
};