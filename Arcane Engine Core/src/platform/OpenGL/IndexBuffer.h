#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>

namespace arcane { namespace opengl {

	class IndexBuffer {
	private:
		GLuint m_BufferID;
		GLsizei m_Count;
	public:
		IndexBuffer(GLuint *data, GLsizei amount);
		~IndexBuffer();

		void bind() const;
		void unbind() const;

		inline GLsizei getCount() { return m_Count; }
	};

} }