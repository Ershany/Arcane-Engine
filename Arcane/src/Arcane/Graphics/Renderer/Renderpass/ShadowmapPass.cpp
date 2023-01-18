#include "arcpch.h"
#include "ShadowmapPass.h"

#include <Arcane/Scene/Scene.h>
#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>

namespace Arcane
{
	ShadowmapPass::ShadowmapPass(Scene *scene) : RenderPass(scene), m_AllocatedFramebuffer(true)
	{
		m_ShadowmapShader = ShaderLoader::LoadShader("Shadowmap_Generation.glsl");

		m_ShadowmapFramebuffer = new Framebuffer(SHADOWMAP_RESOLUTION_X, SHADOWMAP_RESOLUTION_Y, false);
		m_ShadowmapFramebuffer->AddDepthStencilTexture(NormalizedDepthOnly).CreateFramebuffer();
	}

	ShadowmapPass::ShadowmapPass(Scene *scene, Framebuffer *customFramebuffer) : RenderPass(scene), m_AllocatedFramebuffer(false), m_ShadowmapFramebuffer(customFramebuffer)
	{
		m_ShadowmapShader = ShaderLoader::LoadShader("Shadowmap_Generation.glsl");
	}

	ShadowmapPass::~ShadowmapPass() {
		if (m_AllocatedFramebuffer)
			delete m_ShadowmapFramebuffer;
	}

	ShadowmapPassOutput ShadowmapPass::generateShadowmaps(ICamera *camera, bool renderOnlyStatic) {
		glViewport(0, 0, m_ShadowmapFramebuffer->GetWidth(), m_ShadowmapFramebuffer->GetHeight());
		m_ShadowmapFramebuffer->Bind();
		m_ShadowmapFramebuffer->Clear();

		// Setup
		ModelRenderer *modelRenderer = m_ActiveScene->GetModelRenderer();
		Terrain *terrain = m_ActiveScene->GetTerrain();
		DynamicLightManager *lightManager = m_ActiveScene->GetDynamicLightManager();

		// View setup
		m_GLCache->SetShader(m_ShadowmapShader);
		glm::vec3 dirLightShadowmapLookAtPos = camera->GetPosition();
		glm::vec3 dirLightShadowmapEyePos = dirLightShadowmapLookAtPos - (lightManager->GetDirectionalLightDirection(0) * 50.0f);
		glm::mat4 directionalLightProjection = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, SHADOWMAP_NEAR_PLANE, SHADOWMAP_FAR_PLANE);
		glm::mat4 directionalLightView = glm::lookAt(dirLightShadowmapEyePos, dirLightShadowmapLookAtPos, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 directionalLightViewProjMatrix = directionalLightProjection * directionalLightView;
		m_ShadowmapShader->SetUniform("lightSpaceViewProjectionMatrix", directionalLightViewProjMatrix);

		// Setup model renderer
		if (renderOnlyStatic) {
			m_ActiveScene->AddStaticModelsToRenderer();
		}
		else {
			m_ActiveScene->AddModelsToRenderer();
		}

		// Render models
		m_GLCache->SetDepthTest(true);
		m_GLCache->SetBlend(false);
		m_GLCache->SetFaceCull(false);
		modelRenderer->FlushOpaque(m_ShadowmapShader, NoMaterialRequired);
		modelRenderer->FlushTransparent(m_ShadowmapShader, NoMaterialRequired);

		// Render terrain
		terrain->Draw(m_ShadowmapShader, NoMaterialRequired);

		// Render pass output
		ShadowmapPassOutput passOutput;
		passOutput.directionalLightViewProjMatrix = directionalLightViewProjMatrix;
		passOutput.shadowmapFramebuffer = m_ShadowmapFramebuffer;
		return passOutput;
	}
}
