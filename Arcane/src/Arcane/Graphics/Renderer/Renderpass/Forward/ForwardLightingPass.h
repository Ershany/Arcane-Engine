#pragma once

#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>

namespace Arcane
{
	class Shader;
	class Scene;
	class ICamera;

	class ForwardLightingPass : public RenderPass {
	public:
		ForwardLightingPass(Scene *scene, bool shouldMultisample);
		ForwardLightingPass(Scene *scene, Framebuffer *customFramebuffer);
		virtual ~ForwardLightingPass() override;

		LightingPassOutput executeLightingPass(ShadowmapPassOutput &shadowmapData, ICamera *camera, bool renderOnlyStatic, bool useIBL);
	private:
		void bindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData);
	private:
		bool m_AllocatedFramebuffer;
		Framebuffer *m_Framebuffer;
		Shader *m_ModelShader, *m_TerrainShader;
	};
}
