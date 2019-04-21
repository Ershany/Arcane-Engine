#include "pch.h"
#include "PathfindingAgent.h"

#include <input/InputManager.h>
#include <utils/PathfindingUtil.h>

namespace arcane
{
	PathfindingAgent::PathfindingAgent(Terrain* terrain, RenderableModel* agentsModel) : m_Terrain(terrain), m_Model(agentsModel), m_ShouldMove(false)
	{
		
	}

	PathfindingAgent::~PathfindingAgent()
	{
	}

	void PathfindingAgent::update(float deltaTime) {
		if (m_ShouldMove && PathfindingUtil::s_Path.size() > 0) {
			glm::vec3 dest = PathfindingUtil::FindCenterTriangle(*PathfindingUtil::s_Path[m_MovementIndex]->triangle);

			// Do movement
			glm::vec3 velocity = (dest - m_Model->getPosition()) * m_MovementSpeed;
			m_Model->setPosition(m_Model->getPosition() + velocity);

			// Check if it reaches the current destination, if it does update the index
			if (glm::length2(m_Model->getPosition() - dest) < 1.0f) {
				m_MovementIndex -= 1;
				if (m_MovementIndex == -1) {
					m_ShouldMove = false;
				}
			}
		}

		// Set the height of the model to the terrain's bilinear sample
		glm::vec3 pos = m_Terrain->sampleHeightfieldBilinear(m_Model->getPosition());
		m_Model->setPosition(pos);
	}

	void PathfindingAgent::resetPath() {
		m_MovementIndex = PathfindingUtil::s_Path.size() - 1;
		m_ShouldMove = true;
	}
}
