#pragma once

#include <graphics/Shader.h>
#include <graphics/texture/Cubemap.h>

namespace arcane {

	class ReflectionProbe {
	public:
		ReflectionProbe(glm::vec3 &probePosition, glm::vec2 &probeResolution, bool isStatic);
		~ReflectionProbe();

		void generate();

		// Assumes the shader is bound
		void bind(Shader *shader);

		// Getters
		inline Cubemap* getPrefilterMap() { return m_PrefilterMap; }
	private:
		Cubemap *m_PrefilterMap;
		//static Cubemap *m_BRDF_LUT;

		glm::vec3 m_Position;
		glm::vec2 m_ProbeResolution;
		bool m_IsStatic;
		bool m_Generated;
	};

}
