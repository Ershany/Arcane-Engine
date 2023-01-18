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
		bool m_AllocatedFramebuffer;
		Framebuffer *m_ShadowmapFramebuffer;
		Shader *m_ShadowmapShader;
	};
}
