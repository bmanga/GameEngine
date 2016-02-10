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

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

bool g_run = true;

bool init();
void handleKeys(unsigned char key, int x, int y);
void update();

SDL_Window* global_window = nullptr;
SDL_GLContext global_context;

RenderSystem renderer;

Lemur::Camera g_camera;

Mesh mesh;
namespace lm = Lemur::math;
void handleMouse(int x, int y);

#define MODEL_MODE

void run()
{
	while (g_run)
		printf("hello!\n");
}

int main(int argc, char* args[])
{
	using namespace std::string_literals;
	double x = 2.3;
	int y = 1;
	using tt = int;
	std::vector<tt> vi;
	mesh.setMeshData(load_obj("monkeyhard.objm"));
	Lemur::ConsoleLogger::Error("mfde", "sup\n there is something \n extremely wrong\n");
	Lemur::ConsoleLogger::Warning("mgde", "sup");
	Lemur::ConsoleLogger::Info("mdswe", "suffsdap");
	Lemur::ConsoleLogger::Debug("msdfse", "ssdsup");
	Lemur::ConsoleLogger::Warning("mfste", "susdgfdsp");

	TaskExecutor ts;
	//auto m = ts.schedule(run);
	init();
	// Enable text input
	SDL_StartTextInput();

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
		renderer.render(g_camera);
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
				if (!renderer.initGL())
				{
					printf("Unable to initialize OpenGL!\n");
					success = false;
				}

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
