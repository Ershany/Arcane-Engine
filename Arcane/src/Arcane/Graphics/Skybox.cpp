#include "arcpch.h"
#include "Skybox.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Texture/Cubemap.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Platform/OpenGL/Buffer.h>
#include <Arcane/Platform/OpenGL/IndexBuffer.h>
#include <Arcane/Platform/OpenGL/VertexArray.h>
#include <Arcane/Util/Loaders/TextureLoader.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>

namespace Arcane
{
	Skybox::Skybox(const std::vector<std::string> &filePaths) {
		m_SkyboxShader = ShaderLoader::LoadShader("Skybox.glsl");

		CubemapSettings srgbCubemap;
		srgbCubemap.IsSRGB = true;
		m_SkyboxCubemap = TextureLoader::LoadCubemapTexture(filePaths[0], filePaths[1], filePaths[2], filePaths[3], filePaths[4], filePaths[5], &srgbCubemap);

		m_GLCache = GLCache::GetInstance();
	}

	void Skybox::Draw(ICamera *camera) {
		m_GLCache->SetShader(m_SkyboxShader);

		// Pass the texture to the shader
		m_SkyboxCubemap->Bind(0);
		m_SkyboxShader->SetUniform("skyboxCubemap", 0);

		m_SkyboxShader->SetUniform("view", camera->GetViewMatrix());
		m_SkyboxShader->SetUniform("projection", camera->GetProjectionMatrix());

		// Since the vertex shader is gonna make the depth value 1.0, and the default value in the depth buffer is 1.0 so this is needed to draw the sky  box
		m_GLCache->SetDepthTest(true);
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetCullFace(GL_FRONT);
		m_GLCache->SetDepthFunc(GL_LEQUAL);
		
		cube.Draw();

		m_GLCache->SetCullFace(GL_BACK);
		m_GLCache->SetDepthFunc(GL_LESS);

		m_SkyboxCubemap->Unbind();
	}
}
