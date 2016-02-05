#pragma once

template <class RenderEngine>
class RenderSystem
{
public:
	using Engine = RenderEngine;

	void Render (Mesh* mesh)
	{
		Engine::render(mesh);
	}
	//RenderSystem()
	//~RenderSystem();
};


