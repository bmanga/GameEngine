#include <GL/glew.h>
#include <stdio.h>

#include "shader.h"

Shader::Shader(int type, const char* source) : source(source)
{
	this->type = type;

	create();
	compile();
}

void Shader::create()
{
	id = glCreateShader(type);
	glShaderSource(id, 1, &source, nullptr);
}

bool Shader::compile()
{
	bool success = true;

	glCompileShader(id);

	int compiled = false;
	glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		printf("Unable to compile vertex shader %d!\n", id);
		printLog();
		success = false;
	}

	return success;
}

void Shader::printLog()
{
	//Make sure name is shader
	if (glIsShader(id))
	{
		//Shader log length
		int info_log_length = 0;
		int max_length = info_log_length;

		//Get info string length
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &max_length);

		//Allocate string
		char* info_log = new char[max_length];

		//Get info log
		glGetShaderInfoLog(id, max_length, &info_log_length, info_log);
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
		printf("Name %d is not a shader\n", id);
	}
}

int Shader::getType()
{
	return type;
}

unsigned int Shader::getId()
{
	return id;
}