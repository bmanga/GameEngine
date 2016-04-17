#include "Mesh.h"
#include <fstream>
#include <filesystem>
#include "Types.h"
#include <GL/glew.h>
#include "Lemur.h"
#include "FilePaths.h"

namespace Lemur {


void Mesh::loadFromFile(const char* filename)
{
	using namespace std;
	fstream BinaryFile(MESH_PATH/filename, ios::in | ios::binary);
	if (!BinaryFile)
	{
		ConsoleLogger::Error(CODE_LOCATION, CSTR(filename, " not found"));
		return;
	}
	BinaryFile.read(reinterpret_cast<char*>(&m_buffers_info), sizeof(MeshBufferHeader));
	
	//Now that we know the sizes, create the buffer
	u32 BufferSize;
	BinaryFile.read(reinterpret_cast<char*>(&BufferSize), sizeof(u32));

	
	m_buffer = std::unique_ptr<char[]>(new char[BufferSize]);

	BinaryFile.read(m_buffer.get(), BufferSize);
	BinaryFile.close();
}

void* Mesh::vertexBuffer() const
{
	return m_buffer.get() + vertexIndexBufferSize();
}

u32 Mesh::vertexCount() const
{
	return m_buffers_info.vertex_count;
}

size_t Mesh::vertexBufferSize() const
{
	return m_buffers_info.vertex_count * sizeof(Lemur::vec3);
}

size_t Mesh::textcoordsBufferSize() const
{
	return m_buffers_info.has_normals
		? vertexCount() * sizeof(Lemur::vec2)
		: 0;
}

	void* Mesh::vertexIndexBuffer() const
{
	return m_buffer.get();
}

u32 Mesh::vertexIndexBufferUnderlyingType() const
{
	static constexpr u32 underlying_types[] = {
		GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT };

	return underlying_types[m_buffers_info.index_underlying_type];

}

u32 Mesh::vertexIndexCount() const
{
	return m_buffers_info.index_count;
}

size_t Mesh::vertexIndexBufferSize() const
{
	static constexpr u8 underlying_sizes[] = { 1, 2, 4 };

	return m_buffers_info.index_count * 
		underlying_sizes[m_buffers_info.index_underlying_type];
}

void* Mesh::normalBuffer() const
{
	return m_buffer.get() + vertexIndexBufferSize() + vertexBufferSize();
}

u32 Mesh::normalCount() const
{
	return m_buffers_info.has_normals
		? vertexCount() 
		: 0;
}

size_t Mesh::normalBufferSize() const
{
	return normalCount() * sizeof(vec3);
}

Mesh::Mesh(const char* filename)
{
	loadFromFile(filename);
}

}//namespace Lemur
