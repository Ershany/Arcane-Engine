#pragma once

#include <graphics/camera/CubemapCamera.h>
#include <graphics/ibl/EnvironmentProbe.h>
#include <platform/OpenGL/Framebuffers/Framebuffer.h>

namespace arcane
{
	
	class EnvironmentProbeManager
	{
	public:
		EnvironmentProbeManager();

		void GenerateProbe(glm::vec3 &worldPosition, glm::vec2 &probeResolution, bool isStaticProbe);
	private:
		Framebuffer m_CubemapGenerationFramebuffer;
		CubemapCamera m_CubemapCamera;
		std::vector<EnvironmentProbe*> m_EnvironmentProbes;
	};

}
