#pragma once

#include <graphics/renderer/renderpass/RenderPass.h>
#include <graphics/Shader.h>
#include <scene/Scene3D.h>

namespace arcane
{

	class PostProcessPass : public RenderPass
	{
	public:
		PostProcessPass(Scene3D *scene);
		virtual ~PostProcessPass() override;

		void executeRenderPass();
	private:
		Shader m_PostProcessShader;
	};

}
