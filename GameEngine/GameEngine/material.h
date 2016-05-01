#pragma once

#include "texture.h"
#include "ShaderProgram.h"

#include <memory>

namespace Lemur
{

struct Material
{
	// NOTE:
	// The shader program could be decided by the engine instead.
	// Time will tell.
	//
	// Materials should be sorted with the following attributes
	// taking precedence over the others:
	// - Texture
	// - Material components
	// - Shininess
	// - Blend function
	std::shared_ptr<ShaderProgram> shader;

	std::shared_ptr<Texture> texture;
	std::shared_ptr<Texture> bump_map;
	std::shared_ptr<Texture> normal_map;
	std::shared_ptr<Texture> specular_map;
	std::shared_ptr<Texture> mask_map;

	float color[3];

	float ambient[3];
	float diffuse[3];
	float emissive[3];
	float specular[3];

	float shininess;
	float transparency;
};

}