#include "ShaderProgram_v2.h"
#include <GL/glew.h>
#include <fstream>

Lemur::ShaderProgram_v2::ShaderProgram_v2(const char* vertex_shader, 
	const char* fragment_shader) : 
	vert_id(glCreateShader(GL_VERTEX_SHADER)),
	frag_id(glCreateShader(glCreateShader(GL_FRAGMENT_SHADER)))
{
	compilerShader(GL_VERTEX_SHADER, vertex_shader);
	compilerShader(GL_FRAGMENT_SHADER, fragment_shader);
}

void Lemur::ShaderProgram_v2::compilerShader(u32 type, const char* source)
{
	using namespace std;

	ifstream shader_code(std::string(DEFAULT_LOC) + source);
	
}
