#pragma once

#include <graphics/Skybox.h>
#include <graphics/Window.h>
#include <graphics/camera/FPSCamera.h>
#include <graphics/dynamic lights/DynamicLightManager.h>
#include <graphics/renderer/GLCache.h>
#include <graphics/renderer/MeshRenderer.h>
#include <scene/SceneNode.h>
#include <terrain/Terrain.h>
#include <utils/loaders/TextureLoader.h>

namespace arcane {
	
	class Scene3D {
	private:
		graphics::FPSCamera *m_Camera;
		graphics::MeshRenderer *m_MeshRenderer;
		terrain::Terrain *m_Terrain;
		graphics::Skybox *m_Skybox;
		graphics::DynamicLightManager m_DynamicLightManager;
		graphics::GLCache *m_GLCache;

		std::vector<scene::SceneNode*> m_Renderables;

		graphics::Shader m_TerrainShader, m_ModelShader, m_ShadowmapShader;
	public:
		Scene3D(graphics::FPSCamera *camera, graphics::Window *window);
		~Scene3D();
		
		void add(scene::SceneNode *renderable);

		// Passes
		void shadowmapPass();

		void onUpdate(float deltaTime);
		void onRender(unsigned int shadowmap);

		inline graphics::MeshRenderer* getRenderer() const { return m_MeshRenderer; }
		inline graphics::FPSCamera* getCamera() const { return m_Camera; }
	private:
		void init();

		void addObjectsToRenderQueue();
	};

}