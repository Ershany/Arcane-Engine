#include "pch.h"
#include "DeferredPostProcessPass.h"

#include <utils/loaders/ShaderLoader.h>

namespace arcane {

	DeferredPostProcessPass::DeferredPostProcessPass(Scene3D *scene) : RenderPass(scene, RenderPassType::PostProcessPassType)
	{
		m_PostProcessShader = ShaderLoader::loadShader("src/shaders/postprocess.vert", "src/shaders/postprocess.frag");

		DebugPane::bindGammaCorrectionValue(&m_GammaCorrection);
	}

	DeferredPostProcessPass::~DeferredPostProcessPass() {}

	void DeferredPostProcessPass::executePostLightingPass(Framebuffer *framebufferToProcess) {
		glViewport(0, 0, Window::getWidth(), Window::getHeight());

#if DEBUG_ENABLED
		if (DebugPane::getWireframeMode())
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

		// Bind shader and its post processing settings, and also bind the screenspace texture
		framebufferToProcess->unbind();
		GLCache::getInstance()->switchShader(m_PostProcessShader);
		m_PostProcessShader->setUniform1f("gamma_inverse", 1.0f / m_GammaCorrection);
		m_PostProcessShader->setUniform2f("read_offset", glm::vec2(1.0f / (float)framebufferToProcess->getWidth(), 1.0f / (float)framebufferToProcess->getHeight()));
		m_PostProcessShader->setUniform1i("blur_enabled", m_Blur);
		m_PostProcessShader->setUniform1i("screen_texture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, framebufferToProcess->getColourBufferTexture());

		Window::clear();
		ModelRenderer * modelRenderer = m_ActiveScene->getModelRenderer();
		modelRenderer->NDC_Plane.Draw();
	}

}
