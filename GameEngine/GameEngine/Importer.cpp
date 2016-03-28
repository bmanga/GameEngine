#include "Importer.h"

#include <fstream>
#include "types.h"
#include "math.h"
#include "TaskExecutor.h"
#include <string>
#include <sstream>
#include <iterator>
#include <regex>
#include <map>
#include <unordered_map>
#include <set>
#include "Lemur.h"
#include <assimp\Importer.hpp>

std::string DEFAULT_LOC = R"(../assets/mesh/)";
using namespace Lemur;
using namespace math;

void parse_vertices(std::vector<vec3>& vertices, std::istringstream& data);


void parse_material_lib();


void parse_texture_coords(std::vector<vec2>& normalData, std::istringstream& data);


void parse_vertex_normals(std::vector<vec3>& normalData, std::istringstream& data);

void equalize_vertices_to_normals(std::vector<vec3>& vertices,
	std::vector<vec3>& normals,
	std::vector<Lemur::u32>& v_indices,
	std::vector<Lemur::u32>& n_indices);

enum ObjFaceElements
{
	UNKNOWN,
	VERTEX,
	VERTEX_TEXTURE,
	VERTEX_NORMAL,
	VERTEX_TEXTURE_NORMAL
};

ObjFaceElements get_index_elements(const std::string& str);

void parse_face_indices(ObjFaceElements elements, std::vector<Lemur::u32>& vertexi, std::vector<Lemur::u32>& texturei, std::vector<Lemur::u32>& normali, std::istringstream& data);

void sort_normal_data_by_vertex_index(std::vector<vec3>& normals,
	std::vector<Lemur::u32>& v_index,
	std::vector<Lemur::u32>& n_index);

MeshData load_obj(const char* path)
{
	ObjFaceElements face_elems = UNKNOWN;
	std::vector<vec3> vertex_data, normal_data;
	std::vector<vec2> texture_data;
	std::vector<Lemur::u32> vertex_indices, normal_indices, texture_indices;

	auto fpath = DEFAULT_LOC + path;
	std::ifstream file(fpath);

	if (!file) printf("could not find file %s", fpath.c_str());
	//TODO: implement if things go wrong

	std::string data;

	while(getline(file, data))
	{
		std::istringstream stream(data);

		std::string header = *std::istream_iterator<std::string>(stream);
		
		//parse the basics
		if (header == "v") parse_vertices(vertex_data, stream);
		else if (header == "vt") parse_texture_coords(texture_data, stream);
		else if (header == "vn") parse_vertex_normals(normal_data, stream);
		else if (header == "f")
		{
			if (face_elems == UNKNOWN)
			{
				face_elems = get_index_elements(data);
			}

			parse_face_indices(face_elems, 
				vertex_indices, 
				texture_indices, 
				normal_indices,
				stream);
		}

		//ignore the rest for now;
		else continue;

		//if (header == "mtlib") parse_material_lib();
	}

	//We need as many vertices as normals
	if (face_elems == VERTEX_TEXTURE_NORMAL || face_elems == VERTEX_NORMAL)
	{
		//if (vertex_data.size() != normal_data.size())

		{
			equalize_vertices_to_normals(
				vertex_data,
				normal_data,
				vertex_indices,
				normal_indices
			);

			
		}
		sort_normal_data_by_vertex_index(normal_data, vertex_indices, normal_indices);
	}
	return MeshData { vertex_data, texture_data, normal_data,
		vertex_indices, texture_indices, normal_indices };
}


inline void parse_vertices(std::vector<vec3>& vertices, std::istringstream& data)
{
	float x, y, z;
	data >> x >> y >> z;
	vertices.emplace_back( x, y, z );
}

void parse_material_lib()
{
}

void parse_texture_coords(std::vector<vec2>& normalData, std::istringstream& data)
{
	float u, v;
	data >> u >> v;
	normalData.emplace_back(u, v);
}

void parse_vertex_normals(std::vector<vec3>& normalData, std::istringstream& data)
{
	float x, y, z;
	data >> x >> y >> z;

	normalData.push_back(math::normalize(vec3{x, y, z}));
}

