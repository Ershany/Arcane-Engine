#pragma once

#include <platform/OpenGL/Framebuffers/Framebuffer.h>

namespace arcane {

	enum RenderPassType
	{
		ShadowmapPassType,
		LightingPassType,
		PostProcessPassType,
		EnvironmentProbePassType
	};

	struct ShadowmapPassOutput
	{
		glm::mat4 directionalLightViewProjMatrix;
		Framebuffer *shadowmapFramebuffer;
	};

	struct LightingPassOutput
	{
		Framebuffer *outputFramebuffer;
	};

}
