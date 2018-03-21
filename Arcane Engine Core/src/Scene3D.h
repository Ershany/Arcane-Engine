#pragma once

#include "graphics\renderer\Renderable3D.h"
#include "Defs.h"
#include "graphics\camera\Camera.h"
#include "graphics\renderer\Renderer.h"
#include "terrain\Terrain.h"
#include "graphics\Window.h"
#include "graphics\MeshFactory.h"
#include "graphics\Skybox.h"
#include "graphics\dynamic lights\DynamicLightManager.h"

namespace arcane {
	
	class Scene3D {
	private:
		graphics::Window *m_Window;
		graphics::Camera *m_Camera;
		graphics::Renderer *m_Renderer;
		terrain::Terrain *m_Terrain;
		graphics::MeshFactory m_meshFactory;
		graphics::Skybox *m_Skybox;
		graphics::DynamicLightManager m_DynamicLightManager;

		// Some sort of list of entities (tied to models that are in the Renderer (Renderable3D) (should this name be changed to Renderer3D?))
		//std::vector<Entity*> m_Entities;
		std::vector<graphics::Renderable3D*> m_Renderables;

		graphics::Shader m_TerrainShader, m_ModelShader, m_OutlineShader;
	public:
		Scene3D(graphics::Camera *camera, graphics::Window *window);
		~Scene3D();
		
		void Add(graphics::Renderable3D *renderable);

		void onUpdate(float deltaTime);
		void onRender();

		inline graphics::Renderer* getRenderer() const { return m_Renderer; }
		inline graphics::Camera* getCamera() const { return m_Camera; }
	private:
		void init();
	};

}