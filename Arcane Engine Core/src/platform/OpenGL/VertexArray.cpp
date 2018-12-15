#include "pch.h"
#include "VertexArray.h"

namespace arcane {

	VertexArray::VertexArray() {
		glGenVertexArrays(1, &m_VertexArrayID);
	}

	VertexArray::~VertexArray() {
		for (unsigned int i = 0; i < m_Buffers.size(); ++i) {
			delete m_Buffers[i];
		}

		glDeleteVertexArrays(1, &m_VertexArrayID);
	}

	void VertexArray::addBuffer(Buffer *buffer, int index) {
		bind();

		buffer->bind();
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, buffer->getComponentCount(), GL_FLOAT, GL_FALSE, 0, 0);
		buffer->unbind();

		unbind();
	}

	void VertexArray::bind() const {
		glBindVertexArray(m_VertexArrayID);
	}

	void VertexArray::unbind() const {
		glBindVertexArray(0);
	}

}
