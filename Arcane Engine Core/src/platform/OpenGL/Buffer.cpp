#include "pch.h"
#include "Buffer.h"

namespace arcane {

	Buffer::Buffer() {
		glGenBuffers(1, &m_BufferID);
	}

	Buffer::Buffer(float *data, int amount, unsigned int componentCount) {
		glGenBuffers(1, &m_BufferID);
		load(data, amount, componentCount);
	}

	Buffer::~Buffer() {
		glDeleteBuffers(1, &m_BufferID);
	}

	void Buffer::load(float *data, int amount, unsigned int componentCount) {
		m_ComponentCount = componentCount;

		bind();
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(float), data, GL_STATIC_DRAW);
	}

	void Buffer::bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}

	void Buffer::unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

}
