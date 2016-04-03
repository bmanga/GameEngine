#include "render_system.h"


/*
THE GRAPHICS PIPELINE:

https://old.cg.tuwien.ac.at/research/publications/2007/bauchinger-2007-mre/bauchinger-2007-mre-Thesis.pdf
Page 48: Starts covering the graphics pipeline of the YARE game engine.
*/

GLuint VertexBufferObject::bound_id = 0;
GLuint IndexBufferObject::bound_id = 0;
/*
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

glm::vec3 mesh_light_pos(0.0f, 3.0f, 0.0f);

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


	//DEBUG
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
	glUniform3f(spot_light_diffuse_uniform, 1.0f, 0.0f, 1.0f);
	glUniform3f(spot_light_specular_uniform, 1.0f, 0.0f, 1.0f);

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

	int light_pos_uniform = active_program->getUniformLocation("light.position");
	int light_ambient_uniform = active_program->getUniformLocation("light.ambient");
	int light_diffuse_uniform = active_program->getUniformLocation("light.diffuse");
	int light_specular_uniform = active_program->getUniformLocation("light.specular");

	glUniform3f(light_pos_uniform, mesh_light_pos.x, mesh_light_pos.y, mesh_light_pos.z);
	glUniform3f(light_ambient_uniform, 1.0f, 1.0f, 1.0f);
	glUniform3f(light_diffuse_uniform, 1.0f, 1.0f, 1.0f);
	glUniform3f(light_specular_uniform, 1.0f, 1.0f, 1.0f);

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

VertexBufferObject* component_vbo = nullptr;
IndexBufferObject* component_ibo = nullptr;
VertexBufferObject* component_nbo = nullptr;

void RenderSystem::renderComponent(Lemur::Camera camera)
{
	if (!component)
	{
		component = new RenderComponent();
		component->mesh = new Mesh();
		component->mesh->setMeshData(load_obj("testcube.objm"));
		//component->texture = new Texture("..\\assets\\textures\\crate.png");
		component->program = new ShaderProgram("material_vertex.vert", "material_fragment.frag");

		component_vbo = new VertexBufferObject();
		component_vbo->bind();
		component_vbo->bufferData(component->mesh->vertexBufferSize(), (float*)component->mesh->vertexBuffer(), STATIC_DRAW);

		component_ibo = new IndexBufferObject();
		component_ibo->bind();
		component_ibo->bufferData(component->mesh->vertexIndexBufferSize(), (unsigned int*)component->mesh->vertexIndexBuffer(), STATIC_DRAW);

		component_nbo = new VertexBufferObject();
		component_nbo->bind();
		component_nbo->bufferData(component->mesh->normalBufferSize(), (float*)component->mesh->normalBuffer(), STATIC_DRAW);
	}

	// Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind program
	component->program->use();

	lm::mat4 view = camera.getView();
	GLint view_uniform = component->program->getUniformLocation("view");
	glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));

	// Create the perspective projection matrix
	lm::mat4 proj = camera.getProjection();
	GLint proj_uniform = component->program->getUniformLocation("proj");
	glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, glm::value_ptr(proj));

	// Apply the model transformation
	model = lm::rotate(model, lm::radians(0.25f), lm::vec3(0.0f, 0.0f, 1.0f));
	int model_uniform = component->program->getUniformLocation("model");
	glUniformMatrix4fv(model_uniform, 1, GL_FALSE, lm::value_ptr(model));

	int mat_ambient_uniform = component->program->getUniformLocation("material.ambient");
	int mat_diffuse_uniform = component->program->getUniformLocation("material.diffuse");
	int mat_specular_uniform = component->program->getUniformLocation("material.specular");
	int mat_shininess_uniform = component->program->getUniformLocation("material.shininess");

	glUniform3f(mat_ambient_uniform, 0.0215f, 0.1745f, 0.0215f);
	glUniform3f(mat_diffuse_uniform, 0.07568f, 0.61424f, 0.07568f);
	glUniform3f(mat_specular_uniform, 0.633f, 0.727811f, 0.633f);
	glUniform1f(mat_shininess_uniform, 0.6f);

	int light_pos_uniform = component->program->getUniformLocation("light.position");
	int light_ambient_uniform = component->program->getUniformLocation("light.ambient");
	int light_diffuse_uniform = component->program->getUniformLocation("light.diffuse");
	int light_specular_uniform = component->program->getUniformLocation("light.specular");

	glUniform3f(light_pos_uniform, mesh_light_pos.x, mesh_light_pos.y, mesh_light_pos.z);
	glUniform3f(light_ambient_uniform, 1.0f, 1.0f, 1.0f);
	glUniform3f(light_diffuse_uniform, 1.0f, 1.0f, 1.0f);
	glUniform3f(light_specular_uniform, 1.0f, 1.0f, 1.0f);

	component_vbo->bind();

	// Enable vertex position
	int pos_attrib = component->program->getAttribLocation("position");
	glEnableVertexAttribArray(pos_attrib);
	glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	component_nbo->bind();

	int norm_attrib = component->program->getAttribLocation("in_normal");
	glEnableVertexAttribArray(norm_attrib);
	glVertexAttribPointer(norm_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	component_ibo->bind();
	glDrawElements(GL_TRIANGLES, component->mesh->vertexIndexCount(), GL_UNSIGNED_INT, NULL);

	// Disable vertex position
	glDisableVertexAttribArray(pos_attrib);
	glDisableVertexAttribArray(norm_attrib);

	// Unbind program
	glUseProgram(NULL);
}
*/

