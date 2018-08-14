#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include "../../../Defs.h"
#include "../../../utils/Logger.h"
#include "../../../utils/loaders/TextureLoader.h"

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

		inline unsigned int getFramebuffer() { return m_FBO; }
		inline graphics::Texture* getColourBufferTexture() { return m_ColourTexture; }
	private:
		unsigned int m_FBO, m_DepthStencilRBO;
		graphics::Texture *m_ColourTexture;
		
		bool m_Created;
		unsigned int m_Width, m_Height;
	};

} }