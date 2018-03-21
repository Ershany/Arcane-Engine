#include "Framebuffer.h"

namespace arcane { namespace opengl {

	Framebuffer::Framebuffer(int width, int height)
		: m_Width(width), m_Height(height), m_Created(false), m_FBO(0), m_ColourTexture(0)
	{
		glGenFramebuffers(1, &m_FBO);
	}

	Framebuffer::~Framebuffer() {
		glDeleteFramebuffers(1, &m_FBO);
	}

	void Framebuffer::createFramebuffer() {
		// Check if the creation failed
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			utils::Logger::getInstance().error("logged_files/error.txt", "Framebuffer initialization", "Could not initialize the framebuffer");
			return;
		}
	}

	Framebuffer& Framebuffer::addColorAttachment(bool multisampledBuffer) {
		bind();

		// Generate colour texture attachment
		glGenTextures(1, &m_ColourTexture);
		if (multisampledBuffer) {
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColourTexture);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLE_AMOUNT, GL_RGB, m_Width, m_Height, GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, m_ColourTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Both need to clamp to edge or you might see strange colours around the
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // border due to interpolation and how it works with GL_REPEAT
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Attach colour attachment
		if (multisampledBuffer) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ColourTexture, 0);
		}
		else {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColourTexture, 0);
		}

		unbind();
		return *this;
	}

	Framebuffer& Framebuffer::addDepthStencilRBO(bool multisampledBuffer) {
		bind();

		// Generate depth+stencil rbo attachment
		glGenRenderbuffers(1, &m_DepthStencilRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_DepthStencilRBO);
		if (multisampledBuffer)
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLE_AMOUNT, GL_DEPTH24_STENCIL8, m_Width, m_Height);
		else
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);

		// Attach depth+stencil attachment
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthStencilRBO);

		unbind();
		return *this;
	}

	void Framebuffer::bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	}

	void Framebuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

} }