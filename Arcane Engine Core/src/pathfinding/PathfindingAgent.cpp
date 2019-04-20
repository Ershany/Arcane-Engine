#include "pch.h"
#include "PathfindingAgent.h"

#include <input/InputManager.h>

namespace arcane
{
	PathfindingAgent::PathfindingAgent(Terrain* terrain, RenderableModel *agentsModel) : m_Terrain(terrain), m_Model(agentsModel)
	{
		
	}

	PathfindingAgent::~PathfindingAgent()
	{
	}

	void PathfindingAgent::update(float deltaTime) {
		glm::vec3 pos = m_Terrain->sampleHeightfieldBilinear(m_Model->getPosition());
		m_Model->setPosition(pos);
	}
}
