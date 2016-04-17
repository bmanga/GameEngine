#pragma once

#include "OpenGLMaster.h"

namespace Lemur {
enum TextureFormat
{
	RGB = GL_RGB,
	RED = GL_RED
};

enum TextureWrapType
{
	REPEAT = GL_REPEAT,
	MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
	CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
	CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER
};

enum TextureInterpolationType
{
	NEAREST = GL_NEAREST,
	LINEAR = GL_LINEAR,
	NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
};

class Texture
{
private:
	unsigned int id;
	int width, height;

	static unsigned int bound_id;

	unsigned char* loadPixels(const char* path);
	void freePixels(unsigned char* pixels);

public:
	Texture(const char* path);
	Texture(unsigned char* pixels, unsigned int width, unsigned int height);
	Texture(unsigned char* pixels, unsigned int width, unsigned int height, TextureFormat format, TextureFormat internal_format);
	~Texture();

	void setWrapType(TextureWrapType s_type, TextureWrapType t_type);
	void setInterpolation(TextureInterpolationType min_type, TextureInterpolationType mag_type);

	void generateMipmap();

	bool isBound();
	void bind();

	unsigned int getId()
	{
		return id;
	}
};

}// namespace Lemur;