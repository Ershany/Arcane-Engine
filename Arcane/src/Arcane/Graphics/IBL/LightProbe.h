#pragma once

namespace Arcane
{
	class Shader;
	class Cubemap;

	class LightProbe {
	public:
		LightProbe(glm::vec3 &probePosition, glm::vec2 &probeResolution);
		~LightProbe();

		void Generate();

		// Assumes the shader is bound
		void Bind(Shader *shader);

		// Getters
		inline glm::vec3& GetPosition() { return m_Position; }
		inline Cubemap* GetIrradianceMap() { return m_IrradianceMap; }
	private:
		Cubemap *m_IrradianceMap;

		glm::vec3 m_Position;
		glm::vec2 m_ProbeResolution;
		bool m_Generated;
	};
}
