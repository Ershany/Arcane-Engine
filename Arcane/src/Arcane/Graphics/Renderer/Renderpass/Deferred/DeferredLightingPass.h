#pragma once

#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>

namespace Arcane
{
	class Shader;
	class Scene;
	class ICamera;

	class DeferredLightingPass : public RenderPass {
	public:
		DeferredLightingPass(Scene *scene);
		DeferredLightingPass(Scene *scene, Framebuffer *framebuffer);
		virtual ~DeferredLightingPass() override;

		LightingPassOutput ExecuteLightingPass(ShadowmapPassOutput &inputShadowmapData, GBuffer *inputGbuffer, PreLightingPassOutput &preLightingOutput, ICamera *camera, bool useIBL);
	private:
		void BindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData);
	private:
		bool m_AllocatedFramebuffer;
		Framebuffer *m_Framebuffer;
		Shader *m_LightingShader;
	};
}
