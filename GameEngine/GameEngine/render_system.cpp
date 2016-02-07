#include "render_system.h"

GLuint VertexBufferObject::bound_id = 0;
GLuint IndexBufferObject::bound_id = 0;

RenderSystem::RenderSystem()
{
	
}

RenderSystem::~RenderSystem()
{

}

bool RenderSystem::initGL()
{
	bool success = true;

	active_program.compileShaders("testvert.vert", "testfrag.frag");
	active_program.compileProgram();
	active_program.link();

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
	active_program.use();

	// Create the view transformation
	//lm::mat4 view = lm::lookAt(
	//	lm::vec3(1.5f, 1.5f, 1.5f),
	//	lm::vec3(0.0f, 0.0f, 0.0f),
	//	lm::vec3(0.0f, 0.0f, 1.0f)
	//	);

	lm::mat4 view = camera.getView();
	GLint view_uniform = active_program.getUniformLocation("view");
	glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));

	// Create the perspective projection matrix
	lm::mat4 proj = camera.getProjection();
	//lm::mat4 proj = lm::perspective(lm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 10.0f);
	GLint proj_uniform = active_program.getUniformLocation("proj");
	glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, glm::value_ptr(proj));

	// Apply the model transformation
	model = lm::rotate(model, lm::radians(0.25f), lm::vec3(0.0f, 0.0f, 1.0f));
	int model_uniform = active_program.getUniformLocation("model");
	glUniformMatrix4fv(model_uniform, 1, GL_FALSE, lm::value_ptr(model));

	int view_pos_uniform = active_program.getUniformLocation("view_pos");
	glUniform3f(view_pos_uniform, camera.getCenter().x, camera.getCenter().y, camera.getCenter().z);


	int mat_diffuse_uniform = active_program.getUniformLocation("material.diffuse");
	int mat_specular_uniform = active_program.getUniformLocation("material.specular");
	int mat_shininess_uniform = active_program.getUniformLocation("material.shininess");

	glUniform1i(mat_diffuse_uniform, 0);	// TODO: Find out why this is GL_TEXTURE0 (0) instead of getId()
	glUniform1i(mat_specular_uniform, 1);
	glUniform1f(mat_shininess_uniform, 64.0f);

	int light_pos_uniform = active_program.getUniformLocation("light.position");
	int light_ambient_uniform = active_program.getUniformLocation("light.ambient");
	int light_diffuse_uniform = active_program.getUniformLocation("light.diffuse");
	int light_specular_uniform = active_program.getUniformLocation("light.specular");

	glUniform3f(light_pos_uniform, light_pos.x, light_pos.y, light_pos.z);
	glUniform3f(light_ambient_uniform, 0.2f, 0.2f, 0.2f);
	glUniform3f(light_diffuse_uniform, 0.5f, 0.5f, 0.5f);
	glUniform3f(light_specular_uniform, 1.0f, 1.0f, 1.0f);

	// Set vertex data
	global_vbo->bind();

	// Enable vertex position
	int pos_attrib = active_program.getAttribLocation("position");
	glEnableVertexAttribArray(pos_attrib);
	glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 0);

	int col_attrib = active_program.getAttribLocation("in_color");
	glEnableVertexAttribArray(col_attrib);
	glVertexAttribPointer(col_attrib, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	int tex_attrib = active_program.getAttribLocation("in_texcoord");
	glEnableVertexAttribArray(tex_attrib);
	glVertexAttribPointer(tex_attrib, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	int norm_attrib = active_program.getAttribLocation("in_normal");
	glEnableVertexAttribArray(norm_attrib);
	glVertexAttribPointer(norm_attrib, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(8 * sizeof(GLfloat)));

	// Set index data and render
	global_ibo->bind();

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

void RenderSystem::renderMesh(Lemur::Camera camera)
{
	// Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind program
	active_program.use();

	lm::mat4 view = camera.getView();
	GLint view_uniform = active_program.getUniformLocation("view");
	glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));

	// Create the perspective projection matrix
	lm::mat4 proj = camera.getProjection();
	GLint proj_uniform = active_program.getUniformLocation("proj");
	glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, glm::value_ptr(proj));

	// Apply the model transformation
	model = lm::rotate(model, lm::radians(0.25f), lm::vec3(0.0f, 0.0f, 1.0f));
	int model_uniform = active_program.getUniformLocation("model");
	glUniformMatrix4fv(model_uniform, 1, GL_FALSE, lm::value_ptr(model));

	// Set vertex data
	mesh_vbo->bind();

	// Enable vertex position
	int pos_attrib = active_program.getAttribLocation("position");
	glEnableVertexAttribArray(pos_attrib);
	glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	mesh_normal_bo->bind();

	int norm_attrib = active_program.getAttribLocation("in_normal");
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