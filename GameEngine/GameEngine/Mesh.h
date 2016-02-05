#pragma once
#include <vector>

#include "math.h"
#include "types.h"

using std::vector;
using Lemur::math::vec3;
using Lemur::math::vec2;
using Lemur::MeshData;
using Lemur::u32;
class Mesh
{
public:
	Mesh();
	~Mesh();

	void setMeshData(MeshData&& data);

	void* vertexBuffer();

	u32 vertexCount() const;

	size_t vertexBufferSize() const
	{
		return vertexCount() * 
			sizeof(typename decltype(m_vertex_data)::value_type);
	}

	void* vertexIndexBuffer()
	{
		return m_vertex_indices.data();
	}

	u32 vertexIndexCount() const
	{
		return (u32)m_vertex_indices.size();
	}

	size_t vertexIndexBufferSize() const
	{
		return vertexIndexCount() *
			sizeof(typename decltype(m_vertex_indices)::value_type);
	}
private:
	vector<vec3> m_vertex_data;
	vector<vec2> m_texture_data;
	vector<vec3> m_normal_data;

	vector<u32> m_vertex_indices;
	vector<u32> m_texture_indices;
	vector<u32> m_normal_indices;
};

