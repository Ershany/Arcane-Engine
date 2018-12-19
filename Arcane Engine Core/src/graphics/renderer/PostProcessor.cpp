#include "pch.h"
#include "PostProcessor.h"

namespace arcane {

	PostProcessor::PostProcessor(ModelRenderer *meshRenderer)
		: m_ModelRenderer(meshRenderer), m_PostProcessShader("src/shaders/postprocess.vert", "src/shaders/postprocess.frag"), m_ScreenRenderTarget(Window::getWidth(), Window::getHeight())
	{
		m_ScreenRenderTarget.addTexture2DColorAttachment(false).addDepthStencilRBO(false).createFramebuffer();
		DebugPane::bindGammaCorrectionValue(&m_GammaCorrection);
	}

	PostProcessor::~PostProcessor() {

	}

	void PostProcessor::preLightingPostProcess() {
		// SSAO goes here
	}

	void PostProcessor::postLightingPostProcess(Framebuffer *input) {
#if DEBUG_ENABLED
		glFinish();
		m_Timer.reset();
#endif

		// If the input RenderTarget is multi-sampled. Resolve it by blitting it to a non-multi-sampled RenderTarget
		Framebuffer *target = input;
		if (input->isMultisampledColourBuffer()) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, input->getFramebuffer());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ScreenRenderTarget.getFramebuffer());
			glBlitFramebuffer(0, 0, input->getWidth(), input->getHeight(), 0, 0, m_ScreenRenderTarget.getWidth(), m_ScreenRenderTarget.getHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
			target = &m_ScreenRenderTarget;
		}

#if DEBUG_ENABLED
		if (DebugPane::getWireframeMode())
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

		// Bind shader and its post processing settings, and also bind the screenspace texture
		glViewport(0, 0, Window::getWidth(), Window::getHeight());
		input->unbind();
		GLCache::getInstance()->switchShader(m_PostProcessShader.getShaderID());
		m_PostProcessShader.setUniform1f("gamma_inverse", 1.0f / m_GammaCorrection);
		m_PostProcessShader.setUniform2f("read_offset", glm::vec2(1.0f / (float)target->getWidth(), 1.0f / (float)target->getHeight()));
		m_PostProcessShader.setUniform1i("blur_enabled", m_Blur);
		m_PostProcessShader.setUniform1i("screen_texture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, target->getColourBufferTexture());

		Window::clear();
		m_ModelRenderer->NDC_Plane.Draw();

#if DEBUG_ENABLED
		glFinish();
		RuntimePane::setPostProcessTimer((float)m_Timer.elapsed());
#endif
	}

}