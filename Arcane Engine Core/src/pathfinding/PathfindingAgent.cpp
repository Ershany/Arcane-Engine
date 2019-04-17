#include "pch.h"
#include "PathfindingAgent.h"

namespace arcane
{
	PathfindingAgent::PathfindingAgent(Terrain* terrain, RenderableModel *agentsModel) : m_Terrain(terrain), m_Model(agentsModel)
	{
	}

	PathfindingAgent::~PathfindingAgent()
	{
	}

	void PathfindingAgent::update(float deltaTime) {
		// Move the agent (for testing purposes)
		 glm::vec3 newPos = m_Model->getPosition() + glm::vec3(m_MovementSpeedPerSec * deltaTime, 0.0f, m_MovementSpeedPerSec * deltaTime);
		float terrainHeight = m_Terrain->sampleHeightfieldBilinear(newPos);

		// Update the elevation of the agent, so it sits on the terrain in its current position
		m_Model->setPosition(glm::vec3(newPos.x, terrainHeight + (m_Model->getScale().y * 0.5f), newPos.z));
	}
}
