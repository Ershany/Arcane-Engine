#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include "../../Defs.h"
#include "../../utils/Logger.h"

namespace arcane { namespace opengl {

	class Framebuffer {
	public:
		Framebuffer(int width, int height, bool multisampledBuffers = true);
		~Framebuffer();

		void bind();
		void unbind();

		inline GLuint getFramebuffer() { return m_FBO; }
		inline GLuint getColourBufferTexture() { return m_ColourTexture; }
		inline GLuint getDepthStencilBufferTexture() { return m_DepthStencilRBO; }
	private:
		GLuint m_FBO, m_DepthStencilRBO, m_ColourTexture;
		
		unsigned int m_Width, m_Height;
	};

} }