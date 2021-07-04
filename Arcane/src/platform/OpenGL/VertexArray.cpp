#include "arcpch.h"
#include "VertexArray.h"

namespace Arcane
{
	VertexArray::VertexArray() {
		glGenVertexArrays(1, &m_VertexArrayID);
	}

	VertexArray::~VertexArray() {
		for (unsigned int i = 0; i < m_Buffers.size(); ++i) {
			delete m_Buffers[i];
		}

		glDeleteVertexArrays(1, &m_VertexArrayID);
	}

	void VertexArray::AddBuffer(Buffer *buffer, int index) {
		Bind();

		buffer->Bind();
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, buffer->GetComponentCount(), GL_FLOAT, GL_FALSE, 0, 0);
		buffer->Unbind();

		Unbind();
	}

	void VertexArray::Bind() const {
		glBindVertexArray(m_VertexArrayID);
	}

	void VertexArray::Unbind() const {
		glBindVertexArray(0);
	}
}
