#include "PostProcessor.h"

namespace arcane { namespace graphics {

	PostProcessor::PostProcessor(Renderer *renderer) 
		: m_Renderer(renderer), m_PostProcessShader("src/shaders/postprocess.vert", "src/shaders/postprocess.frag"), m_ScreenRenderTarget(Window::getWidth(), Window::getHeight())
	{
		m_ScreenRenderTarget.addColorAttachment(false).addDepthStencilRBO(false).createFramebuffer();
	}

	PostProcessor::~PostProcessor() {

	}

	void PostProcessor::preLightingPostProcess() {
		// SSAO goes here
	}

	void PostProcessor::postLightingPostProcess(opengl::RenderTarget *input) {
		// TODO gamma correction and tone mapping for HDR need to be added to the post process shader
#if DEBUG_ENABLED
		glFinish();
		m_Timer.reset();
#endif

		// If the input RenderTarget is multi-sampled. Resolve it by blitting it to a non-multi-sampled RenderTarget
		opengl::RenderTarget *target = input;
		if (input->isMultisampledColourBuffer()) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, input->getFramebuffer());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ScreenRenderTarget.getFramebuffer());
			glBlitFramebuffer(0, 0, input->getWidth(), input->getHeight(), 0, 0, m_ScreenRenderTarget.getWidth(), m_ScreenRenderTarget.getHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
			target = &m_ScreenRenderTarget;
		}

#if DEBUG_ENABLED
		if (ui::DebugPane::getWireframeMode())
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

		// Bind shader and its post processing settings, and also bind the screenspace texture
		input->unbind();
		GLCache::getInstance()->switchShader(m_PostProcessShader.getShaderID());
		m_PostProcessShader.setUniform2f("read_offset", glm::vec2(1.0f / (float)target->getWidth(), 1.0f / (float)target->getHeight()));
		m_PostProcessShader.setUniform1i("blur_enabled", m_Blur);
		m_PostProcessShader.setUniform1i("screen_texture", 0);
		target->getColourBufferTexture()->bind(0);

		Window::clear();
		m_Renderer->NDC_Plane.Draw();

#if DEBUG_ENABLED
		glFinish();
		ui::RuntimePane::setPostProcessTimer(m_Timer.elapsed());
#endif
	}

} }