#pragma once

#include <vector>
#include "../platform/OpenGL/VertexArray.h"
#include "../platform/OpenGL/IndexBuffer.h"
#include "../platform/OpenGL/Buffer.h"
#include "../platform/OpenGL/Utility.h"
#include "Shader.h"
#include "camera\FPSCamera.h"
#include "Window.h"

namespace arcane { namespace graphics {

	class Skybox {
	public:
		Skybox(const std::vector<const char*> &filePaths, FPSCamera *camera, Window *window);

		void Draw();
	private:
		FPSCamera *m_Camera;
		Window *m_Window;
		Shader m_SkyboxShader;
		
		opengl::VertexArray m_SkyboxVAO;
		opengl::IndexBuffer m_SkyboxIBO;
		opengl::Buffer  m_SkyboxVBO;
		unsigned int m_SkyboxCubemap; // Cubemap texture
	};

} }