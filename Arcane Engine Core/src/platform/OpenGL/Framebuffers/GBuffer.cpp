#include "pch.h"
#include "GBuffer.h"

namespace arcane {

	GBuffer::GBuffer(unsigned int width, unsigned int height) : Framebuffer(width, height) {
		init();
	}

	GBuffer::~GBuffer() {

	}

	void GBuffer::init() {
		addDepthAttachment(false);

		bind();

		// Render Target 1
		glGenTextures(1, &m_GBufferRTs[0]);
		glBindTexture(GL_TEXTURE_2D, m_GBufferRTs[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_GBufferRTs[0], 0);

		// Render Target 2
		glGenTextures(1, &m_GBufferRTs[1]);
		glBindTexture(GL_TEXTURE_2D, m_GBufferRTs[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_GBufferRTs[1], 0);

		// Render Target 3
		glGenTextures(1, &m_GBufferRTs[2]);
		glBindTexture(GL_TEXTURE_2D, m_GBufferRTs[2]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_GBufferRTs[2], 0);

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
