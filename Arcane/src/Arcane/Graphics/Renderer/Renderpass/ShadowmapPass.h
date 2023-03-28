#pragma once

#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>

namespace Arcane
{
	class ICamera;
	class Scene;
	class Shader;

	class ShadowmapPass : public RenderPass {
	public:
		ShadowmapPass(Scene *scene);
		ShadowmapPass(Scene *scene, Framebuffer *customDirectionalLightShadowFramebuffer, Framebuffer *customSpotLightShadowFramebuffer);
		virtual ~ShadowmapPass() override;

		ShadowmapPassOutput generateShadowmaps(ICamera *camera, bool renderOnlyStatic);
	private:
		Shader *m_ShadowmapShader;

		// Only used by some passes. Most will go through the light manager and request the proper framebuffers
		Framebuffer *m_CustomDirectionalLightShadowFramebuffer;
		Framebuffer *m_CustomSpotLightShadowFramebuffer;
	};
}
