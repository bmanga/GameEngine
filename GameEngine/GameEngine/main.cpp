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

#include "SceneManager.h"
#include "TextEngine.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

bool g_run = true;

bool init();
void handleKeys(unsigned char key, int x, int y);
void update();
void handleMouse(int x, int y);

SDL_Window* global_window = nullptr;
SDL_GLContext global_context;

Lemur::Camera g_camera;

namespace lm = Lemur::math;


int main(int argc, char* args[])
{
	init();
	
	Lemur::utils::RunningTimer t;
	Lemur::MeshManager mm;
	Lemur::TextureManager tm;
	Lemur::MyManager manager;
		
	Lemur::utils::FPSCounter fps;

	// Enable text input
	SDL_StartTextInput();
	// Hides the cursor
	SDL_ShowCursor(SDL_FALSE);

	ShaderProgram house_program("material_vertex.vert", "material_fragment.frag");
	ShaderProgram light_program("new_shader_vert.vert", "new_shader_frag.frag");
	//ShaderProgram house_program("new_shader_vert.vert", "new_shader_frag.frag");
	//house_program.addDefine("USE_TEXTURES", FRAGMENT);

	house_program.compile();
	//program.addDefine("NUM_DIR_LIGHTS", "1", FRAGMENT);
	light_program.addDefine("NUM_POINT_LIGHTS", "1", FRAGMENT);
	light_program.addDefine("NUM_SPOT_LIGHTS", "1", FRAGMENT);
	light_program.compile();


	Lemur::ecs::EntityIndex house(manager.createIndex());

	auto& house_position = manager.addComponent<CPosition>(house);
	house_position = { 0, 0, 0 };

	auto& house_renderable = manager.addComponent<CRenderable>(house);
	house_renderable.mesh = mm.load("Farmhouse.lbm");
	auto& mat = house_renderable.material = std::make_shared<Lemur::Material>();
	mat->texture = tm.load("Farmhouse.jpg");
	house_renderable.material->shader = std::make_shared<ShaderProgram>(house_program);

	mat->use_texturing = true;

	/////////////////////////////////// ROW OF 10 MODELS /////////////////////////////////
	for (unsigned int i = 1; i < 10; i++)
	{
		Lemur::ecs::EntityIndex model(manager.createIndex());

		auto& model_position(manager.addComponent<CPosition>(model));
		model_position.x = i * 5.0f;
		model_position.y = 0.0f;
		model_position.z = -6.0f;

		auto& renderable(manager.addComponent<CRenderable>(model));
		renderable.mesh = mm.load("cube.lbm");
		renderable.material = std::make_shared<Lemur::Material>();
		renderable.material->shader = std::make_shared<ShaderProgram>(light_program);
		renderable.material->texture = tm.load("crate.png");
		renderable.material->ambient[0] = 0.24725f;
		renderable.material->ambient[1] = 0.1995f;
		renderable.material->ambient[2] = 0.0745f;
		renderable.material->diffuse[0] = 0.75164f;
		renderable.material->diffuse[1] = 0.60648f;
		renderable.material->diffuse[2] = 0.22648f;
		renderable.material->specular[0] = 0.628281f;
		renderable.material->specular[1] = 0.555802f;
		renderable.material->specular[2] = 0.366065f;
		renderable.material->shininess = 0.4f;
	}

	///////////////////////////////// POINT LIGHT /////////////////////////////////
	Lemur::ecs::EntityIndex point_light(manager.createIndex());

	auto& plight_position(manager.addComponent<CPosition>(point_light));
	plight_position.x = -3.0f;
	plight_position.y = -3.0f;
	plight_position.z = 3.0f;

	auto& light(manager.addComponent<CPointLight>(point_light));
	light.ambient.r = 1.0f;
	light.ambient.g = 1.0f;
	light.ambient.b = 1.0f;
	light.diffuse.r = 1.0f;
	light.diffuse.g = 1.0f;
	light.diffuse.b = 1.0f;
	light.specular.r = 1.0f;
	light.specular.g = 1.0f;
	light.specular.b = 1.0f;
	light.constant = 1.0f;
	light.linear = 0.09f;
	light.quadratic = 0.032f;

	///////////////////////////////// POINT LIGHT RENDERABLE /////////////////////////////////
	Lemur::ecs::EntityIndex light_renderable(manager.createIndex());

	auto& lr_position = manager.addComponent<CPosition>(light_renderable);
	lr_position = { -3, -3, 3 };

	auto& lr_renderable(manager.addComponent<CRenderable>(light_renderable));
	lr_renderable.mesh = mm.load("cube.lbm");
	lr_renderable.material = std::make_shared<Lemur::Material>();
	lr_renderable.material->shader = std::make_shared<ShaderProgram>(light_program);
	lr_renderable.material->emissive[0] = 1.0f;
	lr_renderable.material->emissive[1] = 1.0f;
	lr_renderable.material->emissive[2] = 1.0f;
	lr_renderable.material->shininess = 64.0f;

	///////////////////////////////// SPOT LIGHT /////////////////////////////////
	Lemur::ecs::EntityIndex spot_light(manager.createIndex());

	auto& sl_position(manager.addComponent<CPosition>(spot_light));
	sl_position.x = g_camera.getCenter().x;
	sl_position.y = g_camera.getCenter().y;
	sl_position.z = g_camera.getCenter().z;

	auto& sl_light(manager.addComponent<CSpotLight>(spot_light));
	sl_light.direction = g_camera.getDirection();
	sl_light.cut_off = glm::cos(glm::radians(12.5f)); // Must be cosine value - prevents expensive arccos calculation in fragment shader.
	sl_light.outer_cut_off = lm::cos(lm::radians(15.0f)); // Must be cosine value - prevents expensive arccos calculation in fragment shader.
	sl_light.constant = 1.0f;
	sl_light.linear = 0.09f;
	sl_light.quadratic = 0.032f;
	sl_light.ambient = glm::vec3(0.0f);
	sl_light.diffuse = glm::vec3(1.0f, 0.0f, 1.0f);
	sl_light.specular = glm::vec3(1.0f, 0.0f, 1.0f);

	manager.refresh();

	SceneManager scene_manager;
	scene_manager.root.updateAll();

	RenderSystem render_system(manager);



	fps.start();

	unsigned cnt = 0;
	
	t.start();

	SDL_Event e;
	bool quit = false;

	Lemur::TextEngine textEngine;

	if (!textEngine.loadFont("xan22", 12, "Consolas.ttf"))
	{
		Lemur::ConsoleLogger::Error(CODE_LOCATION, "could not load font");
	}

	while (!quit)
	{

		fps.tick();

		
	
		
		if (t.elapsed_ms() > 300)
		 {
			 static bool maybe = true;
			 Lemur::Pen pen{ 00, 00, {0.0f, 1.0f, 0.0f, 1.0f}, "xan22" };
			fps.update();
			textEngine.clear();

			textEngine.addText(Lemur::str("fps: ",fps.fps()), pen);
			t.reset();
		}
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
				quit = true;
			}

			// User request quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			// Handle keypress with current mouse position
			else if (e.type == SDL_TEXTINPUT)
			{
				if(e.text.text == "Escape")
				{
					quit = true;
					break;
				}
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

		//textEngine.display(glm::mat4(), glm::mat4());
		textEngine.display();

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
		g_camera.translateLocalZ(0.3f);
		break;
	}
	case 's':
	{
		g_camera.translateLocalZ(-0.3f);
		break;
	}
	case 'a':
	{
		g_camera.translateLocalX(-0.3f);
		break;
	}
	case 'd':
	{
		g_camera.translateLocalX(0.3f);
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