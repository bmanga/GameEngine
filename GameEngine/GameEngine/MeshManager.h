#pragma once

#include "types.h"
#include "Mesh.h"
#include <unordered_map>
#include "BufferObject.h"


namespace Lemur{

struct IndexedMesh
{
	VertexBufferObject vbo;
	u32 offset;
	IndexBufferObject ibo;
};

class MeshManager
{
public:
	MeshManager();
	~MeshManager();



	const Mesh* loadMesh(const char* name);
	const Mesh* loadMesh(const std::string& name);
	std::unordered_map<std::string, Mesh> m_meshes;
};


}
