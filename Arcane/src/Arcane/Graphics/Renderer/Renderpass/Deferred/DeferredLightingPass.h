#pragma once

#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>

namespace Arcane
{
	class Shader;
	class Scene3D;
	class ICamera;

	class DeferredLightingPass : public RenderPass {
	public:
		DeferredLightingPass(Scene3D *scene);
		DeferredLightingPass(Scene3D *scene, Framebuffer *framebuffer);
		virtual ~DeferredLightingPass() override;

		LightingPassOutput executeLightingPass(ShadowmapPassOutput &shadowmapData, GeometryPassOutput &geometryData, PreLightingPassOutput &preLightingOutput, ICamera *camera, bool useIBL);
	private:
		void bindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData);
	private:
		bool m_AllocatedFramebuffer;
		Framebuffer *m_Framebuffer;
		Shader *m_LightingShader;
	};
}
