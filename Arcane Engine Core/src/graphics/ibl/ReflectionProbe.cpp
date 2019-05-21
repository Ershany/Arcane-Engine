#include "pch.h"
#include "ReflectionProbe.h"

namespace arcane {

	ReflectionProbe::ReflectionProbe(glm::vec3 &probePosition, glm::vec2 &probeResolution, bool isStatic)
		: m_Position(probePosition), m_ProbeResolution(probeResolution), m_IsStatic(isStatic), m_PrefilterMap(nullptr)
	{}

	ReflectionProbe::~ReflectionProbe() {
		delete m_PrefilterMap;
	}

	void ReflectionProbe::generate() {
		// Generate the HDR environment probe and set the generated flag
		CubemapSettings settings;
		settings.GenerateMips = true;

		m_PrefilterMap = new Cubemap(settings);
		for (int i = 0; i < 6; i++) {
			m_PrefilterMap->generateCubemapFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, (unsigned int)m_ProbeResolution.x, (unsigned int)m_ProbeResolution.y, GL_RGBA16F, GL_RGB, nullptr);
		}

		m_Generated = true;
	}

	void ReflectionProbe::bind(Shader *shader) {
		m_PrefilterMap->bind(2);
		shader->setUniform1i("prefilterMap", 2);
		//m_BRDF_LUT->bind(3);
		//shader->setUniform1i("brdfLUT", 3);
	}

}
