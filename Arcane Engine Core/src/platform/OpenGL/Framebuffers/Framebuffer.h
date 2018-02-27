#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include "../../../Defs.h"
#include "../../../utils/Logger.h"

namespace arcane { namespace opengl {

	class Framebuffer {
	public:
		Framebuffer(int width, int height);
		~Framebuffer();

		void createFramebuffer();
		Framebuffer& addColorAttachment(bool multisampledBuffer);
		Framebuffer& addDepthStencilRBO(bool multisampledBuffer);

		void bind();
		void unbind();

		inline GLuint getFramebuffer() { return m_FBO; }
		inline GLuint getColourBufferTexture() { return m_ColourTexture; }
	private:
		GLuint m_FBO, m_ColourTexture, m_DepthStencilRBO;
		
		bool m_Created;
		unsigned int m_Width, m_Height;
	};

} }