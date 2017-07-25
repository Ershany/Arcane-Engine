#pragma once

#include "graphics\camera\FPSCamera.h"
#include "graphics\renderer\Renderer.h"

namespace arcane {
	
	class Scene3D {
	private:
		graphics::FPSCamera *m_Camera;
		graphics::Renderer *m_Renderer;

		// Some sort of list of entities (tied to models that are in the Renderer (should this be changed to Renderer3D?))
		//std::vector<Entity*> m_Entities;
	public:
		Scene3D(glm::mat4 &projectionMatrix);
		~Scene3D();

		//void Add(Entity *e)

		void onUpdate();
		void onRender();

		inline graphics::Renderer* getRenderer() { return m_Renderer; }
		inline graphics::FPSCamera* getCamera() { return m_Camera; }
	};

}