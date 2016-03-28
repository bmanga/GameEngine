#include "Mesh_v2.h"
#include <fstream>
#include <filesystem>
#include "types.h"
#include <GL/glew.h>

using namespace Lemur;
namespace todo {
static const std::experimental::filesystem::path BASEPATH("/../assets/mesh");

Mesh::Mesh()
{
}


Mesh::~Mesh()
{
}

void Mesh::loadFromFile(const char* filename)
{
	using namespace std;
	fstream BinaryFile(BASEPATH / filename, ios::in | ios::binary);
	BinaryFile.read(reinterpret_cast<char*>(&m_buffers_info), sizeof(BuffersInfo));
	
	//Now that we know the sizes, create the buffer
	size_t BufferSize = vertexBufferSize() + normalBufferSize() +
		textcoordsBufferSize() + vertexIndexBufferSize();

	
	m_buffer = std::unique_ptr<char[]>(new char[BufferSize]);

	BinaryFile.read(m_buffer.get(), BufferSize);
	BinaryFile.close();
}

void* Mesh::vertexBuffer() const
{
	return m_buffer.get();
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
	return m_buffers_info.normal_count * sizeof(Lemur::vec2);
}

	void* Mesh::vertexIndexBuffer() const
{
	return m_buffer.get() + vertexBufferSize() + normalBufferSize() + textcoordsBufferSize();
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
	return m_buffer.get() + vertexBufferSize();
}

u32 Mesh::normalCount() const
{
	return m_buffers_info.normal_count;
}

size_t Mesh::normalBufferSize() const
{
	return m_buffers_info.normal_count * sizeof(vec3);
}
}
