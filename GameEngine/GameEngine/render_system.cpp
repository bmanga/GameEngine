#include "render_system.h"

RenderSystem::RenderSystem()
{
	
}

RenderSystem::~RenderSystem()
{

}

bool RenderSystem::initGL()
{
	bool success = true;

	std::unique_ptr<Shader> vertex_shader(new Shader(GL_VERTEX_SHADER,
		"#version 140\n"
		"in vec3 position;\n"
		"in vec3 in_color;\n"
		"in vec2 in_texcoord;\n"
		"in vec3 in_normal;\n"

		"out vec3 color;\n"
		"out vec2 texcoord;\n"
		"out vec3 normal;\n"

		"out vec3 frag_pos;\n"

		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 proj;\n"

		"void main() {\n"

		"	color = in_color;\n"
		"	texcoord = in_texcoord;\n"
		"	normal = mat3(transpose(inverse(model))) * in_normal;\n"

		"	frag_pos = vec3(model * vec4(position, 1.0f));\n"
		"	gl_Position = proj * view * model * vec4(position, 1.0f);\n"
		"}"
		));
	std::unique_ptr<Shader> fragment_shader(new Shader(GL_FRAGMENT_SHADER,
		"#version 140\n"
		"struct Material {\n"
		"	vec3 ambient;\n"
		"	vec3 diffuse;\n"
		"	vec3 specular;\n"
		"	float shininess;\n"
		"};\n"

		"struct Light {\n"
		"	vec3 position;\n"
		"	vec3 ambient;\n"
		"	vec3 diffuse;\n"
		"	vec3 specular;\n"
		"};\n"

		"in vec3 color;\n"
		"in vec2 texcoord;\n"
		"in vec3 normal;\n"
		"in vec3 frag_pos;\n"

		"out vec4 out_color;\n"

		"uniform sampler2D tex;\n"
		"uniform vec3 view_pos;\n"
		"uniform Material material;\n"
		"uniform Light light;\n"

		"void main() {\n"

		"	vec3 ambient = light.ambient * material.ambient;\n"

		"	vec3 norm = normalize(normal);\n"
		"	vec3 light_dir = normalize(light.position - frag_pos);\n"
		"	float diff = max(dot(norm, light_dir), 0.0);\n"
		"	vec3 diffuse = light.diffuse * (diff * material.diffuse);\n"

		"	vec3 view_dir = normalize(view_pos - frag_pos);\n"
		"	vec3 reflect_dir = reflect(-light_dir, norm);\n"
		"	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);\n"
		"	vec3 specular = light.specular * (spec * material.specular);\n"

		"	vec3 result = (ambient + diffuse + specular) * color;\n"
		"	out_color = texture(tex, texcoord) * vec4(result, 1.0f);\n"
		"}"
		));

	active_program = new ShaderProgram(*vertex_shader, *fragment_shader);

	// Initialize clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// VBO data
	GLfloat vertex_data[] = {
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,  0.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  0.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,  0.0f
	};

	// IBO data
	GLuint index_data[] = { 0, 1, 2, 2, 3, 0 };

	// Create VAO
	glGenVertexArrays(1, &global_vao);
	glBindVertexArray(global_vao);

	// Create VBO
	glGenBuffers(1, &global_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, global_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

	// Create IBO
	glGenBuffers(1, &global_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);

	// Sorry about the hard-coded path :(
	active_texture = new Texture("C:\\Users\\kavan\\Desktop\\crate.bmp");
	active_texture->bind();
	active_texture->setWrapType(CLAMP_TO_EDGE, CLAMP_TO_EDGE);
	active_texture->setInterpolation(LINEAR, LINEAR);

	return success;
}

void RenderSystem::render(Lemur::Camera camera)
{
	// Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind program
	active_program->use();

	// Create the view transformation
	//lm::mat4 view = lm::lookAt(
	//	lm::vec3(1.5f, 1.5f, 1.5f),
	//	lm::vec3(0.0f, 0.0f, 0.0f),
	//	lm::vec3(0.0f, 0.0f, 1.0f)
	//	);

	lm::mat4 view = camera.getView();
	GLint view_uniform = active_program->getUniformLocation("view");
	glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));

	// Create the perspective projection matrix
	lm::mat4 proj = camera.getProjection();
	//lm::mat4 proj = lm::perspective(lm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 10.0f);
	GLint proj_uniform = active_program->getUniformLocation("proj");
	glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, glm::value_ptr(proj));

	// Apply the model transformation
	model = lm::rotate(model, lm::radians(0.25f), lm::vec3(0.0f, 0.0f, 1.0f));
	int model_uniform = active_program->getUniformLocation("model");
	glUniformMatrix4fv(model_uniform, 1, GL_FALSE, lm::value_ptr(model));

	int view_pos_uniform = active_program->getUniformLocation("view_pos");
	glUniform3f(view_pos_uniform, camera.getCenter().x, camera.getCenter().y, camera.getCenter().z);

	int mat_ambient_uniform = active_program->getUniformLocation("material.ambient");
	int mat_diffuse_uniform = active_program->getUniformLocation("material.diffuse");
	int mat_specular_uniform = active_program->getUniformLocation("material.specular");
	int mat_shininess_uniform = active_program->getUniformLocation("material.shininess");

	glUniform3f(mat_ambient_uniform, 1.0f, 1.0f, 1.0f);
	glUniform3f(mat_diffuse_uniform, 1.0f, 1.0f, 1.0f);
	glUniform3f(mat_specular_uniform, 0.5f, 0.5f, 0.5f);
	glUniform1f(mat_shininess_uniform, 32.0f);

	int light_pos_uniform = active_program->getUniformLocation("light.position");
	int light_ambient_uniform = active_program->getUniformLocation("light.ambient");
	int light_diffuse_uniform = active_program->getUniformLocation("light.diffuse");
	int light_specular_uniform = active_program->getUniformLocation("light.specular");

	glUniform3f(light_pos_uniform, light_pos.x, light_pos.y, light_pos.z);
	glUniform3f(light_ambient_uniform, 0.2f, 0.2f, 0.2f);
	glUniform3f(light_diffuse_uniform, 0.5f, 0.5f, 0.5f);
	glUniform3f(light_specular_uniform, 1.0f, 1.0f, 1.0f);

	// Set vertex data
	glBindBuffer(GL_ARRAY_BUFFER, global_vbo);

	// Enable vertex position
	int pos_attrib = active_program->getAttribLocation("position");
	glEnableVertexAttribArray(pos_attrib);
	glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 0);

	int col_attrib = active_program->getAttribLocation("in_color");
	glEnableVertexAttribArray(col_attrib);
	glVertexAttribPointer(col_attrib, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	int tex_attrib = active_program->getAttribLocation("in_texcoord");
	glEnableVertexAttribArray(tex_attrib);
	glVertexAttribPointer(tex_attrib, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	int norm_attrib = active_program->getAttribLocation("in_normal");
	glEnableVertexAttribArray(norm_attrib);
	glVertexAttribPointer(norm_attrib, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(8 * sizeof(GLfloat)));

	// Set index data and render
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global_ibo);
	//glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, NULL);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Disable vertex position
	glDisableVertexAttribArray(pos_attrib);
	glDisableVertexAttribArray(col_attrib);
	glDisableVertexAttribArray(tex_attrib);
	glDisableVertexAttribArray(norm_attrib);

	// Unbind program
	glUseProgram(NULL);
}