#pragma once

#include "Framebuffer.h"

namespace arcane { namespace opengl {

	class ShadowMap : private Framebuffer {
	public:
		ShadowMap(int width, int height);

		inline unsigned int getDepthBufferTexture() { return m_DepthTexture; }
	private:
		unsigned int m_DepthTexture;
	};

} }