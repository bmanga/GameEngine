#pragma once

#include "OpenGLMaster.h"

#include "ShaderProgram.h"
#include "Texture.h"
#include "Math.h"
#include "Camera.h"
#include "Mesh.h"
#include <memory>

#include "ECS.h"

#include "RenderComponent.h"
#include "PointLightComponent.h"
#include "PositionComponent.h"

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
	using SystemManager = Lemur::MyManager;
	SystemManager manager;

	Lemur::math::mat4 model;

public:
	RenderSystem(Lemur::ecs::Manager<Lemur::MySettings>& manager)
	{
		this->manager = manager;
	}

	void setup();
	void render(Lemur::Camera camera);
};