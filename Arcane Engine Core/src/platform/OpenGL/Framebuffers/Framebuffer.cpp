#include "pch.h"
#include "Framebuffer.h"

namespace arcane {

	Framebuffer::Framebuffer(unsigned int width, unsigned int height, bool isMultisampled)
		: m_FBO(0), m_Width(width), m_Height(height), m_IsMultisampled(isMultisampled), m_ColourTexture(0), m_DepthStencilTexture(0), m_DepthStencilRBO(0)
	{
		glGenFramebuffers(1, &m_FBO);
	}

	Framebuffer::~Framebuffer() {
		if (m_ColourTexture != 0) {
			glDeleteTextures(1, &m_ColourTexture);
		}
		if (m_DepthStencilTexture != 0) {
			glDeleteTextures(1, &m_DepthStencilTexture);
		}
		if (m_DepthStencilRBO != 0) {
			glDeleteRenderbuffers(1, &m_DepthStencilRBO);
		}

		glDeleteFramebuffers(1, &m_FBO);
	}

	void Framebuffer::createFramebuffer() {
		bind();
		if (m_ColourTexture == 0) {
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
		if (m_ColourTexture != 0) {
			Logger::getInstance().error("logged_files/error.txt", "Framebuffer initialization", "Framebuffer already has a colour attachment");
			return *this;
		}
#endif

		bind();
		glGenTextures(1, &m_ColourTexture);

		// Generate colour texture attachment
		if (m_IsMultisampled) {
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColourTexture);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLE_AMOUNT, textureFormat, m_Width, m_Height, GL_TRUE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ColourTexture, 0);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, m_ColourTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColourTexture, 0);
		}

		unbind();
		return *this;
	}

	Framebuffer& Framebuffer::addDepthStencilTexture(DepthStencilAttachmentFormat textureFormat) {
#if DEBUG_ENABLED
		if (m_DepthStencilTexture != 0) {
			Logger::getInstance().error("logged_files/error.txt", "Framebuffer initialization", "Framebuffer already has a depth attachment");
			return *this;
		}
#endif

		bind();

		GLenum attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
		if (textureFormat == NormalizedDepthOnly) {
			attachmentType = GL_DEPTH_ATTACHMENT;
		}

		// Generate depth attachment
		glGenTextures(1, &m_DepthStencilTexture);

		if (m_IsMultisampled) {
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_DepthStencilTexture);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLE_AMOUNT, textureFormat, m_Width, m_Height, GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D_MULTISAMPLE, m_DepthStencilTexture, 0);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, m_DepthStencilTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColour[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColour);
			glBindTexture(GL_TEXTURE_2D, 0);

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, m_DepthStencilTexture, 0);
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
