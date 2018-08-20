#pragma once

#include "graphics\renderer\Renderable3D.h"
#include "Defs.h"
#include "graphics\camera\Camera.h"
#include "graphics\renderer\Renderer.h"
#include "graphics\renderer\GLCache.h"
#include "terrain\Terrain.h"
#include "graphics\Window.h"
#include "graphics\Skybox.h"
#include "graphics\dynamic lights\DynamicLightManager.h"
#include "utils\loaders\TextureLoader.h"

namespace arcane {
	
	class Scene3D {
	private:
		graphics::Camera *m_Camera;
		graphics::Renderer *m_Renderer;
		terrain::Terrain *m_Terrain;
		graphics::Skybox *m_Skybox;
		graphics::DynamicLightManager m_DynamicLightManager;
		graphics::GLCache *m_GLCache;

		std::vector<graphics::Renderable3D*> m_Renderables;

		graphics::Shader m_TerrainShader, m_ModelShader, m_ShadowmapShader;
	public:
		Scene3D(graphics::Camera *camera, graphics::Window *window);
		~Scene3D();
		
		void add(graphics::Renderable3D *renderable);

		// Passes
		void shadowmapPass();

		void onUpdate(float deltaTime);
		void onRender(unsigned int shadowmap);

		inline graphics::Renderer* getRenderer() const { return m_Renderer; }
		inline graphics::Camera* getCamera() const { return m_Camera; }
	private:
		void init();

		void addObjectsToRenderQueue();
	};

}