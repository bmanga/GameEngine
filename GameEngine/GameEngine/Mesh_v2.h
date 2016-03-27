#pragma once
#include <vector>
#include <filesystem>
#include "math.h"
#include "types.h"
#include <gl/glew.h>
using std::vector;
using Lemur::math::vec3;
using Lemur::math::vec2;
using Lemur::MeshData;
using Lemur::u32;
using Lemur::u8;
using Lemur::i8;

namespace fs = std::experimental::filesystem;

namespace todo {

namespace impl
{

}//impl


class Mesh
{
	struct BuffersInfo
	{
		u32 vertex_count;
		u32 normal_count;
		u32 textcoords_count;
		u32 index_count : 30;
		u32 index_underlying_type : 2;

	};
	static_assert(sizeof(BuffersInfo) == 4 * 4, "");
	//TODO(bmanga): could use offsetof instead of forcing size. Maybe later
	
public:
	Mesh();
	~Mesh();

	void loadFromFile(const char* filename);

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
private:
	BuffersInfo m_buffers_info;

	// Memory Layout of m_buffer is:
	// -vertex buffer data (vbo)
	// -normal buffer data
	// -textcoords buffer data
	// -index buffer data (ibo)
	std::unique_ptr<char[]> m_buffer;
};

}