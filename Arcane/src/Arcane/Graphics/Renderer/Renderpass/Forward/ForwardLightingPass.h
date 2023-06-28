#pragma once
#ifndef FORWARDLIGHTINGPASS_H
#define FORWARDLIGHTINGPASS_H

#ifndef RENDERPASS_H
#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>
#endif

#ifndef RENDERPASSTYPE_H
#include <Arcane/Graphics/Renderer/Renderpass/RenderPassType.h>
#endif

namespace Arcane
{
	class Shader;
	class Scene;
	class ICamera;
	class Framebuffer;

	class ForwardLightingPass : public RenderPass {
	public:
		ForwardLightingPass(Scene *scene, bool shouldMultisample);
		ForwardLightingPass(Scene *scene, Framebuffer *customFramebuffer);
		virtual ~ForwardLightingPass() override;

		LightingPassOutput ExecuteOpaqueLightingPass(ShadowmapPassOutput &inputShadowmapData, ICamera *camera, bool renderOnlyStatic, bool useIBL);
		LightingPassOutput ExecuteTransparentLightingPass(ShadowmapPassOutput &inputShadowmapData, Framebuffer *inputFramebuffer, ICamera *camera, bool renderOnlyStatic, bool useIBL);
	private:
		void Init();

		void BindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData);
	private:
		bool m_AllocatedFramebuffer;
		Framebuffer *m_Framebuffer;
		Shader *m_ModelShader, *m_SkinnedModelShader, *m_TerrainShader;
	};
}
#endif
