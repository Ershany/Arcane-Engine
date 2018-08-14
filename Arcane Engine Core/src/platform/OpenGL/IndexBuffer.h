#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>

namespace arcane { namespace opengl {

	class IndexBuffer {
	private:
		unsigned int m_BufferID;
		int m_Count;
	public:
		IndexBuffer();
		IndexBuffer(unsigned int *data, int amount);
		~IndexBuffer();

		void load(unsigned int *data, int amount);

		void bind() const;
		void unbind() const;

		inline int getCount() const { return m_Count; }
	};

} }