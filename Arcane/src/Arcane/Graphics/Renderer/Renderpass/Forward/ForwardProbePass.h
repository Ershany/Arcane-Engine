#pragma once

#include <Arcane/Graphics/Texture/Cubemap.h>
#include <Arcane/Graphics/Camera/CubemapCamera.h>
#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/ShadowmapPass.h>

namespace Arcane
{
	class Shader;
	class Scene3D;

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
