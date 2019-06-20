#pragma once

#include <platform/OpenGL/Framebuffers/Framebuffer.h>

namespace arcane {

	class GBuffer : public Framebuffer {
	public:
		GBuffer(unsigned int width, unsigned int height);
		~GBuffer();
	private:
		void init();
	private:
		// 0 RGBA8  ->       albedo.r     albedo.g        albedo.b     ambientOcclusion
		// 1 RGB16F ->       normal.x     normal.y        normal.z
		// 2 RGBA8  ->       metallic     roughness
		std::array<unsigned int, 3> m_GBufferRTs;
	};

}
