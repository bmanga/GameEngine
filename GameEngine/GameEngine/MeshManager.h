#pragma once

#include "types.h"
#include "Mesh.h"
#include <map>

namespace Lemur{

struct IndexedMesh
{
	i32 vbo;
	i32 offset;
	i32 ibo;
};
class MeshManager
{
public:
	MeshManager();
	~MeshManager();

	IndexedMesh indexMesh(Mesh&& mesh)
	{
		
	}

	//std::map<string, IndexedMesh>
};


}
