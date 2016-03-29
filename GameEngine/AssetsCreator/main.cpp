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
	std::unique_ptr<char[]> buffer;
	u32 buffer_size;
};


Mesh AssimpLoadMesh(const char* filename)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile( filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals);

	if(!scene)
	{
		std::cout << "no file found!" << std::endl;
		return{{}, nullptr};
	}

	const aiMesh* mesh = scene->mMeshes[0];
	MeshBufferHeader info;
	info.vertex_count = mesh->mNumVertices;
	info.has_normals = mesh->HasNormals();
	info.has_texture_coords = mesh->HasTextureCoords(0);

	//calculate the index count, assuming triangular faces;	
	info.index_count = mesh->mNumFaces * 3;
	
	size_t indexBufferSize = info.index_count * 4;

	info.index_underlying_type = 2; //underlying type is GL_UNSIGNED_INT

	//create the buffers
	std::unique_ptr<char[]> buffer;

	size_t vertexBufferSize = info.vertex_count * sizeof(aiVector3D);
	size_t normalBufferSize = info.has_normals
		? info.vertex_count * sizeof(aiVector3D)
		: 0;

	size_t textcoordsBufferSize = info.has_texture_coords
		? info.vertex_count * sizeof(aiVector2D)
		: 0;

	u32 bufferSize = vertexBufferSize + normalBufferSize
		+ textcoordsBufferSize + indexBufferSize;
	buffer = std::unique_ptr<char[]>(new char[bufferSize]);

	//fill in the buffer

	//indices
	for (int j = 0; j < mesh->mNumFaces; ++j)
	{
		aiFace face = mesh->mFaces[j];
		//force size to 4 bytes per index
		uint32_t indices[3] = { face.mIndices[0], face.mIndices[1], face.mIndices[2] };
		
		memcpy(&(buffer.get()[12 * j]), indices, 12);

	}

	auto deleteme = buffer.get();
	//vertices
	for (int j = 0; j < info.vertex_count; ++j)
	{
		size_t index = sizeof(aiVector3D) * j + indexBufferSize;
		memcpy(&buffer[index], &mesh->mVertices[j], sizeof(aiVector3D));
	}
	
	//normals
	for (int j = 0; j < info.vertex_count; ++j)
	{
		if (!info.has_normals) break;

		size_t index = sizeof(aiVector3D) * j + indexBufferSize + vertexBufferSize;
		memcpy(&buffer[index],
			&mesh->mNormals[j],
			sizeof(aiVector3D));
	}

	//textcoords
	for (int j = 0; j < info.vertex_count; ++j)
	{
		if (!info.has_texture_coords) break;

		size_t index = sizeof(aiVector2D) * j + indexBufferSize + vertexBufferSize 
			+ normalBufferSize;
		auto texture = mesh->mTextureCoords[0][j];
		aiVector2D texture2d(texture.x, texture.y);
		memcpy(&buffer[index],
			&texture2d,
			sizeof(aiVector2D));
	}

	return{ info, std::move(buffer), bufferSize };
}


void DumpMeshToFile(const Mesh& mesh, const char* filename)
{
	std::experimental::filesystem::path p = "(../assets/mesh)";
	std::fstream binOut(filename, std::ios::out | std::ios::binary);

	binOut.write((char*)&mesh.info, sizeof(MeshBufferHeader));
	binOut.write((char*)&mesh.buffer_size, sizeof(u32));

	binOut.write(mesh.buffer.get(), mesh.buffer_size);
}
int main()
{
	std::cout << "[model to load] [output file]: ";
	std::string model, result;
	std::cin >> model >> result;
	auto mesh = AssimpLoadMesh(model.c_str());
	DumpMeshToFile(mesh, result.c_str());
	if (!mesh.buffer_size)
	{
		std::cout << "warning, no model found\n";
		std::string s;
		std::cin >> s;
	}
}