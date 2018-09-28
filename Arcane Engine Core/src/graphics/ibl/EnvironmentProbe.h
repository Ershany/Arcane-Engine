#pragma once

#include <glm\common.hpp>

#include "../texture/Cubemap.h"
#include "../Shader.h"

namespace arcane { namespace graphics {

	class EnvironmentProbe {
	public:
		EnvironmentProbe(glm::vec3 &probePosition, glm::vec2 &probeResolution);

		void generate();

		// Assumes the shader is bound
		void bind(Shader &shader);
	private:
		Cubemap *m_IrradianceMap, *m_PrefilterMap, *m_BRDF_LUT;
		glm::vec3 m_GeneratedPosition;
		bool m_Generated;
	};

} }