//glm::vec3 mesh_light_pos(0.0f, 3.0f, 0.0f);

bool bos_created = false;
float count = 0.0f;

ShaderProgram* active_program = nullptr;
RenderComponent* active_render_component = nullptr;

void RenderSystem::updateEntity(float elapsed_time, ecs::Entity entity)
{
	// Initialize clear color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create the various buffer objects on the first pass through updateEntity()
	if (!bos_created && manager.getComponentStore<RenderComponent>().has(entity))
	{
		RenderComponent& renderable = manager.getComponentStore<RenderComponent>().get(entity);
		active_render_component = &renderable;

		component_vbo = new VertexBufferObject();
		component_vbo->bind();
		component_vbo->bufferData(renderable.mesh->vertexBufferSize(), (float*)renderable.mesh->vertexBuffer(), STATIC_DRAW);

		component_ibo = new IndexBufferObject();
		component_ibo->bind();
		component_ibo->bufferData(renderable.mesh->vertexIndexBufferSize(), (unsigned int*)renderable.mesh->vertexIndexBuffer(), STATIC_DRAW);

		component_nbo = new VertexBufferObject();
		component_nbo->bind();
		component_nbo->bufferData(renderable.mesh->normalBufferSize(), (float*)renderable.mesh->normalBuffer(), STATIC_DRAW);

		bos_created = true;
	}

	// Modify the various matrices and materials for the renderable component
	// Also binds the active shader program
	if (manager.getComponentStore<RenderComponent>().has(entity) &&
		manager.getComponentStore<PositionComponent>().has(entity))
	{
		RenderComponent& renderable = manager.getComponentStore<RenderComponent>().get(entity);
		PositionComponent& position = manager.getComponentStore<PositionComponent>().get(entity);

		// Bind program
		renderable.program->use();
		active_program = renderable.program;

		lm::mat4 view = camera.getView();
		GLint view_uniform = renderable.program->getUniformLocation("view");
		glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));

		// Create the perspective projection matrix
		lm::mat4 proj = camera.getProjection();
		GLint proj_uniform = renderable.program->getUniformLocation("proj");
		glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, glm::value_ptr(proj));

		// Identity matrix
		model = glm::mat4(1.0f);

		// Apply the model transformation
		//model = lm::rotate(model, lm::radians(0.25f), lm::vec3(0.0f, 0.0f, 1.0f));
		model = lm::translate(model, lm::vec3(position.x, position.y, position.z));
		int model_uniform = renderable.program->getUniformLocation("model");
		glUniformMatrix4fv(model_uniform, 1, GL_FALSE, lm::value_ptr(model));

		int mat_ambient_uniform = renderable.program->getUniformLocation("material.ambient");
		int mat_diffuse_uniform = renderable.program->getUniformLocation("material.diffuse");
		int mat_specular_uniform = renderable.program->getUniformLocation("material.specular");
		int mat_shininess_uniform = renderable.program->getUniformLocation("material.shininess");

		glUniform3f(mat_ambient_uniform, 0.05f, 0.0f, 0.0f);
		glUniform3f(mat_diffuse_uniform, 0.5f, 0.4f, 0.4f);
		glUniform3f(mat_specular_uniform, 0.7f, 0.04f, 0.04f);
		glUniform1f(mat_shininess_uniform, 0.078125f);
	}

	// Uploads the light coordinates and colors
	if (manager.getComponentStore<LightComponent>().has(entity) &&
		manager.getComponentStore<PositionComponent>().has(entity) &&
		active_program != nullptr)
	{
		LightComponent& light = manager.getComponentStore<LightComponent>().get(entity);
		PositionComponent& light_position = manager.getComponentStore<PositionComponent>().get(entity);

		int light_pos_uniform = active_program->getUniformLocation("light.position");
		int light_ambient_uniform = active_program->getUniformLocation("light.ambient");
		int light_diffuse_uniform = active_program->getUniformLocation("light.diffuse");
		int light_specular_uniform = active_program->getUniformLocation("light.specular");

		light_position.x = (float)cos(count) * 30.0f;
		light_position.y = (float)sin(count) * 30.0f;
		count += 0.05f;
		if (count >= glm::pi<float>() * 2.0f) count = 0;

		glUniform3f(light_pos_uniform, light_position.x, light_position.y, light_position.z);
		glUniform3f(light_ambient_uniform, light.ambient.r, light.ambient.g, light.ambient.b);
		glUniform3f(light_diffuse_uniform, light.diffuse.r, light.diffuse.g, light.diffuse.b);
		glUniform3f(light_specular_uniform, light.specular.r, light.specular.g, light.specular.b);
	}
}

void RenderSystem::end()
{
	// Binds the various buffer objects and draws the elements
	if (active_program != nullptr)
	{
		component_vbo->bind();

		// Enable vertex position
		int pos_attrib = active_program->getAttribLocation("position");
		glEnableVertexAttribArray(pos_attrib);
		glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

		component_nbo->bind();

		int norm_attrib = active_program->getAttribLocation("in_normal");
		glEnableVertexAttribArray(norm_attrib);
		glVertexAttribPointer(norm_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

		component_ibo->bind();
		glDrawElements(GL_TRIANGLES, active_render_component->mesh->vertexIndexCount(), GL_UNSIGNED_INT, NULL);

		// Disable vertex position
		glDisableVertexAttribArray(pos_attrib);
		glDisableVertexAttribArray(norm_attrib);

		// Unbind program
		glUseProgram(NULL);
	}
}