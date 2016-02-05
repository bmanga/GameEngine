#include "ShaderProgram_v2.h"
#include <GL/glew.h>
#include <fstream>
#include "types.h"

Lemur::ShaderProgram_v2::ShaderProgram_v2(const char* vertex_shader, 
	const char* fragment_shader) : 
	vert_id(glCreateShader(GL_VERTEX_SHADER)),
	frag_id(glCreateShader(glCreateShader(GL_FRAGMENT_SHADER)))
{
	compileShader(vert_id, vertex_shader);
	compileShader(frag_id, fragment_shader);
}

void Lemur::ShaderProgram_v2::compileShader(u32 id, const char* source)
{
	using namespace std;

	ifstream shader_code(DEFAULT_LOC + source);
	
	//get file size
	shader_code.seekg(0, ios::end);
	int length = shader_code.tellg();
	shader_code.seekg(0, ios::beg);

	char* buffer = new char[length];
	shader_code.read(buffer, length);
	const char* ptr = buffer;
	glShaderSource(id, 1, &ptr, nullptr);
	glCompileShader(id);

#if !defined(DEBUG)
	i32 success = false;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
#endif
}
