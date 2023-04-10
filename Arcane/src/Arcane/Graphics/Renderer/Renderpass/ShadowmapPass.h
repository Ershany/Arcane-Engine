#pragma once

#include <Arcane/Graphics/Camera/CubemapCamera.h>
#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>

namespace Arcane
{
	class ICamera;
	class Scene;
	class Shader;

	class ShadowmapPass : public RenderPass {
	public:
		ShadowmapPass(Scene *scene);
		ShadowmapPass(Scene *scene, Framebuffer *customDirectionalLightShadowFramebuffer, Framebuffer *customSpotLightShadowFramebuffer, Framebuffer *customPointLightShadowFramebuffer);
		virtual ~ShadowmapPass() override;

		ShadowmapPassOutput generateShadowmaps(ICamera *camera, bool renderOnlyStatic);
	private:
		Shader *m_ShadowmapShader;
		CubemapCamera m_CubemapCamera;

		// Option to use custom shadow framebuffers. Most will go through the light manager and request the default resolution framebuffers
		Framebuffer *m_CustomDirectionalLightShadowFramebuffer;
		Framebuffer *m_CustomSpotLightShadowFramebuffer;
		Framebuffer* m_CustomPointLightShadowFramebuffer;
	};
}
