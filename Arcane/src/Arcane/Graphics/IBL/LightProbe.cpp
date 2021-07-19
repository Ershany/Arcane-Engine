#include "arcpch.h"
#include "LightProbe.h"

#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Texture/Cubemap.h>

namespace Arcane
{
	LightProbe::LightProbe(glm::vec3 &probePosition, glm::vec2 &probeResolution)
		: m_Position(probePosition), m_ProbeResolution(probeResolution), m_IrradianceMap(nullptr), m_Generated(false)
	{}

	LightProbe::~LightProbe() {
		delete m_IrradianceMap;
	}

	void LightProbe::Generate() {
		// Generate the HDR light probe and set the generated flag
		CubemapSettings settings;
		settings.TextureFormat = GL_RGBA16F;
		m_IrradianceMap = new Cubemap(settings);
		for (int i = 0; i < 6; i++) {
			m_IrradianceMap->GenerateCubemapFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, (unsigned int)m_ProbeResolution.x, (unsigned int)m_ProbeResolution.y, GL_RGB, nullptr);
		}

		m_Generated = true;
	}

	void LightProbe::Bind(Shader *shader) {
		m_IrradianceMap->Bind(1);
		shader->SetUniform("irradianceMap", 1);
	}
}
