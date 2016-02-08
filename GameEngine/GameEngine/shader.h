#pragma once

#include <GL/glew.h>
#include <string>

enum ShaderType
{
	VERTEX = GL_VERTEX_SHADER,
	FRAGMENT = GL_FRAGMENT_SHADER
};

class Shader
{
private:
	unsigned int id;
	int type;
	const char* path;

	void loadSource(const char* path);
	void create();
	bool compile();

	void printLog();

	static std::string DEFAULT_LOCATION;

public:
	Shader(ShaderType type, const char* path);

	int getType();
	unsigned int getId();
	const char* getPath();
};