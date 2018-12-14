#pragma once

#include "Shader.h"
#include "Window.h"

#include <graphics/camera/FPSCamera.h>
#include <graphics/renderer/GLCache.h>
#include <platform/OpenGL/Buffer.h>
#include <platform/OpenGL/IndexBuffer.h>
#include <platform/OpenGL/VertexArray.h>
#include <utils/loaders/TextureLoader.h>

namespace arcane { namespace graphics {

	class Skybox {
	public:
		Skybox(const std::vector<std::string> &filePaths, FPSCamera *camera);

		void Draw();

		graphics::Cubemap* getSkyboxCubemap() { return m_SkyboxCubemap; }
	private:
		FPSCamera *m_Camera;
		Shader m_SkyboxShader;
		GLCache *m_GLCache;
		
		opengl::VertexArray m_SkyboxVAO;
		opengl::IndexBuffer m_SkyboxIBO;
		opengl::Buffer  m_SkyboxVBO;
		graphics::Cubemap *m_SkyboxCubemap;
	};

} }