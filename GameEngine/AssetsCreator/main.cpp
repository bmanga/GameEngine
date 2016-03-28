#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <vector>
#include <iostream>
#include <memory>

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
	std::vector<aiVector3D> vertices;
	std::vector<aiVector3D> normals;
	std::vector<aiVector2D> textureCoords;
};


Mesh AssimpLoadMesh(const char* filename)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals);

	if(!scene)
	{
		std::cout << "no file found!" << std::endl;
		return{};
	}

	const aiMesh* mesh = scene->mMeshes[0];
	MeshBufferHeader info;
	info.vertex_count = mesh->mNumVertices;
	info.has_normals = mesh->HasNormals();
	info.has_texture_coords = mesh->HasTextureCoords(0);

	//calculate the index count, assuming triangular faces;	
	info.index_count = mesh->mNumFaces * 3;
	
	info.index_underlying_type = 2; //underlying type is GL_UNSIGNED_INT

	std::unique_ptr<char[]> vertexBuffer, normalBuffer, textcoodBuffer, indexBuffer;

	vertexBuffer = info.vertex_count 
		? std::unique_ptr<char[]>(new char[info.vertex_count * sizeof(aiVector3D)] )
		: nullptr;


	
}
int main()
{
	int x = 0;
	std::cout << x;
	;
}