#include "RenderSystem.h"

#include <unordered_map>
#include <vector>
#include <string>
#include "VertexBuffer.h"
#include "Mesh.h"
#include "ShaderProgram.h"
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
			std::string texture = "in_texcoord:2f";
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
		using namespace Lemur;

		auto& vb = vbs[i];
		CRenderable renderable = renderables.at(i);
		ShaderProgram prog = *renderable.material->shader.get();
		CPosition pos = positions.at(i);

		prog.use();

		glUniform3f("view_pos"_uniform, 
			camera.getCenter().x, 
			camera.getCenter().y, 
			camera.getCenter().z);

		glUniformMatrix4fv("view"_uniform, 1, GL_FALSE,
			glm::value_ptr(camera.getView()));

		// Create the perspective projection matrix
		lm::mat4 proj = camera.getProjection();
		glUniformMatrix4fv("proj"_uniform, 1, GL_FALSE, glm::value_ptr(proj));

		glUniform1f("material.shininess"_uniform, renderable.material->shininess);

		glUniform1f("material.transparency"_uniform, renderable.material->transparency);

		if (renderable.material->use_texturing)
		{ 
			auto& mat = renderable.material;
#if 1
			if (mat->texture)
			{
				renderable.material->texture->bind(0);
			}
			if (mat->bump_map)
			{
				renderable.material->bump_map->bind(1);
			}
			if (mat->normal_map)
			{
				renderable.material->normal_map->bind(2);
			}
			if (mat->specular_map)
			{
				renderable.material->specular_map->bind(3);
			}
			if (mat->mask_map)
			{
				renderable.material->mask_map->bind(4);
			}
			glActiveTexture(GL_TEXTURE0);
#endif
			glUniform1i("material.texture"_uniform, 0);
			glUniform1i("material.diffuse_map"_uniform, 1);
			glUniform1i("material.normal_map"_uniform, 2);
			glUniform1i("material.specular_map"_uniform, 3);
			glUniform1i("material.mask_map"_uniform, 4);
			glUniform1i("mysampler"_uniform, 0);

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
		glUniformMatrix4fv("model"_uniform, 1, GL_FALSE, lm::value_ptr(model));

		vb.render(GL_TRIANGLES);

		// Unbind program
		glUseProgram(NULL);
	}
}