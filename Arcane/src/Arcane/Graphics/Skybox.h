#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H

namespace Arcane
{
	class Shader;
	class Cubemap;
	class GLCache;
	class ICamera;

	class Skybox {
	public:
		Skybox(std::vector<std::string> &filePaths);

		void Draw(ICamera *camera);

		Cubemap* GetSkyboxCubemap() { return m_SkyboxCubemap; }

		// Bindings
		inline glm::vec3& GetTintColourRef() { return m_TintColour; }
		inline float& GetLightIntensityRef() { return m_LightIntensity; }
	private:
		Shader *m_SkyboxShader;
		GLCache *m_GLCache;

		Cubemap *m_SkyboxCubemap;

		// Settings to tune skybox look
		glm::vec3 m_TintColour = glm::vec3(1.0f, 1.0f, 1.0f);
		float m_LightIntensity = 1.0f;
	};
}
#endif
