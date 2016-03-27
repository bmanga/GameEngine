#pragma once
#include <gl/glew.h>


enum class DrawHint : unsigned
{
	STATIC = GL_STATIC_DRAW,
	DYNAMIC = GL_DYNAMIC_DRAW
};

template <unsigned BufferType>
class BufferObject
{
private:
	GLuint m_id;
	static GLuint m_active_id;

public:
	BufferObject()
	{
		glGenBuffers(1, &m_id);
	}

	bool isBound() const
	{
		return m_id == m_active_id;
	}

	void bufferData(unsigned size, GLfloat* data, DrawHint type) const
	{
		bind();
		glBufferData(BufferType, size, data, static_cast<unsigned>(type));
	}

	void bind() const
	{
		if (isBound()) return;
		glBindBuffer(BufferType, m_id);
		m_active_id = m_id;
	}

};

template<unsigned N>
GLuint BufferObject<N>::m_active_id = 0;

using VertexBufferObject = BufferObject<GL_ARRAY_BUFFER>;
using IndexBufferObject = BufferObject<GL_ELEMENT_ARRAY_BUFFER>;