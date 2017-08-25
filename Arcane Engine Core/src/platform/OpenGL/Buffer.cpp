#include "Buffer.h"

namespace arcane { namespace opengl {

	Buffer::Buffer() {
		glGenBuffers(1, &m_BufferID);
	}

	Buffer::Buffer(GLfloat *data, GLsizei amount, GLuint componentCount) {
		glGenBuffers(1, &m_BufferID);
		load(data, amount, componentCount);
	}

	Buffer::~Buffer() {
		glDeleteBuffers(1, &m_BufferID);
	}

	void Buffer::load(GLfloat *data, GLsizei amount, GLuint componentCount) {
		m_ComponentCount = componentCount;

		bind();
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(GL_FLOAT), data, GL_STATIC_DRAW);
		unbind();
	}

	void Buffer::bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}

	void Buffer::unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

} }