#include "opengl_master.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <string>
#include <memory>

#include "shader_program.h"
#include "texture.h"
#include "math.h"
#include "Camera.h"

#include "TaskExecutor.h"
#include "Importer.h"
#include "Mesh.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

bool g_run = true;
Mesh mesh;
bool init();
bool initGL();
void handleKeys(unsigned char key, int x, int y);
void update();
void render();
//void close();

SDL_Window* global_window = nullptr;
SDL_GLContext global_context;

// Graphics program
ShaderProgram* global_program = nullptr;
GLuint vertices_bo = 0;
GLuint indices_bo = 0;

Texture* global_texture = nullptr;

GLfloat global_camera_x = 0.0f, global_camera_y = 0.0f;

Lemur::math::mat4 model;

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
	
	TaskExecutor ts;
	//auto m = ts.schedule(run);
	//m.then([]() {std::cout << "bye"; });

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
		render();

		// Update screen
		SDL_GL_SwapWindow(global_window);
	}

	// Disable text input
	SDL_StopTextInput();
	g_run = false;
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
				if (!initGL())
				{
					printf("Unable to initialize OpenGL!\n");
					success = false;
				}
			}
		}
	}

	return success;
}

bool initGL()
{
	using namespace std;
	
	mesh.setMeshData(load_obj(R"(C:\Users\Bruno\Desktop\monkey.obj)"));
	
	bool success = true;

	std::unique_ptr<Shader> vertex_shader(new Shader(GL_VERTEX_SHADER,
		"#version 140\n"
		"in vec3 position;\n"
		"in vec3 in_color;\n"
		"in vec2 in_texcoord;\n"
		"out vec3 color;\n"
		"out vec2 texcoord;\n"
		//"uniform mat4 model;\n"
		//"uniform mat4 view;\n"
		//"uniform mat4 proj;\n"
		"uniform mat4 matrix;\n"
		"void main() {\n"
		"	color = in_color;\n"
		"	texcoord = in_texcoord;\n"
		//"	gl_Position = proj * view * model * vec4(position, 1);\n"
		"	gl_Position = matrix * vec4(position, 1);\n"
		"}"
		));
	std::unique_ptr<Shader> fragment_shader(new Shader(GL_FRAGMENT_SHADER,
		"#version 140\n"
		"in vec3 color;\n"
		"in vec2 texcoord;\n"
		"out vec4 out_color;\n"
		"uniform sampler2D tex;\n"
		"void main() {\n"
		"	out_color = /*texture(tex, texcoord) */ vec4(1, 1, 1, 1.0);\n"
		"}"
		));

	global_program = new ShaderProgram(*vertex_shader, *fragment_shader);

	// Initialize clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);


	// Create VBO
	glGenBuffers(1, &vertices_bo);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_bo);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertexBufferSize(), mesh.vertexBuffer(), GL_STATIC_DRAW);

	// Create IBO
	glGenBuffers(1, &indices_bo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_bo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.vertexIndexBufferSize(), mesh.vertexIndexBuffer(), GL_STATIC_DRAW);

	// Sorry about the hard-coded path :(
	global_texture = new Texture("C:\\Users\\Bruno\\Desktop\\crate.bmp");
	global_texture->bind();
	global_texture->setWrapType(CLAMP_TO_EDGE, CLAMP_TO_EDGE);
	global_texture->setInterpolation(LINEAR, LINEAR);

	return success;
}

void handleKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
	{
		g_camera.translateLocalZ(0.1f);
		global_camera_y -= 16.0f;
		break;
	}
	case 's':
	{
		g_camera.translateLocalZ(-0.1f);
		global_camera_y += 16.0f;
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

void render()
{
	// Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind program
	global_program->use();


	model = lm::rotate(model, lm::radians(1.0f), lm::vec3(0.0f, 0.0f, 1.0f));


	lm::mat4 matrix = g_camera.getViewProjection() * model;
	int matrix_uniform = global_program->getUniformLocation("matrix");
	glUniformMatrix4fv(matrix_uniform, 1, GL_FALSE, lm::value_ptr(matrix));

	// Set vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertices_bo);

	// Enable vertex position
	int pos_attrib = global_program->getAttribLocation("position");
	glEnableVertexAttribArray(pos_attrib);
	glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	int col_attrib = global_program->getAttribLocation("in_color");
	glEnableVertexAttribArray(col_attrib);
	glVertexAttribPointer(col_attrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	int tex_attrib = global_program->getAttribLocation("in_texcoord");
	glEnableVertexAttribArray(tex_attrib);
	glVertexAttribPointer(tex_attrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	glEnable(GL_VERTEX_ARRAY);
	// Set index data and render
	glBindBuffer(GL_ARRAY_BUFFER, vertices_bo);
	glDrawElements(GL_POINTS, mesh.vertexIndexCount(), GL_UNSIGNED_INT, mesh.vertexIndexBuffer());
	//glDrawArrays(GL_TRIANGLES, 0, std::get<3>(model_data).size());

	// Disable vertex position
            	glDisableVertexAttribArray(pos_attrib);
	glDisableVertexAttribArray(col_attrib);
	glDisableVertexAttribArray(tex_attrib);

	// Unbind program
	glUseProgram(NULL);
}

void handleMouse(int x, int y)
{
	static int prev_x = 400, prev_y = 300;

	auto dx = x - prev_x;
	auto dy = y - prev_y;

	prev_x = x;
	prev_y = y;

	g_camera.rotateRelativeX(-dy / 10.f);
	g_camera.rotateRelativeY(-dx / 10.f);

}
