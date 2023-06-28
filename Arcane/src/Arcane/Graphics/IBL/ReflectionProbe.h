#pragma once
#ifndef REFLECTIONPROBE_H
#define REFLECTIONPROBE_H

namespace Arcane
{
	class Shader;
	class Texture;
	class Cubemap;

	class ReflectionProbe {
	public:
		ReflectionProbe(glm::vec3 &probePosition, glm::vec2 &probeResolution);
		~ReflectionProbe();

		void Generate();

		// Assumes the shader is bound
		void Bind(Shader *shader);

		// Getters
		inline glm::vec3& GetPosition() { return m_Position; }
		inline Cubemap* GetPrefilterMap() { return m_PrefilterMap; }
		static inline Texture* GetBRDFLUT() { return s_BRDF_LUT; }

		// Setters
		static void SetBRDFLUT(Texture *texture) { s_BRDF_LUT = texture; }
	private:
		Cubemap *m_PrefilterMap;
		static Texture *s_BRDF_LUT;

		glm::vec3 m_Position;
		glm::vec2 m_ProbeResolution;
		bool m_Generated;
	};
}
#endif
