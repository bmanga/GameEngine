#pragma once

#include "shader.h"

class ShaderProgram
{
private:
	unsigned int id;

	static unsigned int using_id;

	void printLog();

public:
	ShaderProgram(Shader& vertex_shader, Shader& fragment_shader);

	int getAttribLocation(const char* name);
	int getUniformLocation(const char* name);

	bool isUsing();
	void use();
};