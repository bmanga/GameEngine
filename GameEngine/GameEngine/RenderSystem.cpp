#include "RenderSystem.h"

#include <unordered_map>
#include <vector>
#include <string>
#include "VertexBuffer.h"
#include "Mesh.h"
GLuint VertexBufferObject::bound_id = 0;
GLuint IndexBufferObject::bound_id = 0;

// TODO: This could be a vector of struct instances instead...
std::vector<Lemur::VertexBuffer> vbs;

std::vector<CPosition> positions;
std::vector<CRenderable> renderables;

std::unordered_map<std::size_t, bool> creation_map;

void RenderSystem::render(Lemur::Camera camera)
{
	// TODO(Kavan): This could be implemented more cleanly...
	// Design a pattern which would allow all entities with a specific signature to be inside one loop,
	// so that they could all be acted upon in one pass instead of storing them in one pass.
	manager.forEntitiesMatching<Lemur::SRenderable>([this](auto entity_index, auto& position, auto& renderable)
	{
		std::unordered_map<std::size_t, bool>::const_iterator got = creation_map.find(entity_index);
		if (got == creation_map.end())
		{
			auto& mesh = renderable.mesh;
			std::string vertex = "position:3f";
			std::string texture = "in_textcoord:2f";
			std::string normal = "in_normal:3f";
			const Lemur::Mesh::MeshBufferHeader& header = mesh->bufferHeader();

			if (header.has_normals)
			{
				vertex += "," + normal;
			}

			if (header.has_texture_coords)
			{
				vertex += "," + texture;
			}

			Lemur::VertexBuffer vb(vertex.c_str());

			vb.push_back((const char*)mesh->vertexBuffer(), mesh->vertexCount(),
				mesh->indexBuffer(), mesh->indexCount());

			renderables.push_back(renderable);
			positions.push_back(position);

			vbs.push_back(std::move(vb));

			creation_map.insert(std::make_pair(entity_index, true));
		}
	});

	// Initialize clear color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (unsigned int i = 0; i < vbs.size(); i++)
	{
		auto& vb = vbs[i];
		CRenderable renderable = renderables.at(i);
		ShaderProgram prog = *renderable.material->shader.get();
		CPosition pos = positions.at(i);

		prog.use();

		int view_pos_uniform = prog.getUniformLocation("view_pos");
		glUniform3f(view_pos_uniform, camera.getCenter().x, camera.getCenter().y, camera.getCenter().z);

		lm::mat4 view = camera.getView();
		GLint view_uniform = prog.getUniformLocation("view");
		glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));

		// Create the perspective projection matrix
		lm::mat4 proj = camera.getProjection();
		GLint proj_uniform = prog.getUniformLocation("proj");
		glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, glm::value_ptr(proj));

		int mat_shininess_uniform = prog.getUniformLocation("material.shininess");
		glUniform1f(mat_shininess_uniform, renderable.material->shininess);

		int mat_transparency_uniform = prog.getUniformLocation("material.transparency");
		glUniform1f(mat_transparency_uniform, renderable.material->transparency);

		if (renderable.material->use_texturing)
		{ 
			auto& mat = renderable.material;

			if (mat->texture)
			{
				glActiveTexture(GL_TEXTURE0);
				renderable.material->texture->bind();
			}

			if (mat->bump_map)
			{
				glActiveTexture(GL_TEXTURE1);
				renderable.material->bump_map->bind();
			}

			if (mat->normal_map)
			{
				glActiveTexture(GL_TEXTURE2);
				renderable.material->normal_map->bind();
			}

			
			if (mat->specular_map)
			{
				glActiveTexture(GL_TEXTURE3);
				renderable.material->specular_map->bind();
			}

			if (mat->mask_map)
			{

				glActiveTexture(GL_TEXTURE4);
				renderable.material->mask_map->bind();

			}
			glActiveTexture(GL_TEXTURE0);

			int mat_texture_uniform = prog.getUniformLocation("material.texture");
			int mat_diffuse_map_uniform = prog.getUniformLocation("material.diffuse_map");
			int mat_normal_map_uniform = prog.getUniformLocation("material.normal_map");
			int mat_specular_map_uniform = prog.getUniformLocation("material.specular_map");
			int mat_mask_map_uniform = prog.getUniformLocation("material.mask_map");

			glUniform1i(mat_texture_uniform, 0);
			glUniform1i(mat_diffuse_map_uniform, 1);
			glUniform1i(mat_normal_map_uniform, 2);
			glUniform1i(mat_specular_map_uniform, 3);
			glUniform1i(mat_mask_map_uniform, 4);
		}
		else
		{
			int mat_ambient_uniform = prog.getUniformLocation("material.ambient");
			int mat_diffuse_uniform = prog.getUniformLocation("material.diffuse");
			int mat_emissive_uniform = prog.getUniformLocation("material.emissive");
			int mat_specular_uniform = prog.getUniformLocation("material.specular");

			glUniform3f(mat_ambient_uniform, renderable.material->ambient[0], renderable.material->ambient[1], renderable.material->ambient[2]);
			glUniform3f(mat_diffuse_uniform, renderable.material->diffuse[0], renderable.material->diffuse[1], renderable.material->diffuse[2]);
			glUniform3f(mat_emissive_uniform, renderable.material->emissive[0], renderable.material->emissive[1], renderable.material->emissive[2]);
			glUniform3f(mat_specular_uniform, renderable.material->specular[0], renderable.material->specular[1], renderable.material->specular[2]);
		}

		// Point lights
		unsigned int count = 0;
		manager.forEntitiesMatching<Lemur::SPointLight>([&](auto entity_index, auto& position, auto& light)
		{
			std::string i = std::to_string(count++);

			int point_light_position_uniform = prog.getUniformLocation(("point_lights[" + i + "].position").c_str());
			int point_light_constant_uniform = prog.getUniformLocation(("point_lights[" + i + "].constant").c_str());
			int point_light_linear_uniform = prog.getUniformLocation(("point_lights[" + i + "].linear").c_str());
			int point_light_quadratic_uniform = prog.getUniformLocation(("point_lights[" + i + "].quadratic").c_str());
			int point_light_ambient_uniform = prog.getUniformLocation(("point_lights[" + i + "].ambient").c_str());
			int point_light_diffuse_uniform = prog.getUniformLocation(("point_lights[" + i + "].diffuse").c_str());
			int point_light_specular_uniform = prog.getUniformLocation(("point_lights[" + i + "].specular").c_str());

			glUniform3f(point_light_position_uniform, position.x, position.y, position.z);
			glUniform3f(point_light_ambient_uniform, light.ambient.r, light.ambient.g, light.ambient.b);
			glUniform3f(point_light_diffuse_uniform, light.diffuse.r, light.diffuse.r, light.diffuse.r);
			glUniform3f(point_light_specular_uniform, light.specular.r, light.specular.r, light.specular.r);
			glUniform1f(point_light_constant_uniform, light.constant);
			glUniform1f(point_light_linear_uniform, light.linear);
			glUniform1f(point_light_quadratic_uniform, light.quadratic);
		});

		// Directional lights
		manager.forEntitiesMatching<Lemur::SDirLight>([&](auto entity_index, auto& position, auto& dir_light)
		{
			int dir_light_direction_uniform = prog.getUniformLocation("dir_light.direction");
			int dir_light_ambient_uniform = prog.getUniformLocation("dir_light.ambient");
			int dir_light_diffuse_uniform = prog.getUniformLocation("dir_light.diffuse");
			int dir_light_specular_uniform = prog.getUniformLocation("dir_light.specular");

			glUniform3f(dir_light_direction_uniform, dir_light.direction.x, dir_light.direction.y, dir_light.direction.z);
			glUniform3f(dir_light_ambient_uniform, dir_light.ambient.r, dir_light.ambient.g, dir_light.ambient.b);
			glUniform3f(dir_light_diffuse_uniform, dir_light.diffuse.r, dir_light.diffuse.g, dir_light.diffuse.b);
			glUniform3f(dir_light_specular_uniform, dir_light.specular.r, dir_light.specular.g, dir_light.specular.b);
		});

		// Spot lights
		manager.forEntitiesMatching<Lemur::SSpotLight>([&](auto entity_index, auto& position, auto& spot_light)
		{
			int spot_light_position_uniform = prog.getUniformLocation("spot_light.position");
			int spot_light_direction_uniform = prog.getUniformLocation("spot_light.direction");
			int spot_light_cut_off_uniform = prog.getUniformLocation("spot_light.cut_off");
			int spot_light_outer_cut_off_uniform = prog.getUniformLocation("spot_light.outer_cut_off");
			int spot_light_constant_uniform = prog.getUniformLocation("spot_light.constant");
			int spot_light_linear_uniform = prog.getUniformLocation("spot_light.linear");
			int spot_light_quadratic_uniform = prog.getUniformLocation("spot_light.quadratic");
			int spot_light_ambient_uniform = prog.getUniformLocation("spot_light.ambient");
			int spot_light_diffuse_uniform = prog.getUniformLocation("spot_light.diffuse");
			int spot_light_specular_uniform = prog.getUniformLocation("spot_light.specular");

			glUniform3f(spot_light_position_uniform, camera.getCenter().x, camera.getCenter().y, camera.getCenter().z);
			glUniform3f(spot_light_direction_uniform, camera.getDirection().x, camera.getDirection().y, camera.getDirection().z);
			glUniform1f(spot_light_cut_off_uniform, spot_light.cut_off);
			glUniform1f(spot_light_outer_cut_off_uniform, spot_light.outer_cut_off);
			glUniform1f(spot_light_constant_uniform, spot_light.constant);
			glUniform1f(spot_light_linear_uniform, spot_light.linear);
			glUniform1f(spot_light_quadratic_uniform, spot_light.quadratic);
			glUniform3f(spot_light_ambient_uniform, spot_light.ambient.r, spot_light.ambient.g, spot_light.ambient.b);
			glUniform3f(spot_light_diffuse_uniform, spot_light.diffuse.r, spot_light.diffuse.g, spot_light.diffuse.b);
			glUniform3f(spot_light_specular_uniform, spot_light.specular.r, spot_light.specular.g, spot_light.specular.b);
		});

		// Identity matrix
		model = glm::mat4(1.0f);

		model = lm::translate(model, lm::vec3(pos.x, pos.y, pos.z));
		int model_uniform = prog.getUniformLocation("model");
		glUniformMatrix4fv(model_uniform, 1, GL_FALSE, lm::value_ptr(model));

		vb.render(GL_TRIANGLES);

		// Unbind program
		glUseProgram(NULL);
	}
}