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
	//TODO: RESERVED can be array of chars and be constexpr
	static const std::string RESERVED[14]; 

	unsigned int program_id;
	static unsigned int using_id;

	unsigned int vertex_shader_id;
	unsigned int fragment_shader_id;

	std::string vertex_source;
	std::string fragment_source;

	std::string loadShaderSource(const char* name) const;
	bool compileShader(unsigned int shader_id) const;

	bool linkProgram() const;

	void printLog() const;

public:
	ShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path);

	void addDefine(const char* name, const char* value, ShaderType type);
	void addDefine(const char* name, ShaderType type);
	bool compile();

	int getAttribLocation(const char* name) const;
	int getUniformLocation(const char* name) const;

	bool isUsing() const;
	void use() const;

	static const std::string* getParameters();
};