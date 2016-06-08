#pragma once

#include <string>
#include "FilePaths.h"

enum ShaderType
{
	VERTEX = GL_VERTEX_SHADER,
	FRAGMENT = GL_FRAGMENT_SHADER
};

class ShaderProgram
{
private:
	//TODO: RESERVED can be array of chars and be constexpr
	static const std::string RESERVED[14]; 

	unsigned int program_id;
	static unsigned int using_id;

	unsigned int vertex_shader_id;
	unsigned int fragment_shader_id;

	std::string vertex_source;
	std::string fragment_source;

	std::string loadShaderSource(const char* name) const;
	std::string loadShaderSource(Lemur::fs::path name) const;
	bool compileShader(unsigned int shader_id) const;

	bool linkProgram() const;

	void printLog(int shader_id) const;

public:
	ShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path);

	void addDefine(const char* name, const char* value, ShaderType type);
	void addDefine(const char* name, ShaderType type);
	bool compile();

	int getAttribLocation(const char* name) const;
	int getUniformLocation(const char* name) const;

	bool isUsing() const;
	void use() const;

	bool recompile(Lemur::fs::path vert, Lemur::fs::path frag)
	{
		if (!vertex_shader_id || !fragment_shader_id)
			return false;

		glDeleteShader(vertex_shader_id);
		glDeleteShader(fragment_shader_id);
		glDeleteProgram(program_id);

		vertex_shader_id = glCreateShader(VERTEX);
		fragment_shader_id = glCreateShader(FRAGMENT);

		vertex_source = loadShaderSource(vert);
		fragment_source = loadShaderSource(frag);
		return compile();
	}

	void setUniform1i(const char* name, int v0) const
	{
		auto location = getUniformLocation(name);
		glUniform1i(location, v0);
	}

	void setUniform3f(const char* name, float v0, float v1, float v2) const
	{
		auto location = getUniformLocation(name);
		glUniform3f(location, v0, v1, v2);
	}

	void setUniformMatrix4fv(const char* name, int n,  GLboolean transpose, 
		const float* ptr) const
	{
		auto location = getUniformLocation(name);
		glUniformMatrix4fv(location, n, transpose, ptr);
	}
	static const std::string* getParameters();
};

namespace Lemur {
	int operator ""_uniform(const char* name, std::size_t);
}
