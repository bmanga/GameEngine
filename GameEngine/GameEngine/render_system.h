#pragma once

#include "opengl_master.h"

#include "shader_program.h"
#include "texture.h"
#include "math.h"
#include "Camera.h"
#include <memory>

#include "math.h"
#include "ShaderProgram_v2.h"

namespace lm = Lemur::math;

class RenderSystem
{
private:
	Lemur::math::mat4 model;

	Lemur::ShaderProgram_v2 active_program;
	GLuint global_vao = 0;
	GLuint global_vbo = 0;
	GLuint global_ibo = 0;

	Texture* active_texture = nullptr;
	Texture* specular_texture = nullptr;

	lm::vec3 light_pos = lm::vec3(0.0f, 3.0f, 0.0f);

public:
	RenderSystem();
	~RenderSystem();

	bool initGL();
	void render(Lemur::Camera camera);
};