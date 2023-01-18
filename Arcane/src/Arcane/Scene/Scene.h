#pragma once

#include <Arcane/Graphics/Camera/FPSCamera.h>
#include <Arcane/Graphics/Lights/DynamicLightManager.h>
#include <Arcane/Graphics/IBL/ProbeManager.h>
#include <Arcane/Graphics/Renderer/ModelRenderer.h>
#include <Arcane/Terrain/Terrain.h>

#include "entt.hpp"

namespace Arcane
{
	class Entity;
	class Window;
	class Skybox;
	class GLCache;
	class RenderableModel;

	class Scene
	{
		friend class Entity;
	public:
		Scene(Window *window);
		~Scene();

		Entity CreateEntity(const std::string &name = std::string());

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

		std::vector<RenderableModel*>::iterator GetRenderableModelsBegin() { return m_RenderableModels.begin(); }
		std::vector<RenderableModel*>::iterator GetRenderableModelsEnd() { return m_RenderableModels.end(); }
	private:
		void Init();
	private:
		// Global Data
		GLCache *m_GLCache;

		// Collection of entities in our scene
		entt::registry m_Registry;

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
