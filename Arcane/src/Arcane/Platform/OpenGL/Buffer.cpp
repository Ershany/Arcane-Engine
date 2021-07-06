#include "arcpch.h"
#include "Buffer.h"

namespace Arcane
{
	Buffer::Buffer() {
		glGenBuffers(1, &m_BufferID);
	}

	Buffer::Buffer(float *data, int amount, unsigned int componentCount) {
		glGenBuffers(1, &m_BufferID);
		Load(data, amount, componentCount);
	}

	Buffer::~Buffer() {
		glDeleteBuffers(1, &m_BufferID);
	}

	void Buffer::Load(float *data, int amount, unsigned int componentCount) {
		m_ComponentCount = componentCount;

		Bind();
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(float), data, GL_STATIC_DRAW);
	}

	void Buffer::Bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}

	void Buffer::Unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
