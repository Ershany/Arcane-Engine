#pragma once

#include <graphics/Shader.h>
#include <graphics/texture/Cubemap.h>

namespace arcane {
	
	class LightProbe {
	public:
		LightProbe(glm::vec3 &probePosition, glm::vec2 &probeResolution);
		~LightProbe();

		void generate();

		// Assumes the shader is bound
		void bind(Shader *shader);

		// Getters
		inline glm::vec3& getPosition() { return m_Position; }
		inline Cubemap* getIrradianceMap() { return m_IrradianceMap; }
	private:
		Cubemap *m_IrradianceMap;

		glm::vec3 m_Position;
		glm::vec2 m_ProbeResolution;
		bool m_Generated;
	};

}