void equalize_vertices_to_normals(std::vector<vec3>& vertices, std::vector<vec3>& normals,
	std::vector<Lemur::u32>& v_indices, std::vector<Lemur::u32>& n_indices)
{
	using u32Pair = std::pair<Lemur::u32, Lemur::u32>;

	std::vector<u32Pair> unique_pairs;
	unique_pairs.reserve(v_indices.size());

	for (size_t index = 0u; index < v_indices.size(); ++index)
	{
		u32Pair vi_ni(v_indices[index], n_indices[index]);

		auto it = std::find_if(unique_pairs.begin(), unique_pairs.end(), [&vi_ni](const u32Pair& p)
		{
			bool a = p.first == vi_ni.first;
			bool b = p.second == vi_ni.second;
			return (!a && b) || a;
		});


		while (it != unique_pairs.end())
		{ //The pair shares either of the values or neither (not both)
			it = std::find_if(unique_pairs.begin(), unique_pairs.end(), [&vi_ni](const u32Pair& p)
			{
				bool a = p.first == vi_ni.first;
				bool b = p.second == vi_ni.second;
				return (!a && b) || a;
			});
			if (it->first == vi_ni.first)
			{//They share the same vertex. Create a new one

				auto copy = vertices[it->first];
				Lemur::u32 new_index = vertices.size();
				vertices.push_back(std::move(copy));

				//update vi_ni
				vi_ni.first = new_index;
			}

			it = std::find_if(unique_pairs.begin(), unique_pairs.end(), [&vi_ni](const u32Pair& p)
			{
				bool a = p.first == vi_ni.first;
				bool b = p.second == vi_ni.second;
				return (!a && b) || a;
			});
			if (it->second == vi_ni.second)
			{//they share the same normal. Create a new one

				auto copy = normals[it->second];
				Lemur::u32 new_index = normals.size();
				normals.push_back(std::move(copy));

				//update vi_ni
				vi_ni.second = new_index;
			}


		}


		//Add the pair
		unique_pairs.push_back(vi_ni);

		//Update the indices
		v_indices[index] = vi_ni.first;
		n_indices[index] = vi_ni.second;
	}
}

void parse_face_indices(ObjFaceElements elements, 
	std::vector<Lemur::u32>& vertexi, 
	std::vector<Lemur::u32>& texturei, 
	std::vector<Lemur::u32>& normali,
	std::istringstream& data)
{
	Lemur::u32 v = 0, t = 0, n = 0;

	std::string vdata;

	while (data >> vdata)
	{
		if (elements == VERTEX_NORMAL)
		{
			sscanf_s(vdata.c_str(), "%d//%d", &v, &n);
			vertexi.push_back(v - 1);
			normali.push_back(n - 1);
		}
		else if (elements == VERTEX_TEXTURE_NORMAL)
		{
			sscanf_s(vdata.c_str(), "%d/%d/%d", &v, &t, &n);
			vertexi.push_back(v - 1);
			texturei.push_back(v - 1);
			normali.push_back(n - 1);
		}
	}
}

void sort_normal_data_by_vertex_index(std::vector<vec3>& normals,
	std::vector<Lemur::u32>& v_indices,
	std::vector<Lemur::u32>& n_indices)
{
#if 0
	Lemur::vector<Lemur::u32> unique_v_indices(v_indices);
	std::sort(unique_v_indices.begin(), unique_v_indices.end());
	unique_v_indices.erase(std::unique(unique_v_indices.begin(), unique_v_indices.end()), unique_v_indices.end());
	ASSERT_CLERROR(unique_v_indices.size() == normals.size(), CODE_LOCATION, "something is very wrong")


	struct tmp_pair
	{
		vec3 normal;
		Lemur::u32 index;
	};

	Lemur::vector<tmp_pair> pairs(normals.size());

	
}
#else

	auto index = 0u;
	auto vi_it = v_indices.begin();
	auto ni_it = n_indices.begin();

	std::vector<vec3> sorted_vector(normals);

	for (; index != v_indices.size() && index != n_indices.size(); ++index)
	{		

		sorted_vector[n_indices[index]] = normals[v_indices[index]];
		n_indices[index] = v_indices[index];
	}

	normals = sorted_vector;
}
#endif
ObjFaceElements get_index_elements(const std::string& str)
{
	using namespace std;

	std::regex vertex_texture_normal(R"([0-9]+/[0-9]+/[0-9]+)");
	std::regex vertex_texture(R"([0-9]+/[0-9]+)");
	std::regex vertex_normal(R"([0-9]+//[0-9]+)");
	
	smatch m_vtn, m_vt, m_vn;

	regex_search(str, m_vtn, vertex_texture_normal);
	regex_search(str, m_vt, vertex_texture);
	regex_search(str, m_vn, vertex_normal);
	if (m_vtn.size() != 0) return VERTEX_TEXTURE_NORMAL;
	else if (m_vt.size() != 0) return VERTEX_TEXTURE;
	else if (m_vn.size() != 0) return VERTEX_NORMAL;
	else return UNKNOWN;
}
