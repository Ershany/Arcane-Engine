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
		ShadowmapPass(Scene *scene, Framebuffer *customFramebuffer);
		virtual ~ShadowmapPass() override;

		ShadowmapPassOutput generateShadowmaps(ICamera *camera, bool renderOnlyStatic);
	private:
		Framebuffer *m_CustomShadowFramebuffer; // Only used by some passes. Most will go through the light manager and request the proper framebuffers
		Shader *m_ShadowmapShader;
	};
}
