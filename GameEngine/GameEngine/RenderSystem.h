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
#include "LightComponent.h"
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

/*
class RenderSystem
{
private:
	Lemur::math::mat4 model;

	ShaderProgram* active_program = nullptr;
	GLuint global_vao = 0;
	VertexBufferObject* global_vbo = nullptr;
	IndexBufferObject* global_ibo = nullptr;

	Texture* active_texture = nullptr;
	Texture* specular_texture = nullptr;

	Mesh* mesh = nullptr;
	VertexBufferObject* mesh_vbo = nullptr;
	IndexBufferObject* mesh_ibo = nullptr;
	VertexBufferObject* mesh_normal_bo = nullptr;

	RenderComponent* component = nullptr;

public:
	RenderSystem();
	~RenderSystem();

	bool initGL();
	void render(Lemur::Camera camera);

	void renderMesh(Lemur::Camera camera);
	void setMesh(Mesh* mesh);

	void renderComponent(Lemur::Camera camera);
};
*/

/*
class RenderSystem : public ecs::System
{
private:
	Lemur::math::mat4 model;
	Lemur::Camera camera;

	VertexBufferObject* component_vbo = nullptr;
	IndexBufferObject* component_ibo = nullptr;
	VertexBufferObject* component_nbo = nullptr;

public:
	RenderSystem(ecs::Manager& manager) :
		ecs::System(manager)
	{
		// TODO: Maybe do away with required components and assign required components
		// through AND/OR relationship functions
		ecs::ComponentTypeSet required_components;
		required_components.insert(RenderComponent::type);
		required_components.insert(LightComponent::type);
		required_components.insert(PositionComponent::type);
		setRequiredComponents(std::move(required_components));

		ecs::ComponentTypeSet or_components;
		or_components.insert(RenderComponent::type);
		or_components.insert(LightComponent::type);
		or_components.insert(PositionComponent::type);
		setOr(std::move(or_components));
	}

	void updateCamera(Lemur::Camera camera)
	{
		this->camera = camera;
	}

	void updateEntity(float elapsed_time, ecs::Entity entity) override;

	void end() override;
};
*/

class RenderSystem
{
private:
	using SystemManager = Lemur::MyManager;
	SystemManager manager;

	Lemur::math::mat4 model;
	Lemur::Camera camera;

public:
	RenderSystem(Lemur::ecs::Manager<Lemur::MySettings>& manager)
	{
		this->manager = manager;
	}

	void render(Lemur::Camera camera);
};