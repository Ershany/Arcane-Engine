#pragma once

#include <utils/loaders/TextureLoader.h>

namespace arcane {

	class Framebuffer {
	public:
		Framebuffer(unsigned int width, unsigned int height);
		virtual ~Framebuffer();

		// Creation functions
		void createFramebuffer();
		Framebuffer& addTexture2DColorAttachment(bool multisampledBuffer);
		Framebuffer& addDepthRBO(bool multisampledBuffer);
		Framebuffer& addDepthStencilRBO(bool multisampledBuffer);
		Framebuffer& addDepthAttachment(bool multisampledBuffer);

		void bind();
		void unbind();

		// Assumes framebuffer is bound
		void setColorAttachment(unsigned int target, unsigned int targetType, int mipToWriteTo = 0);
		void clear();

		inline unsigned int getWidth() { return m_Width; }
		inline unsigned int getHeight() { return m_Height; }

		inline unsigned int getFramebuffer() { return m_FBO; }
		inline unsigned int getColourBufferTexture() { return m_ColourTexture; }
		inline unsigned int getDepthRBO() { return m_DepthRBO; }
		inline unsigned int getDepthStencilRBO() { return m_DepthStencilRBO; }
		inline unsigned int getDepthTexture() { return m_DepthTexture; }

		inline bool isMultisampledColourBuffer() { return m_IsMultisampledColourBuffer; }
	protected:
		unsigned int m_FBO;

		bool m_IsMultisampledColourBuffer;
		
		// Render Targets (Attachments)
		unsigned int m_ColourTexture;
		unsigned int m_DepthRBO;
		unsigned int m_DepthStencilRBO;
		unsigned int m_DepthTexture;
		
		unsigned int m_Width, m_Height;
	};

}
