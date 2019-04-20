#pragma once

#include <scene/RenderableModel.h>
#include <terrain/Terrain.h>

namespace arcane
{
	class PathfindingAgent
	{
	public:
		glm::vec3 destination;
		glm::vec3 intermediaryDest;

		PathfindingAgent(Terrain* terrain, RenderableModel *agentsModel);
		~PathfindingAgent();
		
		void update(float deltaTime);

		inline const glm::vec3& getPosition() { return m_Model->getPosition(); }
	private:
		Terrain* m_Terrain;
		RenderableModel *m_Model;

		float m_MovementSpeedPerSec = 20.0f;
	};
}
