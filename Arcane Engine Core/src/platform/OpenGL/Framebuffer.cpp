#include "Framebuffer.h"

namespace arcane { namespace opengl {

	Framebuffer::Framebuffer(int width, int height) : m_Width(width), m_Height(height) {
		// Construct the framebuffer
		glGenFramebuffers(1, &m_FBO);
		bind();
		
		// Depth and Stencil
		glGenTextures(1, &m_DepthStencilTexture);
		glBindTexture(GL_TEXTURE_2D, m_DepthStencilTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Both need to clamp to edge or you might see strange colours around the
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // border due to interpolation and how it works with GL_REPEAT
		glBindTexture(GL_TEXTURE_2D, 0);

		// Colour
		glGenTextures(1, &m_ColourTexture);
		glBindTexture(GL_TEXTURE_2D, m_ColourTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Both need to clamp to edge or you might see strange colours around the
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // border due to interpolation and how it works with GL_REPEAT
		glBindTexture(GL_TEXTURE_2D, 0);

		// Add Attachments (ie colour, depth, and stencil buffer)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColourTexture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthStencilTexture, 0);
		

		// Check if the creation failed
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			utils::Logger::getInstance().error("logged_files/error.txt", "Framebuffer initialization", "Could not initialize the framebuffer");
			return;
		}

		unbind();
	}

	void Framebuffer::bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	}

	void Framebuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // This makes sure we use the default framebuffer (provided by GLFW) again
	}

} }