#include <GL/glew.h>
#include <stdio.h>
#include <fstream>
#include "FilePaths.h"
#include "ConsoleLogger.h"
#include "Lemur.h"
#include "ShaderProgram.h"

const std::string Lemur::ShaderProgram::RESERVED[14]= {
	"ModelView",
	"ModelViewI",
	"ModelViewT",
	"ModelViewIT",
	"ProjectionMatrix",
	"ProjectionMatrixI",
	"ProjectionMatrixT",
	"ProjectionMatrixIT",
	"ViewMatrix",
	"ViewMatrixI",
	"ViewMatrixIT",
	"CamPos",
	"TimeInSeconds",
	"TimeInMilliseconds"
};

namespace {
Lemur::fs::path ParseShaderIncludeFile(
	std::string::const_iterator begin,
	std::string::const_iterator end)
{
	Lemur::fs::path file;

	if (*begin == '<' && *end == '>')
		file /= Lemur::SHADER_INCLUDE_PATH;
	else if (*begin == '"' && *end == '"')
		file /= Lemur::SHADER_PATH;
	else
		return file;

	return file / std::string(begin + 1, end);
}
} //namespace

//Returns true until there are no more includes to parse. If an include is not
//valid, it is removed (but still returns true)
bool Lemur::ShaderPreprocessor::processNextInclude(std::string& source)
{
	auto includePos = source.find("#include");

	if (includePos == source.npos)
		return false;

	auto endIncludePos = source.find('\n', includePos);

	auto includeItr = source.begin() + includePos + 8;
	//remove whitespaces to make parsing easier
	auto endIncludeItr = std::remove(source.begin() + includePos,
		source.begin() + endIncludePos, ' ') - 1;

	auto filePath = ParseShaderIncludeFile(includeItr, endIncludeItr);
	
	std::ifstream includeSource(filePath, std::ios::in | std::ios::binary);

	if (!includeSource)
	{
		auto line = std::count(source.begin(), includeItr, '\n') + 1;

		//if filepath is empty, it is a parse error
		if (filePath.empty())
			Lemur::ConsoleLogger::Error(CODE_LOCATION, Lemur::cstr(
				"In file ", filePath.filename(), 
				": \n\tParse error for shader include on line ", line));
		//Otherwise, the file is missing
		else
			Lemur::ConsoleLogger::Error(CODE_LOCATION, Lemur::cstr(
				"In file ", filePath.filename(), ": \n\tShader include file ", 
				filePath.filename(), " on line ", line, " not found"));
		
		source.replace(includePos, endIncludePos - includePos, "");
		return true;
	}

	source.replace(includePos, endIncludePos - includePos,
		std::string(std::istreambuf_iterator<char>(includeSource), {}));

	return true;
}

void Lemur::ShaderPreprocessor::addDefine(const char* name, const char* value)
{
	auto define = str("#define ", name, " ", value, "\n");
	defines += define;
}

void Lemur::ShaderPreprocessor::processDefines(std::string& source) const
{
	if (defines.size())
	{
		auto definePos = source.find("#version");

		if (definePos == source.npos)
			definePos = 0;
		else
			definePos = source.find_first_of('\n', definePos) + 1;
		
		source.insert(definePos, defines);
	}
}

Lemur::ShaderPreprocessor::ShaderPreprocessor(Lemur::fs::path filePath): 
	filePath(std::move(filePath))
{
}

Lemur::fs::path Lemur::ShaderPreprocessor::sourceFilename() const
{
	return filePath.filename();
}

std::string Lemur::ShaderPreprocessor::process() const
{
	//load shader source
	std::ifstream sourceFile(filePath);
	std::string source(std::istreambuf_iterator<char>(sourceFile), {});
	
	//Add defines
	processDefines(source);

	//Replace Includes
	while (processNextInclude(source));

	return source;
}

Lemur::Shader::Shader(ShaderType type, fs::path shader): type(type), 
	preprocessor(shader)
{
	id = glCreateShader(type);
}

Lemur::Shader::Shader(ShaderType type, const char* name):
	Shader(type, SHADER_PATH / name)
{
}

Lemur::Shader::~Shader()
{
	glDeleteShader(id);
}

GLuint Lemur::Shader::getId() const
{
	return id;
}

Lemur::ShaderPreprocessor* Lemur::Shader::getPreprocessor()
{
	return &preprocessor;
}

