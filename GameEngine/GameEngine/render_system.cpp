#include "render_system.h"

GLuint VertexBufferObject::bound_id = 0;
GLuint IndexBufferObject::bound_id = 0;

// Positions all containers
glm::vec3 cube_positions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

glm::vec3 dir_light_position(1.2f, 1.0f, 2.0f);

glm::vec3 point_light_positions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

RenderSystem::RenderSystem()
{
	
}

RenderSystem::~RenderSystem()
{

}

bool RenderSystem::initGL()
{
	bool success = true;

	active_program = new ShaderProgram("mapped_material_vertex.vert", "mapped_material_fragment.frag");

	// Initialize clear color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

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
	global_vbo = new VertexBufferObject();
	global_vbo->bind();
	global_vbo->bufferData(sizeof(vertex_data), vertex_data, STATIC_DRAW);

	// Create IBO
	global_ibo = new IndexBufferObject();
	global_ibo->bind();
	global_ibo->bufferData(sizeof(index_data), index_data, STATIC_DRAW);

	glActiveTexture(GL_TEXTURE0);

	// Sorry about the hard-coded path :(
	active_texture = new Texture("..\\assets\\textures\\crate.png");
	active_texture->bind();
	active_texture->setWrapType(CLAMP_TO_EDGE, CLAMP_TO_EDGE);
	active_texture->setInterpolation(LINEAR, LINEAR);

	glActiveTexture(GL_TEXTURE1);

	specular_texture = new Texture("..\\assets\\textures\\crate_specular.png");
	specular_texture->bind();
	specular_texture->setWrapType(CLAMP_TO_EDGE, CLAMP_TO_EDGE);
	specular_texture->setInterpolation(LINEAR, LINEAR);

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

	int view_pos_uniform = active_program->getUniformLocation("view_pos");
	glUniform3f(view_pos_uniform, camera.getCenter().x, camera.getCenter().y, camera.getCenter().z);

	int mat_diffuse_uniform = active_program->getUniformLocation("material.diffuse");
	int mat_specular_uniform = active_program->getUniformLocation("material.specular");
	int mat_shininess_uniform = active_program->getUniformLocation("material.shininess");

	glUniform1i(mat_diffuse_uniform, 0);	// TODO: Find out why this is GL_TEXTURE0 (0) instead of getId()
	glUniform1i(mat_specular_uniform, 1);
	glUniform1f(mat_shininess_uniform, 64.0f);

	/*int light_pos_uniform = active_program->getUniformLocation("light.position");
	int light_ambient_uniform = active_program->getUniformLocation("light.ambient");
	int light_diffuse_uniform = active_program->getUniformLocation("light.diffuse");
	int light_specular_uniform = active_program->getUniformLocation("light.specular");

	glUniform3f(light_pos_uniform, light_pos.x, light_pos.y, light_pos.z);
	glUniform3f(light_ambient_uniform, 0.2f, 0.2f, 0.2f);
	glUniform3f(light_diffuse_uniform, 0.5f, 0.5f, 0.5f);
	glUniform3f(light_specular_uniform, 1.0f, 1.0f, 1.0f);

	int light_direction_uniform = active_program->getUniformLocation("light.direction");
	int light_constant_uniform = active_program->getUniformLocation("light.constant");
	int light_linear_uniform = active_program->getUniformLocation("light.linear");
	int light_quadratic_uniform = active_program->getUniformLocation("light.quadratic");

	glUniform3f(light_direction_uniform, -0.2f, -1.0f, -0.3f);
	glUniform1f(light_constant_uniform, 1.0f);
	glUniform1f(light_linear_uniform, 0.09f);
	glUniform1f(light_quadratic_uniform, 0.032f);*/

	int dir_light_direction_uniform = active_program->getUniformLocation("dir_light.direction");
	int dir_light_ambient_uniform = active_program->getUniformLocation("dir_light.ambient");
	int dir_light_diffuse_uniform = active_program->getUniformLocation("dir_light.diffuse");
	int dir_light_specular_uniform = active_program->getUniformLocation("dir_light.specular");

	int point_light_position_uniform_0 = active_program->getUniformLocation("point_lights[0].position");
	int point_light_constant_uniform_0 = active_program->getUniformLocation("point_lights[0].constant");
	int point_light_linear_uniform_0 = active_program->getUniformLocation("point_lights[0].linear");
	int point_light_quadratic_uniform_0 = active_program->getUniformLocation("point_lights[0].quadratic");
	int point_light_ambient_uniform_0 = active_program->getUniformLocation("point_lights[0].ambient");
	int point_light_diffuse_uniform_0 = active_program->getUniformLocation("point_lights[0].diffuse");
	int point_light_specular_uniform_0 = active_program->getUniformLocation("point_lights[0].specular");

	int point_light_position_uniform_1 = active_program->getUniformLocation("point_lights[1].position");
	int point_light_constant_uniform_1 = active_program->getUniformLocation("point_lights[1].constant");
	int point_light_linear_uniform_1 = active_program->getUniformLocation("point_lights[1].linear");
	int point_light_quadratic_uniform_1 = active_program->getUniformLocation("point_lights[1].quadratic");
	int point_light_ambient_uniform_1 = active_program->getUniformLocation("point_lights[1].ambient");
	int point_light_diffuse_uniform_1 = active_program->getUniformLocation("point_lights[1].diffuse");
	int point_light_specular_uniform_1 = active_program->getUniformLocation("point_lights[1].specular");

	int point_light_position_uniform_2 = active_program->getUniformLocation("point_lights[2].position");
	int point_light_constant_uniform_2 = active_program->getUniformLocation("point_lights[2].constant");
	int point_light_linear_uniform_2 = active_program->getUniformLocation("point_lights[2].linear");
	int point_light_quadratic_uniform_2 = active_program->getUniformLocation("point_lights[2].quadratic");
	int point_light_ambient_uniform_2 = active_program->getUniformLocation("point_lights[2].ambient");
	int point_light_diffuse_uniform_2 = active_program->getUniformLocation("point_lights[2].diffuse");
	int point_light_specular_uniform_2 = active_program->getUniformLocation("point_lights[2].specular");

	int point_light_position_uniform_3 = active_program->getUniformLocation("point_lights[3].position");
	int point_light_constant_uniform_3 = active_program->getUniformLocation("point_lights[3].constant");
	int point_light_linear_uniform_3 = active_program->getUniformLocation("point_lights[3].linear");
	int point_light_quadratic_uniform_3 = active_program->getUniformLocation("point_lights[3].quadratic");
	int point_light_ambient_uniform_3 = active_program->getUniformLocation("point_lights[3].ambient");
	int point_light_diffuse_uniform_3 = active_program->getUniformLocation("point_lights[3].diffuse");
	int point_light_specular_uniform_3 = active_program->getUniformLocation("point_lights[3].specular");

	int spot_light_position_uniform = active_program->getUniformLocation("spot_light.position");
	int spot_light_direction_uniform = active_program->getUniformLocation("spot_light.direction");
	int spot_light_cut_off_uniform = active_program->getUniformLocation("spot_light.cut_off");
	int spot_light_outer_cut_off_uniform = active_program->getUniformLocation("spot_light.outer_cut_off");
	int spot_light_constant_uniform = active_program->getUniformLocation("spot_light.constant");
	int spot_light_linear_uniform = active_program->getUniformLocation("spot_light.linear");
	int spot_light_quadratic_uniform = active_program->getUniformLocation("spot_light.quadratic");
	int spot_light_ambient_uniform = active_program->getUniformLocation("spot_light.ambient");
	int spot_light_diffuse_uniform = active_program->getUniformLocation("spot_light.diffuse");
	int spot_light_specular_uniform = active_program->getUniformLocation("spot_light.specular");

	glUniform3f(dir_light_direction_uniform, -0.2f, -1.0f, -0.3f);
	glUniform3f(dir_light_ambient_uniform, 0.05f, 0.05f, 0.05f);
	glUniform3f(dir_light_diffuse_uniform, 0.4f, 0.4f, 0.4f);
	glUniform3f(dir_light_specular_uniform, 0.5f, 0.5f, 0.5f);

	glUniform3f(point_light_position_uniform_0, point_light_positions[0].x, point_light_positions[0].y, point_light_positions[0].z);
	glUniform3f(point_light_ambient_uniform_0, 0.05f, 0.05f, 0.05f);
	glUniform3f(point_light_diffuse_uniform_0, 0.8f, 0.8f, 0.8f);
	glUniform3f(point_light_specular_uniform_0, 1.0f, 1.0f, 1.0f);
	glUniform1f(point_light_constant_uniform_0, 1.0f);
	glUniform1f(point_light_linear_uniform_0, 0.09f);
	glUniform1f(point_light_quadratic_uniform_0, 0.032f);

	glUniform3f(point_light_position_uniform_1, point_light_positions[1].x, point_light_positions[1].y, point_light_positions[1].z);
	glUniform3f(point_light_ambient_uniform_1, 0.05f, 0.05f, 0.05f);
	glUniform3f(point_light_diffuse_uniform_1, 0.8f, 0.8f, 0.8f);
	glUniform3f(point_light_specular_uniform_1, 1.0f, 1.0f, 1.0f);
	glUniform1f(point_light_constant_uniform_1, 1.0f);
	glUniform1f(point_light_linear_uniform_1, 0.09f);
	glUniform1f(point_light_quadratic_uniform_1, 0.032f);

	glUniform3f(point_light_position_uniform_2, point_light_positions[2].x, point_light_positions[2].y, point_light_positions[2].z);
	glUniform3f(point_light_ambient_uniform_2, 0.05f, 0.05f, 0.05f);
	glUniform3f(point_light_diffuse_uniform_2, 0.8f, 0.8f, 0.8f);
	glUniform3f(point_light_specular_uniform_2, 1.0f, 1.0f, 1.0f);
	glUniform1f(point_light_constant_uniform_2, 1.0f);
	glUniform1f(point_light_linear_uniform_2, 0.09f);
	glUniform1f(point_light_quadratic_uniform_2, 0.032f);

	glUniform3f(point_light_position_uniform_3, point_light_positions[3].x, point_light_positions[3].y, point_light_positions[3].z);
	glUniform3f(point_light_ambient_uniform_3, 0.05f, 0.05f, 0.05f);
	glUniform3f(point_light_diffuse_uniform_3, 0.8f, 0.8f, 0.8f);
	glUniform3f(point_light_specular_uniform_3, 1.0f, 1.0f, 1.0f);
	glUniform1f(point_light_constant_uniform_3, 1.0f);
	glUniform1f(point_light_linear_uniform_3, 0.09f);
	glUniform1f(point_light_quadratic_uniform_3, 0.032f);

	glUniform3f(spot_light_position_uniform, camera.getCenter().x, camera.getCenter().y, camera.getCenter().z);
	glUniform3f(spot_light_direction_uniform, camera.getDirection().x, camera.getDirection().y, camera.getDirection().z);
	glUniform1f(spot_light_cut_off_uniform, lm::cos(lm::radians(12.5f))); // Must be a cosine value as this prevents an expensive arccos calculation in the fragment shader.
	glUniform1f(spot_light_outer_cut_off_uniform, lm::cos(lm::radians(15.0f)));
	glUniform1f(spot_light_constant_uniform, 1.0f);
	glUniform1f(spot_light_linear_uniform, 0.09f);
	glUniform1f(spot_light_quadratic_uniform, 0.032f);
	glUniform3f(spot_light_ambient_uniform, 0.0f, 0.0f, 0.0f);
	glUniform3f(spot_light_diffuse_uniform, 1.0f, 1.0f, 1.0f);
	glUniform3f(spot_light_specular_uniform, 1.0f, 1.0f, 1.0f);

	// Set vertex data
	global_vbo->bind();

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

	lm::mat4 view = camera.getView();
	GLint view_uniform = active_program->getUniformLocation("view");
	glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));

	// Create the perspective projection matrix
	lm::mat4 proj = camera.getProjection();
	//lm::mat4 proj = lm::perspective(lm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 10.0f);
	GLint proj_uniform = active_program->getUniformLocation("proj");
	glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, glm::value_ptr(proj));

	// Set index data and render
	global_ibo->bind();

	// Apply the model transformations
	int model_uniform = active_program->getUniformLocation("model");
	for (unsigned int i = 0; i < 10; i++)
	{
		model = lm::mat4();
		model = glm::translate(model, cube_positions[i]);
		float angle = 20.0f * i;
		model = lm::rotate(model, angle, lm::vec3(1.0f, 0.3f, 0.5f));
		glUniformMatrix4fv(active_program->getUniformLocation("model"), 1, GL_FALSE, lm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	//// Apply the model transformation
	//model = lm::rotate(model, lm::radians(0.25f), lm::vec3(0.0f, 0.0f, 1.0f));
	//int model_uniform = active_program->getUniformLocation("model");
	//glUniformMatrix4fv(model_uniform, 1, GL_FALSE, lm::value_ptr(model));

	//// Set index data and render
	//global_ibo->bind();

	////glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, NULL);
	//glDrawArrays(GL_TRIANGLES, 0, 36);

	// Disable vertex position
	glDisableVertexAttribArray(pos_attrib);
	glDisableVertexAttribArray(col_attrib);
	glDisableVertexAttribArray(tex_attrib);
	glDisableVertexAttribArray(norm_attrib);

	// Unbind program
	glUseProgram(NULL);
}

void RenderSystem::renderMesh(Lemur::Camera camera)
{
	// Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind program
	active_program->use();

	lm::mat4 view = camera.getView();
	GLint view_uniform = active_program->getUniformLocation("view");
	glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));

	// Create the perspective projection matrix
	lm::mat4 proj = camera.getProjection();
	GLint proj_uniform = active_program->getUniformLocation("proj");
	glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, glm::value_ptr(proj));

	// Apply the model transformation
	model = lm::rotate(model, lm::radians(0.25f), lm::vec3(0.0f, 0.0f, 1.0f));
	int model_uniform = active_program->getUniformLocation("model");
	glUniformMatrix4fv(model_uniform, 1, GL_FALSE, lm::value_ptr(model));

	int mat_ambient_uniform = active_program->getUniformLocation("material.ambient");
	int mat_diffuse_uniform = active_program->getUniformLocation("material.diffuse");
	int mat_specular_uniform = active_program->getUniformLocation("material.specular");
	int mat_shininess_uniform = active_program->getUniformLocation("material.shininess");

	glUniform3f(mat_ambient_uniform, 0.24725f, 0.1995f, 0.0745f);
	glUniform3f(mat_diffuse_uniform, 0.75164f, 0.60648f, 0.22648f);
	glUniform3f(mat_specular_uniform, 0.628281f, 0.555802f, 0.366065f);
	glUniform1f(mat_shininess_uniform, 0.4f);

	/*int light_pos_uniform = active_program->getUniformLocation("light.position");
	int light_ambient_uniform = active_program->getUniformLocation("light.ambient");
	int light_diffuse_uniform = active_program->getUniformLocation("light.diffuse");
	int light_specular_uniform = active_program->getUniformLocation("light.specular");

	glUniform3f(light_pos_uniform, light_pos.x, light_pos.y, light_pos.z);
	glUniform3f(light_ambient_uniform, 1.0f, 1.0f, 1.0f);
	glUniform3f(light_diffuse_uniform, 1.0f, 1.0f, 1.0f);
	glUniform3f(light_specular_uniform, 1.0f, 1.0f, 1.0f);*/

	// Set vertex data
	mesh_vbo->bind();

	// Enable vertex position
	int pos_attrib = active_program->getAttribLocation("position");
	glEnableVertexAttribArray(pos_attrib);
	glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	mesh_normal_bo->bind();

	int norm_attrib = active_program->getAttribLocation("in_normal");
	glEnableVertexAttribArray(norm_attrib);
	glVertexAttribPointer(norm_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Set index data and render
	mesh_ibo->bind();
	glDrawElements(GL_TRIANGLES, mesh->vertexIndexCount(), GL_UNSIGNED_INT, NULL);

	// Disable vertex position
	glDisableVertexAttribArray(pos_attrib);
	glDisableVertexAttribArray(norm_attrib);

	// Unbind program
	glUseProgram(NULL);
}

void RenderSystem::setMesh(Mesh* mesh)
{
	this->mesh = mesh;

	active_program = new ShaderProgram("material_vertex.vert", "material_fragment.frag");

	mesh_vbo = new VertexBufferObject();
	mesh_vbo->bind();
	mesh_vbo->bufferData(mesh->vertexBufferSize(), (float*)mesh->vertexBuffer(), STATIC_DRAW);

	mesh_ibo = new IndexBufferObject();
	mesh_ibo->bind();
	mesh_ibo->bufferData(mesh->vertexIndexBufferSize(), (unsigned int*)mesh->vertexIndexBuffer(), STATIC_DRAW);

	mesh_normal_bo = new VertexBufferObject();
	mesh_normal_bo->bind();
	mesh_normal_bo->bufferData(mesh->normalBufferSize(), (float*)mesh->normalBuffer(), STATIC_DRAW);
}