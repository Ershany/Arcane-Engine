#include "arcpch.h"
#include "Skybox.h"

#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Texture/Cubemap.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Util/Loaders/AssetManager.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>
#include <Arcane/Graphics/Renderer/Renderer.h>

namespace Arcane
{
	Skybox::Skybox(std::vector<std::string> &filePaths)
	{
		m_SkyboxShader = ShaderLoader::LoadShader("Skybox.glsl");

		CubemapSettings srgbCubemap;
		srgbCubemap.IsSRGB = true;
		m_SkyboxCubemap = AssetManager::GetInstance().LoadCubemapTextureAsync(filePaths[0], filePaths[1], filePaths[2], filePaths[3], filePaths[4], filePaths[5], &srgbCubemap);

		m_GLCache = GLCache::GetInstance();
	}

	void Skybox::Draw(ICamera *camera)
	{
		// If light intensity is set to 0.0, then no need to even render the skybox since it won't be seen
		if (m_LightIntensity <= 0.0f)
		{
			return;
		}

		m_GLCache->SetShader(m_SkyboxShader);

		// Pass the texture to the shader
		m_SkyboxCubemap->Bind(0);
		m_SkyboxShader->SetUniform("skyboxCubemap", 0);

		// Setup uniforms
		m_SkyboxShader->SetUniform("view", camera->GetViewMatrix());
		m_SkyboxShader->SetUniform("projection", camera->GetProjectionMatrix());
		m_SkyboxShader->SetUniform("tintColour", m_TintColour);
		m_SkyboxShader->SetUniform("lightIntensity", m_LightIntensity);

		// Since the vertex shader is gonna make the depth value 1.0, and the default value in the depth buffer is 1.0 so this is needed to draw the sky  box
		m_GLCache->SetDepthTest(true);
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetCullFace(GL_FRONT);
		m_GLCache->SetDepthFunc(GL_LEQUAL);
		
		Renderer::DrawNdcCube();

		m_GLCache->SetCullFace(GL_BACK);
		m_GLCache->SetDepthFunc(GL_LESS);

		m_SkyboxCubemap->Unbind();
	}
}
