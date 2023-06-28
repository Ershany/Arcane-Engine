#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H

#ifndef CUBE_H
#include <Arcane/Graphics/Mesh/Common/Cube.h>
#endif

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
	private:
		Shader *m_SkyboxShader;
		GLCache *m_GLCache;
		
		Cube cube;
		Cubemap *m_SkyboxCubemap;
	};
}
#endif
