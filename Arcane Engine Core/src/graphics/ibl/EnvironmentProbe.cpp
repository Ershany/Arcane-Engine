#include "pch.h"
#include "EnvironmentProbe.h"

namespace arcane {

	EnvironmentProbe::EnvironmentProbe(glm::vec3 &probePosition, glm::vec2 &probeResolution, bool isStatic)
		: m_Position(probePosition), m_ProbeResolution(probeResolution), m_IsStatic(isStatic)
	{}

	void EnvironmentProbe::generate() {
		// Generate the environment probe and set the generated flag
		CubemapSettings settings;
		m_IrradianceMap = new Cubemap(settings);
		for (int i = 0; i < 6; i++) {
			m_IrradianceMap->generateCubemapFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, (unsigned int)m_ProbeResolution.x, (unsigned int)m_ProbeResolution.y, GL_RGBA16F, GL_RGBA16F, nullptr);
		}

		m_Generated = true;
	}

	void EnvironmentProbe::bind(Shader &shader) {
		m_IrradianceMap->bind(1);
		shader.setUniform1i("irradianceMap", 1);
		m_PrefilterMap->bind(2);
		shader.setUniform1i("prefilterMap", 2);
		m_BRDF_LUT->bind(3);
		shader.setUniform1i("brdfLUT", 3);
	}

}
