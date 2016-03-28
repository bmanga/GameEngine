#include <GL/glew.h>
#include <stdio.h>

#include "shader_program.h"

ShaderProgram::ShaderProgram(Shader& vertex_shader, Shader& fragment_shader)
{
	id = glCreateProgram();

	glAttachShader(id, vertex_shader.getId());
	glAttachShader(id, fragment_shader.getId());

	link();
}

ShaderProgram::ShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path)
	
{
	
	vertex_shader = new Shader(VERTEX, vertex_shader_path);
	fragment_shader = new Shader(FRAGMENT, fragment_shader_path);
	id = glCreateProgram();

	glAttachShader(id, vertex_shader->getId());
	glAttachShader(id, fragment_shader->getId());

	link();
}

void ShaderProgram::link()
{
	glLinkProgram(id);

	//Check for errors
	int success = true;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (success != GL_TRUE)
	{
		printf("Error linking shader program with \'%s\' and \'%s\' [ID: %d]:\n", vertex_shader->getPath(), fragment_shader->getPath(), id);
		printLog();
		success = false;
	}
}

int ShaderProgram::getAttribLocation(const char* name)
{
	if (!isUsing())
	{
		use();
		printf("Warning: Attempted to get attribute location from non-active shader program. Program %d was bound.", id);
	}

	int location = glGetAttribLocation(id, name);
	if (location == -1)
	{
		printf("%s is not a valid GLSL program attribute!\n", name);
	}

	return location;
}

int ShaderProgram::getUniformLocation(const char* name)
{
	if (!isUsing())
	{
		use();
		printf("Warning: Attempted to get uniform location from non-active shader program. Program %d was bound.", id);
	}

	int location = glGetUniformLocation(id, name);
	if (location == -1)
	{
		printf("%s is not a valid GLSL program uniform!\n", name);
	}

	return location;
}

bool ShaderProgram::isUsing()
{
	return id == using_id;
}

void ShaderProgram::use()
{
	glUseProgram(id);
	using_id = id;
}

void ShaderProgram::printLog()
{
	//Make sure name is shader
	if (glIsProgram(id))
	{
		//Program log length
		int info_log_length = 0;
		int max_length = info_log_length;

		//Get info string length
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &max_length);

		//Allocate string
		char* info_log = new char[max_length];

		//Get info log
		glGetProgramInfoLog(id, max_length, &info_log_length, info_log);
		if (info_log_length > 0)
		{
			//Print Log
			printf("%s\n", info_log);
		}

		//Deallocate string
		delete[] info_log;
	}
	else
	{
		printf("Name %d is not a program\n", id);
	}
}

std::string* ShaderProgram::getParameters()
{
	return RESERVED;
}

unsigned int ShaderProgram::using_id = 0;