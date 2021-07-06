#include "arcpch.h"
#include "IndexBuffer.h"

namespace Arcane
{
	IndexBuffer::IndexBuffer() {
		glGenBuffers(1, &m_BufferID);
	}

	IndexBuffer::IndexBuffer(unsigned int *data, int amount) {
		glGenBuffers(1, &m_BufferID);
		Load(data, amount);
	}

	IndexBuffer::~IndexBuffer() {
		glDeleteBuffers(1, &m_BufferID);
	}

	void IndexBuffer::Load(unsigned int *data, int amount) {
		m_Count = amount;

		Bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, amount * sizeof(unsigned int), data, GL_STATIC_DRAW);
	}

	void IndexBuffer::Bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	}

	void IndexBuffer::Unbind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}
