#pragma once

#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>

namespace Arcane
{
	class ICamera;
	class Scene3D;
	class Shader;

	class ShadowmapPass : public RenderPass {
	public:
		ShadowmapPass(Scene3D *scene);
		ShadowmapPass(Scene3D *scene, Framebuffer *customFramebuffer);
		virtual ~ShadowmapPass() override;

		ShadowmapPassOutput generateShadowmaps(ICamera *camera, bool renderOnlyStatic);
	private:
		bool m_AllocatedFramebuffer;
		Framebuffer *m_ShadowmapFramebuffer;
		Shader *m_ShadowmapShader;
	};
}
