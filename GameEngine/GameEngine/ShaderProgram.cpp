#include <GL/glew.h>
#include <stdio.h>
#include <fstream>
#include "FilePaths.h"
#include "ConsoleLogger.h"
#include "Lemur.h"

#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path)
{
	vertex_shader_id = glCreateShader(VERTEX);
	fragment_shader_id = glCreateShader(FRAGMENT);

	vertex_source = loadShaderSource(vertex_shader_path);
	fragment_source = loadShaderSource(fragment_shader_path);
}

std::string ShaderProgram::loadShaderSource(const char* name)
{
	using namespace std;

	ifstream shader_code(Lemur::SHADER_PATH / name, ios::in | ios::binary);

	if (!shader_code)
	{
		Lemur::ConsoleLogger::Error(CODE_LOCATION, CSTR2("Shader ", name, " not found"));
		return NULL;
	}

	// Get file size
	shader_code.seekg(0, ios::end);
	int32_t length = static_cast<uint32_t>(shader_code.tellg());
	shader_code.seekg(0, ios::beg);

	char* buffer = new char[length + 1];
	buffer[length] = '\0';
	shader_code.read(buffer, length);
	shader_code.close();

	std::string source(buffer);
	delete[] buffer;

	return source;
}

void ShaderProgram::addDefine(const char* name, const char* value, ShaderType type)
{
	std::string statement = "#define ";
	statement.append(name);
	statement.append(" ");
	statement.append(value);
	statement.append("\r\n");
	switch (type)
	{
	case VERTEX:
	{
		std::size_t pos = vertex_source.find("#ifdef");
		if (pos != std::string::npos)
		{
			vertex_source.insert(pos, statement);
		}
	}
	break;
	case FRAGMENT:
	{
		std::size_t pos = fragment_source.find("#ifdef");
		if (pos != std::string::npos)
		{
			fragment_source.insert(pos, statement);
		}
	}
	break;
	}
}

void ShaderProgram::addDefine(const char* name, ShaderType type)
{
	addDefine(name, "", type);
}

bool ShaderProgram::compile()
{
	const char* vertex_ptr = vertex_source.c_str();
	const char* fragment_ptr = fragment_source.c_str();

	glShaderSource(vertex_shader_id, 1, &vertex_ptr, NULL);
	glShaderSource(fragment_shader_id, 1, &fragment_ptr, NULL);

	if (!compileShader(vertex_shader_id))
	{
		//Lemur::ConsoleLogger::Error(CODE_LOCATION, CSTR2("Unable to compile vertex shader \'", vertex_shader_path, "\' [ID: ", vertex_shader_id, "]"));
		return false;
	}
	if (!compileShader(fragment_shader_id))
	{
		//Lemur::ConsoleLogger::Error(CODE_LOCATION, CSTR2("Unable to compile fragment shader \'", fragment_shader_path, "\' [ID: ", fragment_shader_id, "]"));
		return false;
	}

	program_id = glCreateProgram();

	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, fragment_shader_id);

	if (!linkProgram())
	{
		//Lemur::ConsoleLogger::Error(CODE_LOCATION, CSTR2("Error linking shader program with \'" + vertex_shader_path "\' and \'", fragment_shader_path, "\' [ID: ", program_id, "]"));
		return false;
	}

	return true;
}

bool ShaderProgram::compileShader(unsigned int shader_id)
{
	bool success = true;

	glCompileShader(shader_id);

	int compiled = false;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		printLog();
		success = false;
	}

	return success;
}

bool ShaderProgram::linkProgram()
{
	glLinkProgram(program_id);

	//Check for errors
	int success;
	glGetProgramiv(program_id, GL_LINK_STATUS, &success);
	if (success != GL_TRUE)
	{
		//printf("Error linking shader program with \'%s\' and \'%s\' [ID: %d]:\n", vertex_shader->getPath(), fragment_shader->getPath(), id);
		printLog();
		return false;
	}

	return true;
}

int ShaderProgram::getAttribLocation(const char* name)
{
	if (!isUsing())
	{
		use();
		Lemur::ConsoleLogger::Warning(CODE_LOCATION, CSTR2("Attempted to get attribute location from non-bound shader program. Program ", program_id, " was bound."));
	}

	int location = glGetAttribLocation(program_id, name);
	if (location == -1)
	{
		//Lemur::ConsoleLogger::Debug(CODE_LOCATION, CSTR2(name, " is not a valid GLSL program attribute!"));
	}

	return location;
}

int ShaderProgram::getUniformLocation(const char* name)
{
	if (!isUsing())
	{
		use();
		Lemur::ConsoleLogger::Warning(CODE_LOCATION, CSTR2("Attempted to get uniform location from non-bound shader program. Program ", program_id, " was bound."));
	}

	int location = glGetUniformLocation(program_id, name);
	if (location == -1)
	{
		//Lemur::ConsoleLogger::Debug(CODE_LOCATION, CSTR2(name, " is not a valid GLSL program uniform!"));
	}

	return location;
}

bool ShaderProgram::isUsing()
{
	return program_id == using_id;
}

void ShaderProgram::use()
{
	glUseProgram(program_id);
	using_id = program_id;
}

void ShaderProgram::printLog()
{
	//Make sure name is shader
	if (glIsProgram(program_id))
	{
		//Program log length
		int info_log_length = 0;
		int max_length = info_log_length;

		//Get info string length
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &max_length);

		//Allocate string
		char* info_log = new char[max_length];

		//Get info log
		glGetProgramInfoLog(program_id, max_length, &info_log_length, info_log);
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
		printf("Name %d is not a program\n", program_id);
	}
}

std::string* ShaderProgram::getParameters()
{
	return RESERVED;
}

unsigned int ShaderProgram::using_id = 0;