#include "pch.h"
#include "Framebuffer.h"

namespace arcane {

	Framebuffer::Framebuffer(unsigned int width, unsigned int height, bool isMultisampled)
		: m_FBO(0), m_Width(width), m_Height(height), m_IsMultisampled(isMultisampled), m_ColourTexture(), m_DepthStencilTexture(), m_DepthStencilRBO(0)
	{
		glGenFramebuffers(1, &m_FBO);
	}

	Framebuffer::~Framebuffer() {
		glDeleteRenderbuffers(1, &m_DepthStencilRBO);

		glDeleteFramebuffers(1, &m_FBO);
	}

	void Framebuffer::createFramebuffer() {
		bind();
		if (!m_ColourTexture.isGenerated()) {
			// Indicate that there won't be a colour buffer for this FBO
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		// Check if the creation failed
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			Logger::getInstance().error("logged_files/error.txt", "Framebuffer initialization", "Could not initialize the framebuffer");
			return;
		}
		unbind();
	}

	Framebuffer& Framebuffer::addColorTexture(ColorAttachmentFormat textureFormat) {
#if DEBUG_ENABLED
		if (m_ColourTexture.isGenerated()) {
			Logger::getInstance().error("logged_files/error.txt", "Framebuffer initialization", "Framebuffer already has a colour attachment");
			return *this;
		}
#endif

		bind();

		TextureSettings colourTextureSettings;
		colourTextureSettings.TextureFormat = textureFormat;
		colourTextureSettings.TextureWrapSMode = GL_CLAMP_TO_EDGE;
		colourTextureSettings.TextureWrapTMode = GL_CLAMP_TO_EDGE;
		colourTextureSettings.TextureMinificationFilterMode = GL_LINEAR;
		colourTextureSettings.TextureMagnificationFilterMode = GL_LINEAR;
		colourTextureSettings.TextureAnisotropyLevel = 1.0f;
		colourTextureSettings.HasMips = false;
		m_ColourTexture.setTextureSettings(colourTextureSettings);

		// Generate colour texture attachment
		if (m_IsMultisampled) {
			m_ColourTexture.generate2DMultisampleTexture(m_Width, m_Height);
			setColorAttachment(m_ColourTexture.getTextureId(), GL_TEXTURE_2D_MULTISAMPLE);
		}
		else {
			m_ColourTexture.generate2DTexture(m_Width, m_Height, GL_RGB);
			setColorAttachment(m_ColourTexture.getTextureId(), GL_TEXTURE_2D);
		}

		unbind();
		return *this;
	}

	Framebuffer& Framebuffer::addDepthStencilTexture(DepthStencilAttachmentFormat textureFormat) {
#if DEBUG_ENABLED
		if (m_DepthStencilTexture.isGenerated()) {
			Logger::getInstance().error("logged_files/error.txt", "Framebuffer initialization", "Framebuffer already has a depth attachment");
			return *this;
		}
#endif

		GLenum attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
		if (textureFormat == NormalizedDepthOnly) {
			attachmentType = GL_DEPTH_ATTACHMENT;
		}

		bind();

		TextureSettings depthStencilSettings;
		depthStencilSettings.TextureFormat = textureFormat;
		depthStencilSettings.TextureWrapSMode = GL_CLAMP_TO_BORDER;
		depthStencilSettings.TextureWrapTMode = GL_CLAMP_TO_BORDER;
		depthStencilSettings.TextureMinificationFilterMode = GL_NEAREST;
		depthStencilSettings.TextureMagnificationFilterMode = GL_NEAREST;
		depthStencilSettings.TextureAnisotropyLevel = 1.0f;
		depthStencilSettings.HasBorder = true;
		depthStencilSettings.BorderColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		depthStencilSettings.HasMips = false;
		m_DepthStencilTexture.setTextureSettings(depthStencilSettings);

		// Generate depth attachment
		if (m_IsMultisampled) {
			m_DepthStencilTexture.generate2DMultisampleTexture(m_Width, m_Height);
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D_MULTISAMPLE, m_DepthStencilTexture.getTextureId(), 0);
		}
		else {
			m_DepthStencilTexture.generate2DTexture(m_Width, m_Height, GL_DEPTH_COMPONENT);
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, m_DepthStencilTexture.getTextureId(), 0);
		}

		unbind();
		return *this;
	}

	Framebuffer& Framebuffer::addDepthStencilRBO(DepthStencilAttachmentFormat textureFormat) {
#if DEBUG_ENABLED
		if (m_DepthStencilRBO != 0) {
			Logger::getInstance().error("logged_files/error.txt", "Framebuffer initialization", "Framebuffer already has a depth+stencil RBO attachment");
			return *this;
		}
#endif

		bind();

		GLenum attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
		if (textureFormat == NormalizedDepthOnly) {
			attachmentType = GL_DEPTH_ATTACHMENT;
		}

		// Generate depth+stencil RBO attachment
		glGenRenderbuffers(1, &m_DepthStencilRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_DepthStencilRBO);

		if (m_IsMultisampled) {
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLE_AMOUNT, textureFormat, m_Width, m_Height);
		}
		else {
			glRenderbufferStorage(GL_RENDERBUFFER, textureFormat, m_Width, m_Height);
		}

		// Attach depth+stencil attachment
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, m_DepthStencilRBO);

		unbind();
		return *this;
	}

	void Framebuffer::setColorAttachment(unsigned int target, unsigned int targetType, int mipToWriteTo) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, targetType, target, mipToWriteTo);
	}

	void Framebuffer::bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	}

	void Framebuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

}
