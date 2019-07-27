#include "pch.h"
#include "ReflectionProbe.h"

namespace arcane {

	Texture* ReflectionProbe::s_BRDF_LUT = nullptr;

	ReflectionProbe::ReflectionProbe(glm::vec3 &probePosition, glm::vec2 &probeResolution)
		: m_Position(probePosition), m_ProbeResolution(probeResolution), m_Generated(false), m_PrefilterMap(nullptr)
	{}

	ReflectionProbe::~ReflectionProbe() {
		delete m_PrefilterMap;
	}

	void ReflectionProbe::generate() {
		// Generate the HDR reflection probe and set the generated flag
		CubemapSettings settings;
		settings.TextureFormat = GL_RGBA16F;
		settings.TextureMinificationFilterMode = GL_LINEAR_MIPMAP_LINEAR;
		settings.HasMips = true;

		m_PrefilterMap = new Cubemap(settings);
		for (int i = 0; i < 6; i++) {
			m_PrefilterMap->generateCubemapFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, (unsigned int)m_ProbeResolution.x, (unsigned int)m_ProbeResolution.y, GL_RGB, nullptr);
		}

		m_Generated = true;
	}

	void ReflectionProbe::bind(Shader *shader) {
		shader->setUniform("reflectionProbeMipCount", REFLECTION_PROBE_MIP_COUNT);
		
		m_PrefilterMap->bind(2);
		shader->setUniform("prefilterMap", 2);
		s_BRDF_LUT->bind(3);
		shader->setUniform("brdfLUT", 3);
	}

}
