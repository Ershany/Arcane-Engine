#pragma once

#include "ModelRenderer.h"

#include <graphics/Shader.h>
#include <graphics/mesh/common/Quad.h>
#include <platform/OpenGL/Framebuffers/Framebuffer.h>
#include <ui/DebugPane.h>
#include <ui/RuntimePane.h>
#include <utils/Timer.h>

namespace arcane {

	class PostProcessor {
	public:
		PostProcessor(ModelRenderer *meshRenderer);
		~PostProcessor();

		void preLightingPostProcess();
		// Assumes the input RenderTarget is bound, after function runs the default screen RenderTarget is bound
		void postLightingPostProcess(Framebuffer *input);

		// Might be useful to have if we want to have more custom post processing. Unity does it this way
		//void blit(Texture *texture, Framebuffer *source);

		inline void EnableBlur(bool choice) { m_Blur = choice; }
	private:
		float m_GammaCorrection = 2.2f;

		ModelRenderer *m_ModelRenderer;
		Shader m_PostProcessShader;
		Quad m_NDC_Plane;
		Framebuffer m_ScreenRenderTarget;
		Timer m_Timer;

		bool m_Blur = false;
	};

}
