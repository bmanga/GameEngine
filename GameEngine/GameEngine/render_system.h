#pragma once

#include "opengl_master.h"

#include "shader_program.h"
#include "texture.h"
#include "math.h"
#include "Camera.h"
#include "Mesh.h"
#include <memory>

#include "math.h"
#include "ShaderProgram_v2.h"

#include "Mesh.h"

namespace lm = Lemur::math;

enum BufferDrawType
{
	STATIC_DRAW = GL_STATIC_DRAW,
	DYNAMIC_DRAW = GL_DYNAMIC_DRAW
};

class VertexBufferObject
{
private:
	GLuint id;
	static GLuint bound_id;

public:
	VertexBufferObject()
	{
		glGenBuffers(1, &id);
	}

	void bufferData(int size, GLfloat* data, BufferDrawType type)
	{
		if (!isBound()) bind();
		glBufferData(GL_ARRAY_BUFFER, size, data, type);
	}

	bool isBound()
	{
		return id == bound_id;
	}

	void bind()
	{
		if (!isBound())
		{
			glBindBuffer(GL_ARRAY_BUFFER, id);
			bound_id = id;
		}
	}
};

class IndexBufferObject
{
private:
	GLuint id;
	static GLuint bound_id;

public:
	IndexBufferObject()
	{
		glGenBuffers(1, &id);
	}

	void bufferData(int size, GLuint* data, BufferDrawType type)
	{
		if (!isBound()) bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, type);
	}

	bool isBound()
	{
		return id == bound_id;
	}

	void bind()
	{
		if (!isBound())
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
			bound_id = id;
		}
	}
};

class RenderSystem
{
private:
	Mesh cube;
	Lemur::math::mat4 model;

	Lemur::ShaderProgram_v2 active_program;
	GLuint global_vao = 0;
	VertexBufferObject* global_vbo = nullptr;
	IndexBufferObject* global_ibo = nullptr;

	Texture* active_texture = nullptr;
	Texture* specular_texture = nullptr;

	Mesh* mesh = nullptr;
	VertexBufferObject* mesh_vbo = nullptr;
	IndexBufferObject* mesh_ibo = nullptr;
	VertexBufferObject* mesh_normal_bo = nullptr;

	lm::vec3 light_pos = lm::vec3(0.0f, 3.0f, 0.0f);

public:
	RenderSystem();
	~RenderSystem();

	bool initGL();
	void render(Lemur::Camera camera);

	void renderMesh(Lemur::Camera camera);
	void setMesh(Mesh* mesh);
};