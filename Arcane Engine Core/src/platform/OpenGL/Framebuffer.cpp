#include "Framebuffer.h"

namespace arcane { namespace opengl {

	Framebuffer::Framebuffer(int width, int height, bool multisampledBuffers) : m_Width(width), m_Height(height) {
		// Construct the framebuffer
		glGenFramebuffers(1, &m_FBO);
		bind();
		
		// Depth and Stencil buffers
		glGenRenderbuffers(1, &m_DepthStencilRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_DepthStencilRBO);
		if (multisampledBuffers)
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLE_AMOUNT, GL_DEPTH24_STENCIL8, width, height);
		else
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

		// Colour buffers
		glGenTextures(1, &m_ColourTexture);
		if (multisampledBuffers) {
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColourTexture);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLE_AMOUNT, GL_RGB, width, height, GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, m_ColourTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Both need to clamp to edge or you might see strange colours around the
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // border due to interpolation and how it works with GL_REPEAT
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		

		// Add Attachments (ie colour, depth, and stencil buffer)
		if (multisampledBuffers) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ColourTexture, 0);
		}
		else {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColourTexture, 0);
		}
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthStencilRBO);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		

		// Check if the creation failed
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			utils::Logger::getInstance().error("logged_files/error.txt", "Framebuffer initialization", "Could not initialize the framebuffer");
			return;
		}

		unbind();
	}

	Framebuffer::~Framebuffer() {

	}

	void Framebuffer::bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	}

	void Framebuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // This makes sure we use the default framebuffer (provided by GLFW) again
	}

} }