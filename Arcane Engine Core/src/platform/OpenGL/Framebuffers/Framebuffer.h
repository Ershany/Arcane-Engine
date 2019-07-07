#pragma once

#include <utils/loaders/TextureLoader.h>

namespace arcane {

	enum ColorAttachmentFormat {
		Normalized8 = GL_RGBA8,
		Normalized16 = GL_RGBA16,
		FloatingPoint16 = GL_RGBA16F,
		FloatingPoint32 = GL_RGBA32F
	};

	enum DepthStencilAttachmentFormat {
		NormalizedDepthOnly = GL_DEPTH_COMPONENT,
		NormalizedDepthStencil = GL_DEPTH24_STENCIL8,
		FloatingPointDepthStencil = GL_DEPTH32F_STENCIL8
	};

	class Framebuffer {
	public:
		Framebuffer(unsigned int width, unsigned int height, bool isMultisampled);
		virtual ~Framebuffer();

		// Creation functions
		void createFramebuffer();
		Framebuffer& addColorTexture(ColorAttachmentFormat textureFormat);
		Framebuffer& addDepthStencilTexture(DepthStencilAttachmentFormat textureFormat);
		Framebuffer& addDepthStencilRBO(DepthStencilAttachmentFormat rboFormat);

		void bind();
		void unbind();

		// Assumes framebuffer is bound
		void setColorAttachment(unsigned int target, unsigned int targetType, int mipToWriteTo = 0);
		void clear();

		// Getters
		inline unsigned int getFramebuffer() { return m_FBO; }

		inline unsigned int getWidth() { return m_Width; }
		inline unsigned int getHeight() { return m_Height; }

		inline bool isMultisampled() { return m_IsMultisampled; }

		inline unsigned int getColourTexture() { return m_ColourTexture; }

		inline unsigned int getDepthStencilTexture() { return m_DepthStencilTexture; }
		inline unsigned int getDepthStencilRBO() { return m_DepthStencilRBO; }
	protected:
		unsigned int m_FBO;

		unsigned int m_Width, m_Height;
		bool m_IsMultisampled;
		
		// Render Targets (Attachments)
		unsigned int m_ColourTexture;
		unsigned int m_DepthStencilTexture;
		unsigned int m_DepthStencilRBO;
	};

}
