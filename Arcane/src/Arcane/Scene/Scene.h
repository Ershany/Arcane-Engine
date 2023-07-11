#pragma once
#ifndef SCENE_H
#define SCENE_H

#ifndef FPSCAMERA_H
#include <Arcane/Graphics/Camera/FPSCamera.h>
#endif

#ifndef LIGHTMANAGER_H
#include <Arcane/Graphics/Lights/LightManager.h>
#endif

#ifndef WATERMANAGER_H
#include <Arcane/Graphics/Water/WaterManager.h>
#endif

#ifndef PROBEMANAGER_H
#include <Arcane/Graphics/IBL/ProbeManager.h>
#endif

#ifndef TERRAIN_H
#include <Arcane/Terrain/Terrain.h>
#endif

#ifndef WATERPASS_H
#include <Arcane/Graphics/Renderer/Renderpass/WaterPass.h>
#endif

#ifndef ENTT_CONFIG_CONFIG_H
#include "entt.hpp"
#endif

namespace Arcane
{
	class Entity;
	class Window;
	class Skybox;
	class GLCache;

	enum class ModelFilterType
	{
		AllModels,
		StaticModels,
		OpaqueModels,
		OpaqueStaticModels,
		TransparentModels,
		TransparentStaticModels
	};

	class Scene
	{
		friend class EditorPass;
		friend class Entity;
		friend class LightManager;
		friend class WaterManager;
		friend class ScenePanel;
		friend class WaterPass;
	public:
		Scene(Window *window);
		~Scene();

		Entity CreateEntity(const std::string &name = std::string());

		void Init();
		void OnUpdate(float deltaTime);

		void AddModelsToRenderer(ModelFilterType filter);
		void AddSkinnedModelsToRenderer(ModelFilterType filter);

		inline Terrain* GetTerrain() { return &m_Terrain; }
		inline LightManager* GetLightManager() { return &m_LightManager; }
		inline WaterManager* GetWaterManager() { return &m_WaterManager; }
		inline ProbeManager* GetProbeManager() { return &m_ProbeManager; }
		inline FPSCamera* GetCamera() { return &m_SceneCamera; }
		inline Skybox* GetSkybox() { return m_Skybox; }
	private:
		void PreInit();
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
		Terrain m_Terrain;
		LightManager m_LightManager;
		ProbeManager m_ProbeManager;
		WaterManager m_WaterManager;
	};
}
#endif
