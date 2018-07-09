#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "Buffer.h"
#include <vector>

namespace arcane { namespace opengl {

	class VertexArray {
	private:
		unsigned int m_VertexArrayID;
		std::vector<Buffer*> m_Buffers;
	public:
		VertexArray();
		~VertexArray();

		// Function for automatically adding non-interleaved buffer data
		void addBuffer(Buffer *buffer, int index);
		
		// Function for manually setting up vertex pointers. Required if you want to setup how interleaved buffer data
		void addVertexAttribArrayPointer(int index, int componentCount, size_t stride, size_t offset);

		void bind() const;
		void unbind() const;
	};

} }