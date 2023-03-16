#pragma once

#include <Arcane/Graphics/Texture/Texture.h>

namespace Arcane
{
	enum ColorAttachmentFormat {
		NormalizedSingleChannel8 = GL_RED,
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

	enum StencilValue : int
	{
		ModelStencilValue = 0x01,
		TerrainStencilValue = 0x02
	};

	class Framebuffer {
	public:
		Framebuffer(unsigned int width, unsigned int height, bool isMultisampled);
		virtual ~Framebuffer();

		// Creation functions
		void CreateFramebuffer();
		Framebuffer& AddColorTexture(ColorAttachmentFormat textureFormat);
		Framebuffer& AddDepthStencilTexture(DepthStencilAttachmentFormat textureFormat);
		Framebuffer& AddDepthStencilRBO(DepthStencilAttachmentFormat rboFormat);

		void Bind();
		void Unbind();

		// Assumes framebuffer is bound
		void SetColorAttachment(unsigned int target, unsigned int targetType, int mipToWriteTo = 0);
		void Clear();

		// Getters
		inline unsigned int GetFramebuffer() { return m_FBO; }

		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }

		inline bool IsMultisampled() const { return m_IsMultisampled; }

		inline Texture* GetColourTexture() { return &m_ColourTexture; }

		inline Texture* GetDepthStencilTexture() { return &m_DepthStencilTexture; }
		inline unsigned int GetDepthStencilRBO() { return m_DepthStencilRBO; }
	protected:
		unsigned int m_FBO;

		unsigned int m_Width, m_Height;
		bool m_IsMultisampled;
		
		// Render Targets (Attachments)
		Texture m_ColourTexture;
		Texture m_DepthStencilTexture;
		unsigned int m_DepthStencilRBO;
	};
}
