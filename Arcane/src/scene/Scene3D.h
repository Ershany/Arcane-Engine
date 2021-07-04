#pragma once

#include <graphics/Skybox.h>
#include <graphics/Window.h>
#include <graphics/camera/FPSCamera.h>
#include <graphics/lights/DynamicLightManager.h>
#include <graphics/ibl/ProbeManager.h>
#include <graphics/renderer/GLCache.h>
#include <graphics/renderer/ModelRenderer.h>
#include <scene/RenderableModel.h>
#include <terrain/Terrain.h>
#include <utils/loaders/TextureLoader.h>

namespace Arcane
{
	class Scene3D {
	public:
		Scene3D(Window *window);
		~Scene3D();

		void OnUpdate(float deltaTime);

		void AddModelsToRenderer();
		void AddStaticModelsToRenderer();
		void AddTransparentModelsToRenderer();
		void AddTransparentStaticModelsToRenderer();
		void AddOpaqueModelsToRenderer();
		void AddOpaqueStaticModelsToRenderer();

		inline ModelRenderer* GetModelRenderer() { return &m_ModelRenderer; }
		inline Terrain* GetTerrain() { return &m_Terrain; }
		inline DynamicLightManager* GetDynamicLightManager() { return &m_DynamicLightManager; }
		inline ProbeManager* GetProbeManager() { return &m_ProbeManager; }
		inline FPSCamera* GetCamera() { return &m_SceneCamera; }
		inline Skybox* GetSkybox() { return m_Skybox; }
	private:
		void Init();
	private:
		// Global Data
		GLCache *m_GLCache;

		// Scene parameters
		ProbeBlendSetting m_SceneProbeBlendSetting = PROBES_SIMPLE;

		// Scene Specific Data
		FPSCamera m_SceneCamera;
		Skybox *m_Skybox;
		ModelRenderer m_ModelRenderer;
		Terrain m_Terrain;
		DynamicLightManager m_DynamicLightManager;
		ProbeManager m_ProbeManager;
		std::vector<RenderableModel*> m_RenderableModels;
	};
}
