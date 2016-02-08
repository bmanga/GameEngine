#pragma once

#include "shader.h"

class ShaderProgram
{
private:
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
};