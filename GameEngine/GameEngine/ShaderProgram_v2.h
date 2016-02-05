#pragma once
#include "types.h"
#include <string>


namespace Lemur
{
	class ShaderProgram_v2
	{
	
	public:
		ShaderProgram_v2(const char* vertex_shader, const char* fragment_shader);
		~ShaderProgram_v2();

		u32 getVertID();
		u32 getFragID();
	
		void compile();
		void link();
		void unLink();

	private:
		void compilerShader(u32 type, const char* source);


	private:
		u32 vert_id;
		u32 frag_id;

		static string DEFAULT_LOC;
	};

	string ShaderProgram_v2::DEFAULT_LOC ("../assets/shaders");
}

