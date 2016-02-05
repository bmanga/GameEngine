#include "Importer.h"

#include <fstream>
#include "types.h"
#include "math.h"
#include "TaskExecutor.h"
#include <string>
#include <sstream>
#include <iterator>
#include <regex>

using namespace Lemur;
using namespace math;

void parse_vertices(std::vector<vec3>& vertices, std::istringstream& data);


void parse_material_lib();


void parse_texture_coords(std::vector<vec2>& normalData, std::istringstream& data);


void parse_vertex_normals(std::vector<vec3>& normalData, std::istringstream& data);



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

MeshData load_obj(const char* path)
{

	using namespace std;

	ObjFaceElements face_elems = UNKNOWN;
	vector<vec3> vertex_data, normal_data;
	vector<vec2> texture_data;
	vector<Lemur::u32> vertex_indices, normal_indices, texture_indices;

	ifstream file(path);

	//TODO: implement if things go wrong

	string data;

	while(getline(file, data))
	{
		istringstream stream(data);

		string header = *istream_iterator<string>(stream);
		
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
	normalData.emplace_back(x, y, z);
}

void parse_face_indices(ObjFaceElements elements, std::vector<Lemur::u32>& vertexi, std::vector<Lemur::u32>& texturei, std::vector<Lemur::u32>& normali, std::istringstream& data)
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
	}
}

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
