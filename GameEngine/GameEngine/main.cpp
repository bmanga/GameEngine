#include "opengl_master.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <string>
#include <memory>

#include "shader_program.h"
#include "texture.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

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
GLuint global_vbo = 0;
GLuint global_ibo = 0;

Texture* global_texture = nullptr;

GLfloat global_camera_x = 0.0f, global_camera_y = 0.0f;

glm::mat4 model;

int main(int argc, char* args[])
{
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
		}

		// Render quad
		render();

		// Update screen
		SDL_GL_SwapWindow(global_window);
	}

	// Disable text input
	SDL_StopTextInput();

	return 0;
}

bool init()
{
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
	bool success = true;

	std::unique_ptr<Shader> vertex_shader(new Shader(GL_VERTEX_SHADER,
		"#version 140\n"
		"in vec3 position;\n"
		"in vec3 in_color;\n"
		"in vec2 in_texcoord;\n"
		"out vec3 color;\n"
		"out vec2 texcoord;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 proj;\n"
		"void main() {\n"
		"	color = in_color;\n"
		"	texcoord = in_texcoord;\n"
		"	gl_Position = proj * view * model * vec4(position, 1);\n"
		"}"
		));
	std::unique_ptr<Shader> fragment_shader(new Shader(GL_FRAGMENT_SHADER,
		"#version 140\n"
		"in vec3 color;\n"
		"in vec2 texcoord;\n"
		"out vec4 out_color;\n"
		"uniform sampler2D tex;\n"
		"void main() {\n"
		"	out_color = texture(tex, texcoord) * vec4(color, 1.0);\n"
		"}"
		));

	global_program = new ShaderProgram(*vertex_shader, *fragment_shader);

	// Initialize clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// VBO data
	GLfloat vertex_data[] = {
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};

	// IBO data
	GLuint index_data[] = { 0, 1, 2, 2, 3, 0 };

	// Create VBO
	glGenBuffers(1, &global_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, global_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

	// Create IBO
	glGenBuffers(1, &global_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);

	// Sorry about the hard-coded path :(
	global_texture = new Texture("C:\\Users\\kavan\\Documents\\Visual Studio 2015\\Projects\\fiddling_about\\Debug\\crate.bmp");
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
		global_camera_y -= 16.0f;
		break;
	}
	case 's':
	{
		global_camera_y += 16.0f;
		break;
	}
	case 'a':
	{
		global_camera_x -= 16.0f;
		break;
	}
	case 'd':
	{
		global_camera_x += 16.0f;
		break;
	}
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

	// Create the view transformation
	glm::mat4 view = glm::lookAt(
		glm::vec3(1.5f, 1.5f, 1.5f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
		);
	GLint view_uniform = global_program->getUniformLocation("view");
	glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));

	// Create the perspective projection matrix
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 10.0f);
	GLint proj_uniform = global_program->getUniformLocation("proj");
	glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, glm::value_ptr(proj));

	// Apply the model transformation
	model = glm::rotate(model, glm::radians(2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	int model_uniform = global_program->getUniformLocation("model");
	glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

	// Set vertex data
	glBindBuffer(GL_ARRAY_BUFFER, global_vbo);

	// Enable vertex position
	int pos_attrib = global_program->getAttribLocation("position");
	glEnableVertexAttribArray(pos_attrib);
	glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);

	int col_attrib = global_program->getAttribLocation("in_color");
	glEnableVertexAttribArray(col_attrib);
	glVertexAttribPointer(col_attrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	int tex_attrib = global_program->getAttribLocation("in_texcoord");
	glEnableVertexAttribArray(tex_attrib);
	glVertexAttribPointer(tex_attrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	// Set index data and render
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global_ibo);
	//glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, NULL);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Disable vertex position
	glDisableVertexAttribArray(pos_attrib);
	glDisableVertexAttribArray(col_attrib);
	glDisableVertexAttribArray(tex_attrib);

	// Unbind program
	glUseProgram(NULL);
}