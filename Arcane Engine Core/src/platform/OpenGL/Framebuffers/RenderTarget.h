#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "../../../Defs.h"
#include "../../../utils/Logger.h"
#include "../../../utils/loaders/TextureLoader.h"

namespace arcane { namespace opengl {

	class RenderTarget {
	public:
		RenderTarget(unsigned int width, unsigned int height);
		~RenderTarget();

		void createFramebuffer();
		RenderTarget& addColorAttachment(bool multisampledBuffer);
		RenderTarget& addDepthStencilRBO(bool multisampledBuffer);
		RenderTarget& addDepthAttachment(bool multisampledBuffer);

		void bind();
		void unbind();

		void clear();

		inline unsigned int getWidth() { return m_Width; }
		inline unsigned int getHeight() { return m_Height; }

		inline unsigned int getFramebuffer() { return m_FBO; }
		inline graphics::Texture* getColourBufferTexture() { return m_ColourTexture; }
		inline unsigned int getDepthTexture() { return m_DepthTexture; }

		inline bool isMultisampledColourBuffer() { return m_IsMultisampledColourBuffer; }
	private:
		unsigned int m_FBO;

		bool m_IsMultisampledColourBuffer;
		
		// Attachments
		graphics::Texture *m_ColourTexture;
		unsigned int m_DepthStencilRBO;
		unsigned int m_DepthTexture;
		
		unsigned int m_Width, m_Height;
	};

} }