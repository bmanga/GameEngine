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

void Lemur::ShaderPreprocessor::addDefine(const char* name, const char* value)
{
	auto define = str("#define ", name, " ", value, "\n");
	defines += define;
}

void Lemur::ShaderPreprocessor::process(std::string& source) const
{
	//Add defines
	if (defines.size())
	{
		auto definePos = source.find("#version");
		if (definePos == source.npos)
		{
			definePos = 0;
		}
		else
		{
			definePos = source.find_first_of('\n', definePos) + 1;
		}

		source.insert(definePos, defines);
	}
}

Lemur::Shader::Shader(ShaderType type, fs::path shader): type(type), path(shader)
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

void Lemur::Shader::upload(bool preprocess)
{
	loadSource();

	if (preprocess)
		preprocessor.process(source);

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
		                                  cstr("Unable to compile shader ", path.filename()));

		printInfoLog();
		return false;
	}

	ConsoleLogger::Debug(CODE_LOCATION, cstr("Shader ", path.filename(),
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

bool Lemur::Shader::loadSource()
{
	using namespace std;

	ifstream shader(path, ios::in, ios::binary);

	if (!shader)
	{
		ConsoleLogger::Error(CODE_LOCATION, cstr("Shader ", path.filename(),
		                                         " not found"));
		return false;
	}

	// Get file size
	shader.seekg(0, ios::end);
	int32_t length = static_cast<uint32_t>(shader.tellg());
	shader.seekg(0, ios::beg);

	//read in the source into the string
	source.clear();
	source.resize(length + 1);
	shader.read(&source[0], length);
	shader.close();

	return true;
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
