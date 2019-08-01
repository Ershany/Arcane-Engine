#pragma once

#include <platform/OpenGL/Framebuffers/Framebuffer.h>

namespace arcane {

	class GBuffer : public Framebuffer {
	public:
		GBuffer(unsigned int width, unsigned int height);
		~GBuffer();

		inline Texture* getAlbedo() { return &m_GBufferRenderTargets[0]; }
		inline Texture* getNormal() { return &m_GBufferRenderTargets[1]; }
		inline Texture* getMaterialInfo() { return &m_GBufferRenderTargets[2]; }
	private:
		void init();
	private:
		// 0 RGBA8  ->       albedo.r     albedo.g        albedo.b     albedo's alpha
		// 1 RGB32F ->       normal.x     normal.y        normal.z
		// 2 RGBA8  ->       metallic     roughness       ambientOcclusion
		std::array<Texture, 3> m_GBufferRenderTargets;
	};

}