void Lemur::Shader::upload() const
{
	std::string source = preprocessor.process();
	const char* sourcePtr = source.c_str();
	glShaderSource(id, 1, &sourcePtr, nullptr);
}

bool Lemur::Shader::compile() const
{
	glCompileShader(id);

	int compiled = false;
	glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		ConsoleLogger::Error(CODE_LOCATION,
			cstr("Unable to compile shader ", preprocessor.sourceFilename()));

		printInfoLog();
		return false;
	}

	ConsoleLogger::Debug(CODE_LOCATION, cstr("Shader ", preprocessor.sourceFilename(),
		" successfully compiled"));
	return true;
}

void Lemur::Shader::printInfoLog() const
{
	int length = 0;
	int maxLength = length;

	//get info string length
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

	//Create a string
	char* log = new char[maxLength];

	//Get info log
	glGetShaderInfoLog(id, maxLength, &length, log);

	if (length)
	{
		ConsoleLogger::Error("GPU Compiler", log);
	}

	delete[] log;
}


Lemur::ShaderProgram::ShaderProgram(const char* vertexShader, 
	const char* fragmentShader) : 
	vertexShader(VERTEX, vertexShader),
	fragmentShader(FRAGMENT, fragmentShader)

{
	id = glCreateProgram();
	glAttachShader(id, this->vertexShader.getId());
	glAttachShader(id, this->fragmentShader.getId());
}

Lemur::ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(id);
}

Lemur::Shader* Lemur::ShaderProgram::getShader(ShaderType type)
{
	return type == VERTEX ? &vertexShader : &fragmentShader;
}

void Lemur::ShaderProgram::addDefine(const char* name, 
	const char* value, 
	ShaderType type)
{
	ShaderPreprocessor* prep = (type == VERTEX)
		? vertexShader.getPreprocessor()
		: fragmentShader.getPreprocessor();

	prep->addDefine(name, value);
}

void Lemur::ShaderProgram::addDefine(const char* name, ShaderType type)
{
	addDefine(name, "", type);
}

bool Lemur::ShaderProgram::compile()
{
	vertexShader.upload();
	fragmentShader.upload();

	bool shadersCompiled = true;
	
	shadersCompiled &= vertexShader.compile();
	shadersCompiled &= fragmentShader.compile();

	if (shadersCompiled && linkProgram())
		return true;

	return false;
}


bool Lemur::ShaderProgram::linkProgram() const
{
	glLinkProgram(id);

	//Check for errors
	int success;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (success != GL_TRUE)
	{
		ConsoleLogger::Error(CODE_LOCATION, cstr("Failed to link program with id ",
			id));
		return false;
	}

	ConsoleLogger::Debug(CODE_LOCATION, cstr("Successfully linked program with "
		"id ", id));
	return true;
}

int Lemur::ShaderProgram::getAttribLocation(const char* name) const
{
	if (!isUsing())
	{
		use();
		Lemur::ConsoleLogger::Warning(CODE_LOCATION, Lemur::cstr("Attempted to get attribute location from non-bound shader program. Program ", id, " was bound."));
	}

	int location = glGetAttribLocation(id, name);
	if (location == -1)
	{
		//Lemur::ConsoleLogger::Debug(CODE_LOCATION, CSTR2(name, " is not a valid GLSL program attribute!"));
	}

	return location;
}

int Lemur::ShaderProgram::getUniformLocation(const char* name) const
{
	if (!isUsing())
	{
		//TODO(bmanga): revert to previously active program afterwards
		use();
		Lemur::ConsoleLogger::Warning(CODE_LOCATION, Lemur::cstr("Attempted to get uniform location from non-bound shader program. Program ", id, " was bound."));
	}

	int location = glGetUniformLocation(id, name);
	
	if (location == -1)
	{
		//Lemur::ConsoleLogger::Debug(CODE_LOCATION, cstr(name, " is not a valid GLSL program uniform!"));
	}

	return location;
}

bool Lemur::ShaderProgram::isUsing() const
{
	return id == activeId;
}

void Lemur::ShaderProgram::use() const
{
	glUseProgram(id);
	activeId = id;
}

const std::string* Lemur::ShaderProgram::getParameters()
{
	return RESERVED;
}

int Lemur::operator ""_uniform(const char* name, std::size_t)
{
	GLint program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);

	return glGetUniformLocation(program, name);
}


unsigned int Lemur::ShaderProgram::activeId = 0;
