#pragma once
#ifndef FORWARDPROBEPASS_H
#define FORWARDPROBEPASS_H

#ifndef CUBEMAP_H
#include <Arcane/Graphics/Texture/Cubemap.h>
#endif

#ifndef CUBEMAPCAMERA_H
#include <Arcane/Graphics/Camera/CubemapCamera.h>
#endif

#ifndef RENDERPASS_H
#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>
#endif

#ifndef FRAMEBUFFER_H
#include <Arcane/Platform/OpenGL/Framebuffer/Framebuffer.h>
#endif

namespace Arcane
{
	class Shader;
	class Scene;

	class ForwardProbePass : public RenderPass {
	public:
		ForwardProbePass(Scene *scene);
		virtual ~ForwardProbePass() override;

		void pregenerateIBL();
		void pregenerateProbes();

		void generateLightProbe(glm::vec3& probePosition);
		void generateReflectionProbe(glm::vec3& probePosition);
	private:
		void generateBRDFLUT();
		void generateFallbackProbes();
	private:
		Framebuffer m_SceneCaptureDirLightShadowFramebuffer, m_SceneCaptureSpotLightShadowFramebuffer, m_SceneCaptureLightingFramebuffer, m_LightProbeConvolutionFramebuffer, m_ReflectionProbeSamplingFramebuffer;
		Cubemap m_SceneCapturePointLightDepthCubemap;
		CubemapCamera m_CubemapCamera;
		CubemapSettings m_SceneCaptureSettings;
		Cubemap m_SceneCaptureCubemap;

		Shader *m_ConvolutionShader, *m_ImportanceSamplingShader;
	};
}
#endif
