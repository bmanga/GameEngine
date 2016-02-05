#pragma once

template <class RenderEngine>
class RenderSystem
{
public:
	using Engine = RenderEngine;

	void Render (struct Mesh* mesh)
	{
		Engine::render(mesh);
	}
	//RenderSystem()
	//~RenderSystem();
};


