#pragma once

#include <graphics/renderer/renderpass/RenderPass.h>
#include <graphics/Shader.h>
#include <scene/Scene3D.h>

namespace arcane
{

	class LightingPass : public RenderPass
	{
	public:
		LightingPass(Scene3D *scene);
		virtual ~LightingPass() override;

		LightingPassOutput executeRenderPass(ShadowmapPassOutput shadowmapData);
	private:
		Framebuffer m_Framebuffer;
		Shader m_ModelShader, m_TerrainShader;
	};

}
