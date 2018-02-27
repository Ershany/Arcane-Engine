#pragma once

#include "Framebuffer.h"

namespace arcane { namespace opengl {

	class ShadowMap : private Framebuffer {
	public:
		ShadowMap(int width, int height);

		inline GLuint getDepthBufferTexture() { return m_DepthTexture; }
	private:
		GLuint m_DepthTexture;
	};

} }