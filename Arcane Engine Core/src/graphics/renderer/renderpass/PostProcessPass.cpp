#include "pch.h"
#include "PostProcessPass.h"

#include <utils/loaders/ShaderLoader.h>

namespace arcane {

	PostProcessPass::PostProcessPass(Scene3D *scene) : RenderPass(scene), m_TonemappedNonLinearTarget(Window::getWidth(), Window::getHeight(), false),
		m_ScreenRenderTarget(Window::getWidth(), Window::getHeight(), false), m_ResolveRenderTarget(Window::getResolutionWidth(), Window::getResolutionHeight(), false)
	{
		m_PostProcessShader = ShaderLoader::loadShader("src/shaders/postprocess.vert", "src/shaders/postprocess.frag");
		m_FxaaShader = ShaderLoader::loadShader("src/shaders/fxaa.vert", "src/shaders/fxaa.frag");

		m_TonemappedNonLinearTarget.addColorTexture(Normalized8).addDepthStencilRBO(NormalizedDepthOnly).createFramebuffer();
		m_ScreenRenderTarget.addColorTexture(FloatingPoint16).addDepthStencilRBO(NormalizedDepthOnly).createFramebuffer();
		m_ResolveRenderTarget.addColorTexture(FloatingPoint16).addDepthStencilRBO(NormalizedDepthOnly).createFramebuffer();
		DebugPane::bindGammaCorrectionValue(&m_GammaCorrection);
	}

	PostProcessPass::~PostProcessPass() {}

	void PostProcessPass::executePostProcessPass(Framebuffer *framebufferToProcess) {
		glViewport(0, 0, m_ScreenRenderTarget.getWidth(), m_ScreenRenderTarget.getHeight());

		// If the framebuffer is multi-sampled, resolve it
		Framebuffer *supersampledTarget = framebufferToProcess;
		if (framebufferToProcess->isMultisampled()) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferToProcess->getFramebuffer());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ResolveRenderTarget.getFramebuffer());
			glBlitFramebuffer(0, 0, framebufferToProcess->getWidth(), framebufferToProcess->getHeight(), 0, 0, m_ResolveRenderTarget.getWidth(), m_ResolveRenderTarget.getHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
			supersampledTarget = &m_ResolveRenderTarget;
		}

		// If some sort of super-sampling is set, we need to downsample (or upsample) our image to match the window's resolution
		Framebuffer *target = supersampledTarget;
		if (target->getWidth() != m_ScreenRenderTarget.getWidth() || target->getHeight() != m_ScreenRenderTarget.getHeight()) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, supersampledTarget->getFramebuffer());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ScreenRenderTarget.getFramebuffer());
			glBlitFramebuffer(0, 0, supersampledTarget->getWidth(), supersampledTarget->getHeight(), 0, 0, m_ScreenRenderTarget.getWidth(), m_ScreenRenderTarget.getHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
			target = &m_ScreenRenderTarget;
		}

#if DEBUG_ENABLED
		if (DebugPane::getWireframeMode())
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

		// Set post process settings and convert our scene from HDR (linear) -> SDR (non-linear)
		m_TonemappedNonLinearTarget.bind();
		m_TonemappedNonLinearTarget.clear();
		GLCache::getInstance()->switchShader(m_PostProcessShader);
		m_PostProcessShader->setUniform1f("gamma_inverse", 1.0f / m_GammaCorrection);
		m_PostProcessShader->setUniform2f("read_offset", glm::vec2(1.0f / (float)target->getWidth(), 1.0f / (float)target->getHeight()));
		m_PostProcessShader->setUniform1i("colour_texture", 0);
		target->getColourTexture()->bind(0);

		ModelRenderer *modelRenderer = m_ActiveScene->getModelRenderer();
		modelRenderer->NDC_Plane.Draw();

		// Finally render the scene to the widnow's framebuffer
		m_TonemappedNonLinearTarget.unbind();
		Window::clear();
		GLCache::getInstance()->switchShader(m_FxaaShader);
		m_FxaaShader->setUniform1i("enableFXAA", FXAA_ENABLE);
		m_FxaaShader->setUniform1i("showEdges", 0);
		m_FxaaShader->setUniform1f("lumaThreshold", 0.5f);
		m_FxaaShader->setUniform1f("mulReduction", 8.0f);
		m_FxaaShader->setUniform1f("minReduction", 128.0f);
		m_FxaaShader->setUniform1f("maxSpan", 8.0f);
		m_FxaaShader->setUniform1i("colour_texture", 0);
		m_TonemappedNonLinearTarget.getColourTexture()->bind(0);

		modelRenderer->NDC_Plane.Draw();
	}

}
