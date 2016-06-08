#pragma once
#include <memory>
#include "types.h"

namespace Lemur {

class Mesh
{
public:
	//TODO: maybe use bitfileds? Not sure if it is worth the time
	struct MeshBufferHeader
	{
		u32 vertex_count;
		u32 index_count;

		bool has_normals;
		bool has_texture_coords;
		u8  index_underlying_type;

	};
	static_assert(sizeof(MeshBufferHeader) == 4 * 3, "");
	//TODO(bmanga): could use offsetof instead of forcing size. Maybe later
	


	void* vertexBuffer() const;
	unsigned int* indexBuffer() const;

	u32 vertexCount() const;
	u32 indexCount() const;

	size_t vertexBufferSize() const;
	size_t indexBufferSize() const;

	u32 indexBufferUnderlyingType() const;

	const MeshBufferHeader& bufferHeader() const
	{
		return m_buffers_info;
	}

	Mesh() {};
	Mesh(const char* filename);
private:
	void loadFromFile(const char* filename);
	MeshBufferHeader m_buffers_info;

	// Memory Layout of m_vertex_buffer is:
	// -vertex buffer data (vbo)
	// -normal buffer data
	// -textcoords buffer data
	std::unique_ptr<char[]> m_vertex_buffer;
	std::unique_ptr<unsigned int[]> m_index_buffer;

	u32 m_vertex_buffer_size = 0;
	u32 m_index_buffer_size = 0;
};

}// namespace Lemur