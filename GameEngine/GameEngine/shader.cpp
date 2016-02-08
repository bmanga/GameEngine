#include "shader.h"

#include <stdio.h>
#include <fstream>

std::string Shader::DEFAULT_LOCATION("../assets/shaders/");

Shader::Shader(ShaderType type, const char* path)
{
	this->type = type;
	this->path = path;

	create();
	loadSource(path);
	compile();
}

void Shader::loadSource(const char* path)
{
	using namespace std;

	std::string file = DEFAULT_LOCATION + path;
	ifstream shader_code(file, ios::in | ios::binary);

	if (!shader_code) printf(("Shader '" + file + "' not found!").c_str());

	// Get file size
	shader_code.seekg(0, ios::end);
	int32_t length = static_cast<uint32_t>(shader_code.tellg());
	shader_code.seekg(0, ios::beg);

	char* buffer = new char[length];
	shader_code.read(buffer, length);
	shader_code.close();
	const char* ptr = buffer;

	glShaderSource(id, 1, &ptr, &length);
}

void Shader::create()
{
	id = glCreateShader(type);
}

bool Shader::compile()
{
	bool success = true;

	glCompileShader(id);

	int compiled = false;
	glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		if (type == VERTEX) printf("Unable to compile vertex shader \'%s\' [ID: %d]:\n", path, id);
		else if (type == FRAGMENT) printf("Unable to compile fragment shader \'%s\' [ID: %d]:\n", path, id);
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
		printf("Shader %d is not a shader!\n", id);
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

const char* Shader::getPath()
{
	return path;
}