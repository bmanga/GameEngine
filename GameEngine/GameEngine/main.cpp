#include "opengl_master.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <string>
#include <memory>
#include <iostream>
#include "Camera.h"
#include <typeinfo>
#include "render_system.h"
#include "ConsoleLogger.h"
#include "TaskExecutor.h"
#include "Importer.h"
#include "Lemur.h"
#include "Mesh_v2.h"
#include "linear_allocator.h"

#include "render_component.h"
#include "ecs\manager.h"
#include "ecs\entity.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

bool g_run = true;

bool init();
void handleKeys(unsigned char key, int x, int y);
void update();

SDL_Window* global_window = nullptr;
SDL_GLContext global_context;

ecs::Manager manager;
RenderSystem* renderer = new RenderSystem(manager);

Lemur::Camera g_camera;

Mesh mesh;
namespace lm = Lemur::math;
void handleMouse(int x, int y);

#define MEM_SIZE (1024 * 1024 * 1000) // 1 GB
LinearAllocator* linear_allocator = nullptr;

//#define MODEL_MODE

void run()
{
	while (g_run)
		printf("hello!\n");
}

int main(int argc, char* args[])
{
	todo::Mesh mq;
	mq.loadFromFile("testcube.lbm");
	using namespace std::string_literals;
	double x = 2.3;
	int y = 1;
	using tt = int;
	std::vector<tt> vi;

	ASSERT_CLERROR(false, CODE_LOCATION," error here");
	Lemur::ConsoleLogger::Error("mfde", "sup\n there is something \n extremely wrong\n");
	Lemur::ConsoleLogger::Warning("mgde", "sup");
	Lemur::ConsoleLogger::Info("mdswe", "suffsdap");
	Lemur::ConsoleLogger::Debug("msdfse", "ssdsup");
	Lemur::ConsoleLogger::Warning("mfste", "susdgfdsp");

	// vvv LINEAR ALLOCATOR vvv
	void* mem = malloc(MEM_SIZE);
	linear_allocator = new LinearAllocator(MEM_SIZE, mem);

	std::string* s = new(linear_allocator->allocate(sizeof(std::string), 4)) std::string();
	*s = "foo";

	std::string* s2 = new(linear_allocator->allocate(sizeof(std::string), 4)) std::string();
	*s2 = "bar";
	// ^^^ LINEAR ALLOCATOR ^^^

	std::vector<int> v2;
	v2.push_back(11);

	TaskExecutor ts;
	//auto m = ts.schedule(run);
	init();
	// Enable text input
	SDL_StartTextInput();

	//////////////////////////////////////
	manager.createComponentStore<RenderComponent>();
	manager.addSystem(ecs::System::ptr(renderer));

	RenderComponent component;
	component.mesh = new todo::Mesh();
	component.mesh->loadFromFile("monkeycompiled.lbm");
	component.program = new ShaderProgram("material_vertex.vert", "material_fragment.frag");
	component.texture = new Texture("..\\assets\\textures\\crate.png");

	ecs::Entity entity = manager.createEntity();
	manager.addComponent(entity, std::move(component));
	manager.registerEntity(entity);
	//////////////////////////////////////

	SDL_Event e;
	bool quit = false;
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			// User request quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			// Handle keypress with current mouse position
			else if (e.type == SDL_TEXTINPUT)
			{
				int x = 0;
				int y = 0;
				SDL_GetMouseState(&x, &y);
				handleKeys(e.text.text[0], x, y);
			}

			else if (e.type == SDL_MOUSEMOTION)
			{
				int x, y;
				SDL_GetMouseState(&x, &y);
				handleMouse(x, y);
			}
		}

		// Render quad
#ifdef MODEL_MODE
		renderer.renderMesh(g_camera);
#else
		//renderer.render(g_camera);
		//renderer.renderComponent(g_camera);
		
		renderer->updateCamera(g_camera);
		manager.updateEntities(0.0f);
#endif

		// Update screen
		SDL_GL_SwapWindow(global_window);
	}

	// Disable text input
	SDL_StopTextInput();

	return 0;
}

bool init()
{
	glewInit();
	bool success = true;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL failed to initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		// Use OpenGL 3.1 core
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		// Create window
		global_window = SDL_CreateWindow("GameEngine",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (!global_window)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			// Create context
			global_context = SDL_GL_CreateContext(global_window);
			if (!global_context)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				// Initialize GLEW
				glewExperimental = GL_TRUE;		// Necessary to access latest features
				GLenum glew_error = glewInit();
				if (glew_error != GLEW_OK)
				{
					printf("Error initializing GLEW: %s\n", glewGetErrorString(glew_error));
				}

				// Use VSync
				if (SDL_GL_SetSwapInterval(1) < 0)
				{
					printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
				}

				// Initialize OpenGL
				/*if (!renderer.initGL())
				{
					printf("Unable to initialize OpenGL!\n");
					success = false;
				}*/

#ifdef MODEL_MODE
				renderer.setMesh(&mesh);
#endif
			}
		}
	}

	return success;
}

void handleKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
	{
		g_camera.translateLocalZ(0.1f);
		break;
	}
	case 's':
	{
		g_camera.translateLocalZ(-0.1f);
		break;
	}
	case 'a':
	{
		g_camera.translateLocalX(-0.1f);
		break;
	}
	case 'd':
	{
		g_camera.translateLocalX(0.1f);
		break;
	}

	case 'r':
		g_camera.rotateLocalZ(3.14f);
		break;
	case 'f':
		g_camera.rotateLocalZ(-3.14f);
	}
}

void update()
{
	// No update per frame needed...
}

void handleMouse(int x, int y)
{
	static int prev_x = 400, prev_y = 300;

	auto dx = x - prev_x;
	auto dy = y - prev_y;

	prev_x = x;
	prev_y = y;

	g_camera.rotateRelativeX(-dy / 2.f);
	g_camera.rotateRelativeY(-dx / 2.f);

}
