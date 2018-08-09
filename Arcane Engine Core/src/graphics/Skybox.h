#pragma once

#include <vector>

#include "../platform/OpenGL/VertexArray.h"
#include "../platform/OpenGL/IndexBuffer.h"
#include "../platform/OpenGL/Buffer.h"
#include "../utils/loaders/TextureLoader.h"
#include "camera/Camera.h"
#include "Shader.h"
#include "Window.h"

namespace arcane { namespace graphics {

	class Skybox {
	public:
		Skybox(const std::vector<std::string> &filePaths, Camera *camera);

		void Draw();
	private:
		Camera *m_Camera;
		Shader m_SkyboxShader;
		
		opengl::VertexArray m_SkyboxVAO;
		opengl::IndexBuffer m_SkyboxIBO;
		opengl::Buffer  m_SkyboxVBO;
		graphics::Cubemap *m_SkyboxCubemap;
	};

} }