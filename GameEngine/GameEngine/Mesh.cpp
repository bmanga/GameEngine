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
		ConsoleLogger::Error(CODE_LOCATION, cstr(filename, " not found"));
		return;
	}

	
	//Now that we know the sizes, create the buffer
	BinaryFile.read(reinterpret_cast<char*>(&m_vertex_buffer_size), sizeof(u32));
	BinaryFile.read(reinterpret_cast<char*>(&m_index_buffer_size), sizeof(u32));
	
	m_vertex_buffer = std::unique_ptr<char[]>(new char[m_vertex_buffer_size]);
	m_index_buffer = std::unique_ptr<unsigned int[]>(new unsigned int[m_index_buffer_size/4]);

	BinaryFile.read(m_vertex_buffer.get(), m_vertex_buffer_size);
	BinaryFile.read((char*)m_index_buffer.get(), m_index_buffer_size);
	std::cout << m_index_buffer[33];
	BinaryFile.read(reinterpret_cast<char*>(&m_buffers_info),
		sizeof(MeshBufferHeader));
	BinaryFile.close();
}

void* Mesh::vertexBuffer() const
{
	return m_vertex_buffer.get();
}

u32 Mesh::vertexCount() const
{
	return m_buffers_info.vertex_count;
}

size_t Mesh::vertexBufferSize() const
{
	return m_vertex_buffer_size;
}



unsigned int* Mesh::indexBuffer() const
{
	return m_index_buffer.get();
}

u32 Mesh::indexBufferUnderlyingType() const
{
	static constexpr u32 underlying_types[] = {
		GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT };

	return underlying_types[m_buffers_info.index_underlying_type];

}

u32 Mesh::indexCount() const
{
	return m_buffers_info.index_count;
}

size_t Mesh::indexBufferSize() const
{
	return m_index_buffer_size;
}


Mesh::Mesh(const char* filename)
{
	loadFromFile(filename);
}

}//namespace Lemur
