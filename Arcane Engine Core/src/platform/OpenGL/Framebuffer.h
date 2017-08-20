#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "../../utils/Logger.h"

namespace arcane { namespace opengl {

	class Framebuffer {
	public:
		Framebuffer(int width, int height);

		void bind();
		void unbind();

		inline GLuint getColourBufferTexture() { return m_ColourTexture; }
		inline GLuint getDepthStencilBufferTexture() { return m_DepthStencilTexture; }
	private:
		GLuint m_FBO, m_DepthStencilTexture, m_ColourTexture;
		
		unsigned int m_Width, m_Height;
	};

} }