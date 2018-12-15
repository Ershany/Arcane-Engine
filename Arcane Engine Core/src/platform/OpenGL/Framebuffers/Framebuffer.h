#pragma once

#include <utils/loaders/TextureLoader.h>

namespace arcane {

	class Framebuffer {
	public:
		Framebuffer(unsigned int width, unsigned int height);
		~Framebuffer();

		void createFramebuffer();
		Framebuffer& addColorAttachment(bool multisampledBuffer);
		Framebuffer& addDepthStencilRBO(bool multisampledBuffer);
		Framebuffer& addDepthAttachment(bool multisampledBuffer);

		void bind();
		void unbind();

		void clear();

		inline unsigned int getWidth() { return m_Width; }
		inline unsigned int getHeight() { return m_Height; }

		inline unsigned int getFramebuffer() { return m_FBO; }
		inline unsigned int getColourBufferTexture() { return m_ColourTexture; }
		inline unsigned int getDepthTexture() { return m_DepthTexture; }

		inline bool isMultisampledColourBuffer() { return m_IsMultisampledColourBuffer; }
	private:
		unsigned int m_FBO;

		bool m_IsMultisampledColourBuffer;
		
		// Render Targets (Attachments)
		unsigned int m_ColourTexture;
		unsigned int m_DepthStencilRBO;
		unsigned int m_DepthTexture;
		
		unsigned int m_Width, m_Height;
	};

}
