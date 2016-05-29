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
#include "ConsoleLogger.h"
#include "Lemur.h"

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

	//FrameBuffer id
	GLuint fbo_id;

	//RenderBuffer id
	GLuint color_rbo_id;
	GLuint depth_rbo_id;

public:
	explicit RenderSystem(Lemur::ecs::Manager<Lemur::MySettings>& manager):
		manager(manager)
	{
		glGenFramebuffers(1, &fbo_id);

		//create the renderbuffers
		glGenRenderbuffers(1, &color_rbo_id);
		glGenRenderbuffers(1, &depth_rbo_id);
		glBindRenderbuffer(GL_RENDERBUFFER, color_rbo_id);
		glRenderbufferStorage(GL_RENDER, GL_RGBA8, 800, 600);
		glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo_id);
		glRenderbufferStorage(GL_RENDER, GL_DEPTH_COMPONENT32, 800, 600);

		//setup framebuffer
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_id);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_RENDERBUFFER, color_rbo_id);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER, depth_rbo_id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		GLenum fboStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			std::string error;
			switch (fboStatus)
			{
			case GL_FRAMEBUFFER_UNDEFINED: 
				error = "framebuffer undefined"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				error = "framebuffer attachment is incomplete"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				error = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				error = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				error = " GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
			default:
				error = "unknown error";
			}
			Lemur::ConsoleLogger::Error(CODE_LOCATION,
				error.c_str());
		}
	}

	void setup();
	void render(Lemur::Camera camera);
};