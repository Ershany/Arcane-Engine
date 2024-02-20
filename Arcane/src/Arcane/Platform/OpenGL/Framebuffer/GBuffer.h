#pragma once
#ifndef GBUFFER_H
#define GBUFFER_H

#ifndef FRAMEBUFFER_H
#include <Arcane/Platform/OpenGL/Framebuffer/Framebuffer.h>
#endif

namespace Arcane
{
	class GBuffer : public Framebuffer
	{
	public:
		GBuffer(unsigned int width, unsigned int height);
		~GBuffer();

		inline Texture* GetAlbedo() { return &m_GBufferRenderTargets[0]; }
		inline Texture* GetNormal() { return &m_GBufferRenderTargets[1]; }
		inline Texture* GetMaterialInfo() { return &m_GBufferRenderTargets[2]; }
	private:
		void Init();
	private:
		// 0 RGBA8  ->       albedo.r     albedo.g        albedo.b             albedo's alpha
		// 1 RGB32F ->       normal.x     normal.y        normal.z
		// 2 RGBA8  ->       metallic     roughness       ambientOcclusion     emissionIntensity
		std::array<Texture, 3> m_GBufferRenderTargets;
	};
}
#endif
