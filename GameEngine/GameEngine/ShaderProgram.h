#pragma once

#include <string>
#include "FilePaths.h"
#include "ConsoleLogger.h"
#include <fstream>
#include "Lemur.h"

namespace Lemur {

enum ShaderType
{
	VERTEX = GL_VERTEX_SHADER,
	FRAGMENT = GL_FRAGMENT_SHADER
};

class ShaderPreprocessor
{
private:
	std::string defines;

public:
	void addDefine(const char* name, const char* value);
	void process(std::string& source) const;
};

class Shader
{
public:
	Shader(ShaderType type, fs::path shader);
	Shader(ShaderType type, const char* name);;
	~Shader();

	GLuint getId() const;
	ShaderPreprocessor* getPreprocessor();

	void upload(bool preprocess = true);
	bool compile() const;
	void printInfoLog() const;
	bool loadSource();

private:
	GLuint id = 0;
	ShaderType type;
	fs::path path;
	std::string source;
	ShaderPreprocessor preprocessor;
};

class ShaderProgram
{
private:
	//TODO: RESERVED can be array of chars and be constexpr
	static const std::string RESERVED[14]; 

	Shader vertexShader;
	Shader fragmentShader;

	GLuint id = 0;
	static GLuint activeId;

public:
	ShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path);
	~ShaderProgram();

	bool compile();
	bool linkProgram() const;
	void addDefine(const char* name, const char* value, ShaderType type);
	void addDefine(const char* name, ShaderType type);

	int getAttribLocation(const char* name) const;
	int getUniformLocation(const char* name) const;

	bool isUsing() const;
	void use() const;

	Shader* getShader(ShaderType type);

	static const std::string* getParameters();
};

	int operator ""_uniform(const char* name, std::size_t);

} // namespace Lemur
