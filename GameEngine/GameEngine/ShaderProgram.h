#pragma once

#include <string>
#include <fstream>
#include "FilePaths.h"
#include <unordered_map>

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
	Lemur::fs::path filePath;

	static bool processNextInclude(std::string& source);
	void processDefines(std::string& source) const;
public:
	explicit ShaderPreprocessor(Lemur::fs::path filePath);
	void addDefine(const char* name, const char* value);
	fs::path sourceFilename() const;
	const fs::path& sourceFilepath() const { return filePath;  }
	
	std::string process() const;
};

class Shader
{
public:
	Shader(ShaderType type, fs::path shader);
	Shader(ShaderType type, const char* name);;
	~Shader();

	GLuint getId() const;
	ShaderPreprocessor* getPreprocessor();

	void upload() const;
	bool compile() const;
	void printInfoLog() const;


private:
	GLuint id = 0;
	ShaderType type;
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
