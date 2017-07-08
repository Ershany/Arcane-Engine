#include "Buffer.h"

namespace arcane { namespace opengl {

	Buffer::Buffer(GLfloat *data, GLsizei amount, GLuint componentCount) : m_ComponentCount(componentCount)
	{
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(GL_FLOAT), data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	Buffer::~Buffer() {
		glDeleteBuffers(1, &m_BufferID);
	}

	void Buffer::bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}

	void Buffer::unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

} }