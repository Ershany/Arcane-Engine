#pragma once

#include "../../Defs.h"
#include "../mesh/common/Quad.h"
#include "Renderer.h"
#include "../Shader.h"
#include "../../platform/OpenGL/Framebuffers/RenderTarget.h"
#include "../../utils/Timer.h"
#include "../../ui/DebugPane.h"
#include "../../ui/RuntimePane.h"

namespace arcane { namespace graphics {

	class PostProcessor {
	public:
		PostProcessor(Renderer *renderer);
		~PostProcessor();

		void preLightingPostProcess();
		// Assumes the input RenderTarget is bound, after function runs the default screen RenderTarget is bound
		void postLightingPostProcess(opengl::RenderTarget *input);

		// Might be useful to have if we want to have more custom post processing. Unity does it this way
		//void blit(Texture *texture, opengl::RenderTarget *source);

		inline void EnableBlur(bool choice) { m_Blur = choice; }
	private:
		float m_GammaCorrection = 2.2f;

		Renderer *m_Renderer;
		Shader m_PostProcessShader;
		Quad m_NDC_Plane;
		opengl::RenderTarget m_ScreenRenderTarget;
		Timer m_Timer;

		bool m_Blur = false;
	};

} }