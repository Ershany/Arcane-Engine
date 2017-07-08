#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>

namespace arcane { namespace opengl {

	class Buffer {
	private:
		GLuint m_BufferID;
		GLuint m_ComponentCount;
	public:
		Buffer(GLfloat *data, GLsizei amount, GLuint componentCount);
		~Buffer();

		void bind() const;
		void unbind() const;

		inline GLuint getComponentCount() const { return m_ComponentCount; }
	};

} }