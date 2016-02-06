#include "ShaderProgram_v2.h"
#include <GL/glew.h>
#include <fstream>
#include "types.h"

using Lemur::i32;
static Lemur::u32 active_prog_id = 0;
std::string Lemur::ShaderProgram_v2::DEFAULT_LOC("../assets/shaders/");

Lemur::ShaderProgram_v2::ShaderProgram_v2(const char* vertex_shader, 
	const char* fragment_shader) : ShaderProgram_v2()
{
	compileShaders(vertex_shader, fragment_shader);
}

Lemur::ShaderProgram_v2::ShaderProgram_v2()
{

}

void Lemur::ShaderProgram_v2::compileShaders(const char* vertex_shader, const char* fragment_shader) 
{
	vert_id = glCreateShader(GL_VERTEX_SHADER);
	frag_id = glCreateShader(GL_FRAGMENT_SHADER);

	compileShader(vert_id, vertex_shader);
	compileShader(frag_id, fragment_shader);
}

Lemur::u32 Lemur::ShaderProgram_v2::getVertID() const
{
	return vert_id;
}

Lemur::u32 Lemur::ShaderProgram_v2::getFragID() const
{
	return frag_id;
}

bool Lemur::ShaderProgram_v2::compileProgram()
{
	if (!(vert_id && frag_id)) return false;
	if(!program_id)
	{
		program_id = glCreateProgram();
		glAttachShader(program_id, vert_id);
		glAttachShader(program_id, frag_id);
	}

	return true;

}

void Lemur::ShaderProgram_v2::link() const
{
	glLinkProgram(program_id);
	//TODO: check for errors
}

//void Lemur::ShaderProgram_v2::unLink() const
//{
//	
//}

void Lemur::ShaderProgram_v2::compileShader(u32 id, const char* source)
{
	using namespace std;

	std::string file = DEFAULT_LOC + source;
	ifstream shader_code(file, ios::in | ios::binary);
	
	if (!shader_code) printf(("shader '" + file + "' not found").c_str());
	//get file size
	shader_code.seekg(0, ios::end);
	i32 length = static_cast<u32>(shader_code.tellg());
	shader_code.seekg(0, ios::beg);

	char* buffer = new char[length];
	shader_code.read(buffer, length);
	shader_code.close();
	const char* ptr = buffer;
	glShaderSource(id, 1, &ptr, &length);
	glCompileShader(id);

#if !defined(DEBUG)
	i32 success = false;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		printf("Unabl to compile vertex shader %d\n", id);
		printLog(id);
	}
#endif
}

void Lemur::ShaderProgram_v2::printLog(u32 id)
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
i32 Lemur::ShaderProgram_v2::getAttribLocation(const char* name)
{
	if (!isActive())
	{
		use();
		printf("Warning: Attempted to get attribute location from non-active shader program. Program %d was bound.", program_id);
	}

	int location = glGetAttribLocation(program_id, name);
	if (location == -1)
	{
		printf("%s is not a valid GLSL program attribute!\n", name);
	}

	return location;
}

i32 Lemur::ShaderProgram_v2::getUniformLocation(const char* name)
{
	if (!isActive())
	{
		use();
		printf("Warning: Attempted to get uniform location from non-active shader program. Program %d was bound.", program_id);
	}

	int location = glGetUniformLocation(program_id, name);
	if (location == -1)
	{
		printf("%s is not a valid GLSL program uniform!\n", name);
	}

	return location;
}

bool Lemur::ShaderProgram_v2::isActive()
{
	return program_id == active_prog_id;
}

void Lemur::ShaderProgram_v2::use()
{
	glUseProgram(program_id);
	active_prog_id = program_id;
}

void Lemur::ShaderProgram_v2::printLog()
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