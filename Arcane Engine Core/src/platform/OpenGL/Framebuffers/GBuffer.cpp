#include "pch.h"
#include "GBuffer.h"

namespace arcane {

	GBuffer::GBuffer(unsigned int width, unsigned int height) : Framebuffer(width, height, false) {
		init();
	}

	GBuffer::~GBuffer() {}

	void GBuffer::init() {
		addDepthStencilTexture(NormalizedDepthStencil);

		bind();

		// Render Target 1
		{
			TextureSettings renderTarget1;
			renderTarget1.TextureFormat = GL_RGBA8;
			renderTarget1.TextureWrapSMode = GL_CLAMP_TO_EDGE;
			renderTarget1.TextureWrapTMode = GL_CLAMP_TO_EDGE;
			renderTarget1.TextureMinificationFilterMode = GL_NEAREST;
			renderTarget1.TextureMagnificationFilterMode = GL_NEAREST;
			renderTarget1.TextureAnisotropyLevel = 1.0f;
			renderTarget1.HasMips = false;
			m_GBufferRenderTargets[0].setTextureSettings(renderTarget1);
			m_GBufferRenderTargets[0].generate2DTexture(m_Width, m_Height, GL_RGB);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_GBufferRenderTargets[0].getTextureId(), 0);
		}

		// Render Target 2
		{
			TextureSettings renderTarget2;
			renderTarget2.TextureFormat = GL_RGB32F;
			renderTarget2.TextureWrapSMode = GL_CLAMP_TO_EDGE;
			renderTarget2.TextureWrapTMode = GL_CLAMP_TO_EDGE;
			renderTarget2.TextureMinificationFilterMode = GL_NEAREST;
			renderTarget2.TextureMagnificationFilterMode = GL_NEAREST;
			renderTarget2.TextureAnisotropyLevel = 1.0f;
			renderTarget2.HasMips = false;
			m_GBufferRenderTargets[1].setTextureSettings(renderTarget2);
			m_GBufferRenderTargets[1].generate2DTexture(m_Width, m_Height, GL_RGB);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_GBufferRenderTargets[1].getTextureId(), 0);
		}

		// Render Target 3
		{
			TextureSettings renderTarget3;
			renderTarget3.TextureFormat = GL_RGBA8;
			renderTarget3.TextureWrapSMode = GL_CLAMP_TO_EDGE;
			renderTarget3.TextureWrapTMode = GL_CLAMP_TO_EDGE;
			renderTarget3.TextureMinificationFilterMode = GL_NEAREST;
			renderTarget3.TextureMagnificationFilterMode = GL_NEAREST;
			renderTarget3.TextureAnisotropyLevel = 1.0f;
			renderTarget3.HasMips = false;
			m_GBufferRenderTargets[2].setTextureSettings(renderTarget3);
			m_GBufferRenderTargets[2].generate2DTexture(m_Width, m_Height, GL_RGB);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_GBufferRenderTargets[2].getTextureId(), 0);
		}

		// Finally tell OpenGL that we will be rendering to all of the attachments
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		// Check if the creation failed
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			Logger::getInstance().error("logged_files/error.txt", "Framebuffer initialization", "Could not initialize GBuffer");
			return;
		}
		unbind();
	}

}
