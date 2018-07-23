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
		m_ColourTexture = new graphics::Texture();

		// Generate colour texture attachment
		if (multisampledBuffer) {
			m_ColourTexture->Generate2DMultisampleTexture(m_Width, m_Height, GL_RGB, MSAA_SAMPLE_AMOUNT);
		}
		else {
			m_ColourTexture->SetTextureMinFilter(GL_LINEAR);
			m_ColourTexture->SetTextureMagFilter(GL_LINEAR);
			m_ColourTexture->SetTextureWrapS(GL_CLAMP_TO_EDGE); // Both need to clamp to edge or you might see strange colours around the
			m_ColourTexture->SetTextureWrapT(GL_CLAMP_TO_EDGE); // border due to interpolation and how it works with GL_REPEAT
			m_ColourTexture->Generate2DTexture(m_Width, m_Height, GL_RGB, GL_RGB, nullptr);
		}

		// Attach colour attachment
		if (multisampledBuffer) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ColourTexture->GetTextureId(), 0);
		}
		else {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColourTexture->GetTextureId(), 0);
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