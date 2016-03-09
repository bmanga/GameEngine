#pragma once

#include "shader.h"

#include <string>

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

	unsigned int id;
	static unsigned int using_id;

	Shader* vertex_shader;
	Shader* fragment_shader;

	void link();

	void printLog();

public:
	ShaderProgram(Shader& vertex_shader, Shader& fragment_shader);
	ShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path);

	int getAttribLocation(const char* name);
	int getUniformLocation(const char* name);

	bool isUsing();
	void use();

	std::string* getParameters();
};