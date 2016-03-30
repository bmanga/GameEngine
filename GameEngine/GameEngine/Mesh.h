#pragma once
#include <memory>
#include "types.h"

namespace Lemur {

class Mesh
{
	
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
	
public:

	void* vertexBuffer() const;

	u32 vertexCount() const;

	size_t vertexBufferSize() const;

	size_t textcoordsBufferSize() const;

	void* vertexIndexBuffer() const;

	u32 vertexIndexBufferUnderlyingType() const;

	u32 vertexIndexCount() const;

	size_t vertexIndexBufferSize() const;

	void* normalBuffer() const;

	u32 normalCount() const;

	size_t normalBufferSize() const;

	Mesh() {};
	Mesh(const char* filename);
private:
	void loadFromFile(const char* filename);
	MeshBufferHeader m_buffers_info;

	// Memory Layout of m_buffer is:
	// -index buffer data (ibo)
	// -vertex buffer data (vbo)
	// -normal buffer data
	// -textcoords buffer data
	std::unique_ptr<char[]> m_buffer;
};

}// namespace Lemur