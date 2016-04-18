#include "OpenGLMaster.h"

#include <SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Camera.h"
#include "Manager.h"

#include "LinearAllocator.h"
#include "FPSCounter.h"
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

void run()
{
	while (g_run)
		printf("hello!\n");
}

int main(int argc, char* args[])
{
	Lemur::MeshManager mm;
	Lemur::TextureManager tm;
	Lemur::MyManager manager;
		
	init();
	
	Lemur::utils::FPSCounter fps;

	// Enable text input
	SDL_StartTextInput();

	// Hides the cursor
	SDL_ShowCursor(SDL_FALSE);


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
	fps.start();
	while (!quit)
	{
		fps.tick();

		static int cnt = 0;
		++cnt;
		
			

		if (cnt > fps.fps() + 1) {
			fps.update();
			printf("%f\n", fps.fps());
			cnt = 0;
		}
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

		render_system.render(g_camera);

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
