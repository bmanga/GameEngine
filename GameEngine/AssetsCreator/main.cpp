#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <filesystem>

using namespace std::experimental;
using u32 = uint32_t;
using u8 = uint8_t;
struct MeshBufferHeader
{
	u32 vertex_count;
	u32 index_count;

	bool has_normals;
	bool has_texture_coords;
	u8  index_underlying_type;

};
struct Mesh
{
	MeshBufferHeader info;
	std::unique_ptr<char[]> vertex_buffer;
	std::unique_ptr<unsigned int[]> index_buffer;
	u32 vertex_buffer_size;
	u32 index_buffer_size;
};

std::ostream& operator<< (std::ostream& out, const Mesh& mesh)
{
	out << "\nMeshBufferHeader:";
	out << "\n\t vertex count: " << mesh.info.vertex_count;
	out << "\n\t index count: " << mesh.info.index_count;
	out << "\n\t has normals: " << mesh.info.has_normals;
	out << "\n\t has texture coords: " << mesh.info.has_texture_coords;
	out << "\nvertex buffer size: " << mesh.vertex_buffer_size;
	out << "\nindex buffer size: " << mesh.index_buffer_size;

	return out << "\n";
}

Mesh AssimpLoadMesh(const char* filename)
{
	std::experimental::filesystem::path p = "../assets/mesh/unprocessed";
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile((p/filename).generic_string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals);

	if(!scene)
	{
		std::cout << "no file found!" << std::endl;
		return{{}, nullptr};
	}

	const aiMesh* mesh = scene->mMeshes[0];
	MeshBufferHeader info;
	info.vertex_count = mesh->mNumVertices;
	std::cout << "num of vertices : " << mesh->mNumVertices << "\n";
	info.has_normals = mesh->HasNormals();
	info.has_texture_coords = mesh->HasTextureCoords(0);

	//calculate the index count, assuming triangular faces;	
	info.index_count = mesh->mNumFaces * 3;
	
	size_t indexBufferSize = info.index_count * 4;

	info.index_underlying_type = 2; //underlying type is GL_UNSIGNED_INT



	//vertex coords
	size_t vertexBufferSize = info.vertex_count * sizeof(aiVector3D);
	//+ normals
	vertexBufferSize += info.has_normals
		? info.vertex_count * sizeof(aiVector3D)
		: 0;
	//+ texture coords
	vertexBufferSize  += info.has_texture_coords
		? info.vertex_count * sizeof(aiVector2D)
		: 0;


	std::unique_ptr<char[]> vertex_buffer(new char[vertexBufferSize]);
	std::unique_ptr<unsigned int[]> index_buffer(new unsigned int[info.index_count]);


	//fill in the buffer

	//indices
	for (unsigned j = 0; j < mesh->mNumFaces; ++j)
	{
		aiFace face = mesh->mFaces[j];
		//force size to 4 bytes per index
		uint32_t indices[3] = { face.mIndices[0], face.mIndices[1], face.mIndices[2] };
		index_buffer[3 * j];
		memcpy((char*)&(index_buffer[3 * j]), indices, 12);

	}

	//vertices
	size_t index = 0u;
	for (unsigned j = 0; j < info.vertex_count; ++j)
	{
		//vertex position data
		memcpy(&vertex_buffer[index], &mesh->mVertices[j], sizeof(aiVector3D));
		index += sizeof(aiVector3D);

		if(info.has_normals)
		{
			memcpy(&vertex_buffer[index],&mesh->mNormals[j],sizeof(aiVector3D));
			
			index += sizeof(aiVector3D);
		}

		if(info.has_texture_coords)
		{
			auto texture = mesh->mTextureCoords[0][j];
			aiVector2D texture2d(texture.x, texture.y);
			memcpy(&vertex_buffer[index],
				&texture2d,
				sizeof(aiVector2D));
			
			index += sizeof(aiVector2D);
		}
	}

	return
	{ 
		info, std::move(vertex_buffer), std::move(index_buffer),
		vertexBufferSize, indexBufferSize 
	};
}


void DumpMeshToFile(const Mesh& mesh, const char* filename)
{
	std::experimental::filesystem::path p = "../assets/mesh";
	//if (p.has_filename()) std::cout << "hellooo\n";
	std::cout << p.root_name();
	std::fstream binOut(p/filename, std::ios::out | std::ios::binary);

	binOut.write((char*)&mesh.vertex_buffer_size, sizeof(u32));
	binOut.write((char*)&mesh.index_buffer_size, sizeof(u32));
	binOut.write(mesh.vertex_buffer.get(), mesh.vertex_buffer_size);
	binOut.write((const char*)mesh.index_buffer.get(), mesh.index_buffer_size);
	binOut.write((char*)&mesh.info, sizeof(MeshBufferHeader));
}
int main()
{
	std::cout << "Model to load must be located is 'assets/mesh/unprocessed'\n"
		         "output model will be located in 'assets/mesh'\n";
	std::cout << "[model to load] [output file]: ";
	std::string model, result;
	std::cin >> model >> result;
	auto mesh = AssimpLoadMesh(model.c_str());
	DumpMeshToFile(mesh, result.c_str());
	std::cout << mesh;
	std::cout << "press q to quit: ";
	do
	{
		std::cin >> model;
	} while (model != "q");
}