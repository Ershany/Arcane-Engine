#pragma once

#include <graphics/camera/CubemapCamera.h>
#include <graphics/renderer/renderpass/RenderPass.h>
#include <graphics/renderer/renderpass/ShadowmapPass.h>
#include <graphics/Shader.h>
#include <scene/Scene3D.h>

namespace arcane {

	class EnvironmentProbePass : public RenderPass {
	public:
		EnvironmentProbePass(Scene3D *scene);
		virtual ~EnvironmentProbePass() override;

		void pregenerateProbes();
	private:
		Framebuffer m_CubemapGenerationFramebuffer;
		CubemapCamera m_CubemapCamera;
	};

}
