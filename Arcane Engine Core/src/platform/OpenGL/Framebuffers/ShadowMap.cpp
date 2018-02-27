#include "ShadowMap.h"

namespace arcane { namespace opengl {

	ShadowMap::ShadowMap(int width, int height) : Framebuffer(width, height) { 
		bind();

		// Generate shadowmap
		glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Should be changed in order to properly work for shadow mapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Should be changed in order to properly work for shadow mapping
		glBindTexture(GL_TEXTURE_2D, 0);

		// Attach shadowmap
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);

		// Indicate that there won't be a colour buffer for this FBO
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		createFramebuffer();
		unbind();
	}

} }