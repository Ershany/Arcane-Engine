#pragma once

#include <Arcane/Graphics/Mesh/Common/Cube.h>

namespace Arcane
{
	class Shader;
	class Cubemap;
	class GLCache;
	class ICamera;

	class Skybox {
	public:
		Skybox(const std::vector<std::string> &filePaths);

		void Draw(ICamera *camera);

		Cubemap* GetSkyboxCubemap() { return m_SkyboxCubemap; }
	private:
		Shader *m_SkyboxShader;
		GLCache *m_GLCache;
		
		Cube cube;
		Cubemap *m_SkyboxCubemap;
	};
}
