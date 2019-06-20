#pragma once

#include <graphics/camera/CubemapCamera.h>
#include <graphics/renderer/renderpass/RenderPass.h>
#include <graphics/renderer/renderpass/ShadowmapPass.h>
#include <graphics/Shader.h>
#include <scene/Scene3D.h>

namespace arcane {

	class ForwardProbePass : public RenderPass {
	public:
		ForwardProbePass(Scene3D *scene);
		virtual ~ForwardProbePass() override;

		void pregenerateIBL();
		void pregenerateProbes();

		void generateLightProbe(glm::vec3& probePosition);
		void generateReflectionProbe(glm::vec3& probePosition);
	private:
		void generateBRDFLUT();
		void generateFallbackProbes();
	private:
		Framebuffer m_SceneCaptureShadowFramebuffer, m_SceneCaptureLightingFramebuffer, m_LightProbeConvolutionFramebuffer, m_ReflectionProbeSamplingFramebuffer;
		CubemapCamera m_CubemapCamera;
		CubemapSettings m_SceneCaptureSettings;
		Cubemap m_SceneCaptureCubemap;

		Shader *m_ConvolutionShader, *m_ImportanceSamplingShader;
	};

}
