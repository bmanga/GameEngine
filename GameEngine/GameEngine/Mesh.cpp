#include "Mesh.h"
#include <tuple>
#include <fstream>

Mesh::Mesh()
{
}


Mesh::~Mesh()
{
}

void Mesh::dumpToFile(const std::string& str)
{
	struct BuffersInfo
	{
		u32 vertex_count;
		u32 normal_count;
		u32 textcoords_count;
		u32 index_count : 30;
		u32 index_underlying_type : 2;

	};

	using namespace std;
	m_vertex_data.shrink_to_fit();
	m_texture_data.shrink_to_fit();
	m_normal_data.shrink_to_fit();
	m_vertex_indices.shrink_to_fit();

	fstream binOut(str, ios::out | ios::binary);
	BuffersInfo info{
		vertexCount(),
		normalCount(),
		m_texture_data.size(),
		vertexIndexCount(),
		2u
	};
	binOut.write((char*)&info, sizeof(BuffersInfo));
	binOut.write((char*)vertexBuffer(), vertexBufferSize());
	binOut.write((char*)normalBuffer(), normalBufferSize());
	binOut.write((char*)m_texture_data.data(), m_texture_data.size() * sizeof(vec2));
	binOut.write((char*)vertexIndexBuffer(), vertexIndexBufferSize());
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

size_t Mesh::vertexBufferSize() const
{
	return vertexCount() * 
		sizeof(typename decltype(m_vertex_data)::value_type);
}

void* Mesh::vertexIndexBuffer()
{
	return m_vertex_indices.data();
}

u32 Mesh::vertexIndexCount() const
{
	return (u32)m_vertex_indices.size();
}

size_t Mesh::vertexIndexBufferSize() const
{
	return vertexIndexCount() *
		sizeof(typename decltype(m_vertex_indices)::value_type);
}

void* Mesh::normalBuffer()
{
	return m_normal_data.data();
}

u32 Mesh::normalCount() const
{
	return (u32)m_normal_data.size();
}

size_t Mesh::normalBufferSize() const
{
	return normalCount() *
		sizeof(typename decltype(m_normal_indices)::value_type);
}