#include "pch.h"
#include "PathfindingAgent.h"

#include <input/InputManager.h>
#include <utils/PathfindingUtil.h>

namespace arcane
{
	PathfindingAgent::PathfindingAgent(Terrain* terrain, RenderableModel *agentsModel) : m_Terrain(terrain), m_Model(agentsModel)
	{
		
	}

	PathfindingAgent::~PathfindingAgent()
	{
	}

	void PathfindingAgent::update(float deltaTime) {
		// Teleports the agent to the end of the path
		if (PathfindingUtil::s_Path.size() > 0) {
			m_Model->setPosition(PathfindingUtil::FindCenterTriangle(*PathfindingUtil::s_Path[0]->triangle));
		}

		// Set the height of the model to the terrain's bilinear sample
		glm::vec3 pos = m_Terrain->sampleHeightfieldBilinear(m_Model->getPosition());
		m_Model->setPosition(pos);
	}
}
