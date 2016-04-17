#include "OpenGLMaster.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <string>
#include <memory>
#include "Camera.h"
#include "ConsoleLogger.h"
#include "TaskExecutor.h"
#include "Lemur.h"
#include "Mesh.h"
#include "Manager.h"
#include "LinearAllocator.h"

#include "RenderSystem.h"
#include "ECS.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

bool g_run = true;

bool init();
void handleKeys(unsigned char key, int x, int y);
void update();

SDL_Window* global_window = nullptr;
SDL_GLContext global_context;

Lemur::Camera g_camera;

namespace lm = Lemur::math;
void handleMouse(int x, int y);

#define MEM_SIZE (1024 * 1024 * 1000) // 1 GB
LinearAllocator* linear_allocator = nullptr;

void run()
{
	while (g_run)
		printf("hello!\n");
}

int main(int argc, char* args[])
{
	Lemur::MeshManager mm;
	Lemur::TextureManager tm;
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

	// Hides the cursor
	SDL_ShowCursor(SDL_FALSE);

	Lemur::MyManager manager;

	// Ensures ECS Manager's underlying Settings struct works correctly
	static Lemur::MySettings settings;
	std::cout << "Components: " << settings.componentCount() << std::endl;
	std::cout << "Tags: " << settings.tagCount() << std::endl;
	std::cout << "Signatures: " << settings.signatureCount() << std::endl;

	Lemur::ecs::EntityIndex model(manager.createIndex());
	auto& model_position(manager.addComponent<CPosition>(model));
	model_position.x = 0.0f;
	model_position.y = 0.0f;
	model_position.z = 0.0f;
	auto& renderable(manager.addComponent<CRenderable>(model));
	renderable.mesh = mm.load("model.lbm");
	renderable.program = new ShaderProgram("material_vertex.vert", "material_fragment.frag");
	renderable.texture = tm.load("crate.png");

	Lemur::ecs::EntityIndex point_light(manager.createIndex());
	auto& light_position(manager.addComponent<CPosition>(point_light));
	light_position.x = 0.0f;
	light_position.y = 0.0f;
	light_position.z = 0.0f;
	auto& light(manager.addComponent<CLight>(point_light));
	light.ambient.r = 1.0f;
	light.ambient.g = 1.0f;
	light.ambient.b = 1.0f;
	light.diffuse.r = 1.0f;
	light.diffuse.g = 1.0f;
	light.diffuse.b = 1.0f;
	light.specular.r = 1.0f;
	light.specular.g = 1.0f;
	light.specular.b = 1.0f;

	manager.refresh();

	RenderSystem render_system(manager);

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

				SDL_WarpMouseInWindow(global_window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
			}
		}

		// Render quad
#ifdef ECS_TEST
		render_system.render(g_camera);
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
	int dx = x - (SCREEN_WIDTH / 2);
	int dy = y - (SCREEN_HEIGHT / 2);

	g_camera.rotateRelativeX(-dy / 2.f);
	g_camera.rotateRelativeY(-dx / 2.f);
}
