#include "texture.h"

#include <stdio.h>

Texture::Texture(const char* path)
{
	glGenTextures(1, &id);
	bind();

	unsigned char* pixels = loadPixels(path);
	if (!pixels)
	{
		printf("%s is not a valid texture file!", path);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	freePixels(pixels);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

void Texture::setWrapType(TextureWrapType s_type, TextureWrapType t_type)
{
	if (!isBound())
		bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s_type);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t_type);
}

void Texture::setInterpolation(TextureInterpolationType min_type, TextureInterpolationType mag_type)
{
	if (!isBound())
		bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_type);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_type);
}

void Texture::generateMipmap()
{
	if (!isBound())
		bind();

	glGenerateMipmap(GL_TEXTURE_2D);
}

bool Texture::isBound()
{
	return id == bound_id;
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
	bound_id = id;

	// TODO: Don't forget about unbinding!!!
}

unsigned char* Texture::loadPixels(const char* path)
{
	return SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
}

void Texture::freePixels(unsigned char* pixels)
{
	SOIL_free_image_data(pixels);
}

unsigned int Texture::bound_id = 0;