#include "pch.h"
#include "PostProcessPass.h"

namespace arcane
{

	PostProcessPass::PostProcessPass(Scene3D *scene) : RenderPass(scene, RenderPassType::PostProcessPass),
		m_PostProcessShader("src/shaders/postprocess.vert", "src/shaders/postprocess.frag")
	{

	}

	PostProcessPass::~PostProcessPass() {}

	void PostProcessPass::executeRenderPass() {

	}

}
