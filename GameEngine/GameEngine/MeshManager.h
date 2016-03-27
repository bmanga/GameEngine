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

	IndexedMesh indexMesh(Mesh&& mesh);


	IndexedMesh loadMesh(const char* name);

	std::unordered_map<string, IndexedMesh> m_meshes;
};


}