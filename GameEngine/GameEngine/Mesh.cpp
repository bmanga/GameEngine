#include "Mesh.h"
#include <tuple>


Mesh::Mesh()
{
}


Mesh::~Mesh()
{
}

void Mesh::setMeshData(MeshData&& data)
{
	std::tie(m_vertex_data, m_texture_data, m_normal_data,
	m_vertex_indices, m_texture_indices, m_normal_indices) = 
		std::move(data);
}

void* Mesh::vertexBuffer()
{
	return m_vertex_data.data();
}

u32 Mesh::vertexCount() const
{
	return (u32)m_vertex_data.size();
}
