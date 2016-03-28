#pragma once

#include "types.h"
#include "Mesh.h"
#include <unordered_map>
#include "BufferObject.h"
#include <string>

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

	IndexedMesh indexMesh(Mesh&& mesh);


	const Mesh* loadMesh(const char* name);

	std::unordered_map<std::string, Mesh> m_meshes;
};


}
