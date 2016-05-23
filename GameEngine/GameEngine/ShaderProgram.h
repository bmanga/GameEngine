#pragma once

#include <string>

enum ShaderType
{
	VERTEX = GL_VERTEX_SHADER,
	FRAGMENT = GL_FRAGMENT_SHADER
};

class ShaderProgram
{
private:
	std::string RESERVED[14] = {
		"ModelView",
		"ModelViewI",
		"ModelViewT",
		"ModelViewIT",
		"ProjectionMatrix",
		"ProjectionMatrixI",
		"ProjectionMatrixT",
		"ProjectionMatrixIT",
		"ViewMatrix",
		"ViewMatrixI",
		"ViewMatrixIT",
		"CamPos",
		"TimeInSeconds",
		"TimeInMilliseconds"
	};

	unsigned int program_id;
	static unsigned int using_id;

	unsigned int vertex_shader_id;
	unsigned int fragment_shader_id;

	std::string vertex_source;
	std::string fragment_source;

	std::string loadShaderSource(const char* name);
	bool compileShader(unsigned int shader_id);

	bool linkProgram();

	void printLog();

public:
	ShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path);

	void addDefine(const char* name, const char* value, ShaderType type);
	void addDefine(const char* name, ShaderType type);
	bool compile();

	int getAttribLocation(const char* name);
	int getUniformLocation(const char* name);

	bool isUsing();
	void use();

	std::string* getParameters();
};