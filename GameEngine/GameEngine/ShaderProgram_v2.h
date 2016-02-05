#pragma once
#include "types.h"
#include <string>


namespace Lemur
{
	class ShaderProgram_v2
	{
	
	public:
		ShaderProgram_v2(const char* vertex_shader, const char* fragment_shader);
		~ShaderProgram_v2() = default;

		ShaderProgram_v2();

		void compileShaders(const char* vertex_shader, 
			const char* fragment_shader);

		u32 getVertID() const;

		u32 getFragID() const;

		i32 getAttribLocation(const char* name);
		i32 getUniformLocation(const char* name);

		bool isActive();
		void use();
		bool compileProgram();
		void link() const;

		void printLog();
		//void unLink() const;

	private:
		static void compileShader(u32 id, const char* source);
		static void printLog(u32 id);

	private:
		u32 vert_id = 0;
		u32 frag_id = 0;

		u32 program_id = 0;

		static string DEFAULT_LOC;
	};

	
}

