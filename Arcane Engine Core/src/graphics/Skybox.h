#pragma once

#include "Shader.h"
#include "Window.h"

#include <graphics/camera/FPSCamera.h>
#include <graphics/renderer/GLCache.h>
#include <platform/OpenGL/Buffer.h>
#include <platform/OpenGL/IndexBuffer.h>
#include <platform/OpenGL/VertexArray.h>
#include <utils/loaders/TextureLoader.h>

namespace arcane {

	class Skybox {
	public:
		Skybox(const std::vector<std::string> &filePaths);

		void Draw(ICamera *camera);

		Cubemap* getSkyboxCubemap() { return m_SkyboxCubemap; }
	private:
		Shader *m_SkyboxShader;
		GLCache *m_GLCache;
		
		VertexArray m_SkyboxVAO;
		IndexBuffer m_SkyboxIBO;
		Buffer  m_SkyboxVBO;
		Cubemap *m_SkyboxCubemap;
	};

}
