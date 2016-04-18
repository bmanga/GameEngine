#pragma once

#include "Manager.h"
#include "texture.h"

namespace Lemur
{

class TextureManager : public ResourceManager<Texture>
{
private:
	unsigned char* loadPixels(const char* path)
	{
		int width, height;
		return SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);
	}
public:
	std::shared_ptr<Texture> load(const char* filename)
	{
		auto texture = ResourceManager<Texture>::load(filename);
	}
};

}